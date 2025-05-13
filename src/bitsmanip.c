#include <stdlib.h>
#include <stdio.h>
#include "bitsmanip.h"


// Function to initialize a BitStream structure
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
// Function to close a BitStream structure
void bitstream_close(BitStream* bs) {
    if (bs) {
        if (get_fp(*bs)) {
            fclose(bs->fp);
        }
        free(bs);
    }
}

// Function to fill the buffer with data from the file
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
// Function to read a single bit from the buffer
uint8_t bitstream_read_bit(BitStream* bs) {
    bitstream_fill_buffer(bs);
    if (get_bits_available(*bs) == 0) {
        return 0; // EOF or error
    }
    // Get the most significant bit
    uint8_t bit = (get_buffer(*bs) >> 7) & 1;
    // Shift buffer left by 1
    bs->buffer <<= 1;
    bs->bits_available--;
    return bit;
}

// Function to read n bits from the buffer
uint32_t bitstream_read_bits(BitStream* bs, int n) {
    uint32_t result = 0;
    for (int i = 0; i < n; i++) {
        result = (result << 1) | bitstream_read_bit(bs);//8(0)and sets the ones
    }
    return result;
}
// Function to peek n bits from the buffer without consuming them
uint32_t bitstream_peek_bits(BitStream* bs, int n) {
    // Save current state
    uint8_t saved_buffer = get_buffer(*bs);
    int saved_bits = get_bits_available(*bs);
    long saved_pos = ftell(get_fp(*bs));// or bs->bytes_read
    int saved_eof = get_is_eof(*bs);
    // Read bits
    uint32_t result = bitstream_read_bits(bs, n);
    // Restore state
    bs->buffer = saved_buffer;
    bs->bits_available = saved_bits;
    fseek(bs->fp, saved_pos, SEEK_SET);
    bs->is_eof = saved_eof;
    return result;
}


// fuction to get fp from BitStream
FILE*get_fp(BitStream bs){
    return bs.fp;
}

// function to get buffer from BitStream
uint8_t get_buffer(BitStream bs){
    return bs.buffer;

}
// function to get bits_available from BitStream
int get_bits_available(BitStream bs){
    return bs.bits_available;
}
// function to get bytes_read from BitStream
long get_bytes_read(BitStream bs){
    return bs.bytes_read;
}
// function to get is_eof from BitStream
int get_is_eof(BitStream bs){
    return bs.is_eof;
}

// Test functions

// int main(){
//     BitStream*bs=bitstream_init("images/invader.jpeg");
//     uint16_t byte1=bitstream_read_bits(bs,16);
//     while(!get_is_eof(*bs)){
//         printf("%04x\n",byte1);
//         byte1=bitstream_read_bits(bs,16);
//     }
//     return 0;
// }