#include <stdlib.h>
#include <stdio.h>
#include "bitsmanip.h"


// Fonction pour initialiser une structure de  BitStram 
BitStream* bitstream_init(const char* filename) {
    BitStream* bs = (BitStream*)malloc(sizeof(BitStream));
    if (!bs) return NULL;
    bs->fp = fopen(filename, "rb");
    if (!get_fp(*bs)) {
        free(bs);
        return NULL;
    }
    bs->buffer = 0;
    bs->bits_available = 0;
    bs->bytes_read = 0;
    bs->is_eof = 0;
    return bs;
}
// Fonction pour fermer une structure de BitStream
void bitstream_close(BitStream* bs) {
    if (bs) {
        if (get_fp(*bs)) {
            fclose(bs->fp);
        }
        free(bs);
    }
}

// Fonction pour remplir Buffer des données du fichier 
static void bitstream_fill_buffer(BitStream* bs) {
    if (get_bits_available(*bs) == 0 && !get_is_eof(*bs)) {
        int byte = fgetc(get_fp(*bs));
        if (byte == EOF) {
            bs->is_eof = 1;
        } else {
            bs->buffer = (uint8_t)byte;
            bs->bits_available = 8;
            bs->bytes_read++;
        }
    }
}
// Fonction pour lire un bit du Buffer 
uint8_t bitstream_read_bit(BitStream* bs) {
    bitstream_fill_buffer(bs);
    if (get_bits_available(*bs) == 0) {
        return 0; // EOF ou error
    }
    // Obtenir le bit de poids fort 
    uint8_t bit = (get_buffer(*bs) >> 7) & 1;
    // Décalage à gauche par 1
    bs->buffer <<= 1;
    bs->bits_available--;
    return bit;
}

// Fonction pour lire n bits du Buffer
uint32_t bitstream_read_bits(BitStream* bs, int n) {
    uint32_t result = 0;
    for (int i = 0; i < n; i++) {
        result = (result << 1) | bitstream_read_bit(bs);//met 8 zéros et remplace par 1 les bits non nuls 
    }
    return result;
}
// Fonction pour voir n bits du Buffer sans les consommer 
uint32_t bitstream_peek_bits(BitStream* bs, int n) {
    // Sauvegarder l'état courant
    uint8_t saved_buffer = get_buffer(*bs);
    int saved_bits = get_bits_available(*bs);
    long saved_pos = ftell(get_fp(*bs));// ou bs->bytes_read
    int saved_eof = get_is_eof(*bs);
    // lire bits
    uint32_t result = bitstream_read_bits(bs, n);
    // Restorer l'état 
    bs->buffer = saved_buffer;
    bs->bits_available = saved_bits;
    fseek(bs->fp, saved_pos, SEEK_SET);
    bs->is_eof = saved_eof;
    return result;
}


// Fonction pour avoir fp du BitStream
FILE*get_fp(BitStream bs){
    return bs.fp;
}

// Fonction pour avoir buffer  du BitStream
uint8_t get_buffer(BitStream bs){
    return bs.buffer;

}
// Fonction pour avoir les bits disponibles du BitStream 
int get_bits_available(BitStream bs){
    return bs.bits_available;
}
// Fonction pour avoir les octets lues du BitStream
long get_bytes_read(BitStream bs){
    return bs.bytes_read;
}
// Fonction pour avoir la valeur de is_oef 
int get_is_eof(BitStream bs){
    return bs.is_eof;
}

// Test des fonctions

// int main(){
//     BitStream*bs=bitstream_init("images/invader.jpeg");
//     uint16_t byte1=bitstream_read_bits(bs,16);
//     while(!get_is_eof(*bs)){
//         printf("%04x\n",byte1);
//         byte1=bitstream_read_bits(bs,16);
//     }
//     return 0;
// }