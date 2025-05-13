#include "bitsmanip.h"
#include "reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// marqueurs
#define SOI 0xffd8
#define EOI 0xffd9
#define APP0 0xffe0
#define COM 0xfffe
#define DQT 0xffdb
#define SOF0 0xffc0
#define DHT 0xffc4
#define SOS 0xffda


IMAGE* init_image() {
    IMAGE* img = malloc(sizeof(IMAGE));
    if (!img) {
        printf("Erreur d'allocation IMAGE\n");
        exit(1);
    }
    img->Largeur = 0;
    img->Hauteur = 0;
    img->nb_components = 0;
//    img->COMPONENTS = NULL;
    img->Quant_Table[0] = NULL;
    img->Quant_Table[1] = NULL;
    img->Quant_Table[2] = NULL;
    img->Quant_Table[3] = NULL;
    img->COMMENTAIRE = NULL;
    img->APPX = 0;
    img->nb_components_sos = 0;
    img->Ss = 0;
    img->Se = 0;
    img->Ah = 0;
    img->Al = 0;
    for (int i = 0; i < 4; i++) {
        img->HUFFMAN_tables[i].nb_symbols = 0;
    }
    img->compressed_data = NULL;
    img->compressed_size = 0;
    return img;
}


IMAGE* read_jpeg(const char* file_name){
    BitStream* bs=bitstream_init(file_name);
    IMAGE*image=init_image();
    uint16_t MARQUEUR=0;
    read_soi(bs);
    MARQUEUR=bitstream_peek_bits(bs,16);
    do{ 
        switch(MARQUEUR){
            case APP0:
                read_appx(bs,image);
                break;
            case COM:
                read_com(bs,image);
                break;
            case DQT:
                read_dqt(bs,image);
                break;
            case SOF0:
                read_sofx(bs,image);
                break;
            case DHT:
                read_dht(bs,image);
                break;
            case SOS:
                read_sos(bs,image);
                break;
            default:
                printf("ERROR MARQUEUR JPEG\n");
                exit(0);
        }
        MARQUEUR=bitstream_peek_bits(bs,16);
    }while (MARQUEUR!=EOI);
    read_eoi(bs);
    bitstream_close(bs);
    return image;
}
void read_soi(BitStream*bs){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOI){
        printf("ERROR SOI\n");
        exit(0);
    } 
}

void read_eoi(BitStream*bs){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=EOI){
        printf("ERROR EOI\n");
        exit(0);
    }
}

void read_appx(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=APP0){
        printf("ERROR APP0\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint8_t byter=bitstream_read_bits(bs,8);
    if(byter!=0x4a){
        printf("ERROR APPX J\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x46){
        printf("ERROR APPX F\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x49){
        printf("ERROR APPX I\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x46){
        printf("ERROR APPX F\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x00){
        printf("ERROR APPX 00\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x01){
        printf("ERROR APPX 01\n");
        exit(0);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x01){
        printf("ERROR APPX 01\n");
        exit(0);
    }
    image->APPX=0;
    bitstream_read_bits(bs,(section_size-9)*8);
}
void read_com(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=COM){
        printf("ERROR COM\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    image->COMMENTAIRE=malloc((section_size-1)*sizeof(char));
    uint8_t byter=0;
    for(int16_t i=0;i<section_size-2;i++){
        byter=bitstream_read_bits(bs,8);
        image->COMMENTAIRE[i]=(char)byter;
    }
    image->COMMENTAIRE[section_size-2]='\0';
}
void read_dqt(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=DQT){
        printf("ERROR DQT\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16)-2;
    uint8_t table_nb=(uint8_t)section_size/65;
    uint16_t byter=0;
    uint8_t precision=0;
    uint8_t table_id=0;
    int byter_size=0;
    for(uint8_t j=0;j<table_nb;j++){
        precision=bitstream_read_bits(bs,4);
        table_id=bitstream_read_bits(bs,4);
        if(table_id>3){
            printf("ERROR: Invalid quantization table ID %d\n", table_id);
            exit(0);
        }
        // unuseful but maybe in progressive
        if(precision==0){
            byter_size=8;
        }else{
            byter_size=16;
        }
        image->Quant_Table[table_id]=malloc(64*sizeof(uint8_t));
        for(uint8_t i=0;i<64;i++){
            byter=bitstream_read_bits(bs,byter_size);
            image->Quant_Table[table_id][i]=byter;
        }

    }
}
void read_sofx(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOF0){
        printf("ERROR SOF0\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint16_t section_counter=2;
    uint8_t precision=bitstream_read_bits(bs,8);
    section_counter++;
    if(precision!=8){
        printf("ERROR PRECISON SOFX\n");
        exit(0);
    }
    image->Hauteur=bitstream_read_bits(bs,16);
    image->Largeur=bitstream_read_bits(bs,16);
    image->nb_components+=bitstream_read_bits(bs,8);
    section_counter+=5;
    if(image->nb_components>3){
        printf("ERROR SOFX NB_COMPONENTS\n");
        exit(0);
    }
//    image->COMPONENTS=malloc(image->nb_components * sizeof(COMPONENT));
    for(uint8_t i=0;i<image->nb_components;i++){
        (&(image->COMPONENTS[i]))->id=bitstream_read_bits(bs,8);
        (&(image->COMPONENTS[i]))->h_factor=bitstream_read_bits(bs,4);
        (&(image->COMPONENTS[i]))->v_factor=bitstream_read_bits(bs,4);
        (&(image->COMPONENTS[i]))->qt_id=bitstream_read_bits(bs,8);
        section_counter+=3;
    }
    if(section_counter!=section_size){
        printf("ERROR SOF0 section size mismatch\n");
        exit(0);
    }

}
void read_dht(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=DHT){
        printf("ERROR DHT\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint16_t byter=2;
    uint8_t AC_DC=0;
    uint8_t table_id=0;
    HUFFMAN_TABLE*current_table;
    int total_symbols = 0;
    while(byter<section_size){
        if(bitstream_read_bits(bs,3)!=0){
        printf("ERROR DHT 000\n");
        exit(0);
        }
        AC_DC=bitstream_read_bit(bs);
        table_id=bitstream_read_bits(bs,4);
        byter++;
        if(table_id>3){
            printf("ERROR DHT TABLE ID %u\n",table_id);
            exit(0);
        }
        current_table = &(image->HUFFMAN_tables[table_id + (AC_DC *2)]);
        current_table->id = table_id;
        current_table->ac_dc = AC_DC;
        total_symbols = 0;
        for(int i=0; i<16; i++){
            current_table->tailles[i] = bitstream_read_bits(bs,8);
            total_symbols += current_table->tailles[i];
            byter++;
        }
        current_table->nb_symbols = total_symbols;
        if(total_symbols > 256){
            printf("ERROR: Too many symbols in Huffman table\n");
            exit(0);
        }
        for(int i=0; i<total_symbols; i++){
            current_table->symbols[i] = bitstream_read_bits(bs,8);
            byter++;
        }
    }
}
void read_sos(BitStream*bs,IMAGE*image){
    uint16_t marqueur =0;
    marqueur= bitstream_read_bits(bs, 16);
    if(marqueur != SOS){
        printf("ERROR SOS\n");
        exit(0);
    }
    uint16_t section_size = bitstream_read_bits(bs, 16);
    uint16_t section_counter=2;
    uint8_t nb_components = bitstream_read_bits(bs, 8);
    for(uint8_t i = image->nb_components_sos; i < nb_components+image->nb_components_sos; i++){
        image->COMPONENTS_SOS[i].id = bitstream_read_bits(bs, 8);
        image->COMPONENTS_SOS[i].dc_table_id = bitstream_read_bits(bs, 4);
        image->COMPONENTS_SOS[i].ac_table_id = bitstream_read_bits(bs, 4);
        section_counter += 2;
    }
    image->nb_components_sos += nb_components;
    image->Ss = bitstream_read_bits(bs, 8);
    image->Se = bitstream_read_bits(bs, 8);
    image->Ah = bitstream_read_bits(bs, 4);
    image->Al = bitstream_read_bits(bs, 4);
    section_counter += 4;
    if(section_counter != section_size){
        printf("ERROR SOS section size mismatch\n");
        exit(0);
    }

    size_t compressed_data_buffer_size = 1024;
    uint8_t* holding_buffer = malloc(compressed_data_buffer_size*sizeof(uint8_t));
    size_t position = 0;
    uint8_t byter=0;
    while(marqueur!=EOI){
        byter = bitstream_read_bits(bs, 8);
        if (position >= compressed_data_buffer_size) {
            compressed_data_buffer_size *= 2;
            holding_buffer = realloc(holding_buffer, compressed_data_buffer_size);
        }
        holding_buffer[position++] = byter;
        marqueur = bitstream_peek_bits(bs, 16);
    }
    image->compressed_data=holding_buffer;
    image->compressed_size=position;
}


void print_image(const IMAGE* image) {
    printf("Largeur: %d\n", image->Largeur);
    printf("Hauteur: %d\n", image->Hauteur);
    printf("Nombre de composantes: %d\n", image->nb_components);
    for (int i = 0; i < image->nb_components; i++) {
        printf("  Composante %d:\n", i);
        printf("    id: %d\n", image->COMPONENTS[i].id);
        printf("    h_factor: %d\n", image->COMPONENTS[i].h_factor);
        printf("    v_factor: %d\n", image->COMPONENTS[i].v_factor);
        printf("    qt_id: %d\n", image->COMPONENTS[i].qt_id);
    }
    for (int i = 0; i < 4; i++) {
        if (image->Quant_Table[i]) {
            printf("Quant_Table[%d]: \n", i);
            printf("           ");
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    printf("%3d ", image->Quant_Table[i][j * 8 + k]);
                }
                printf("\n           ");
            }
            printf("\n");
        }
    }
    for (int i = 0; i < 4; i++) {
        const HUFFMAN_TABLE* ht = &image->HUFFMAN_tables[i];
        if (ht->nb_symbols > 0) {
            printf("Huffman Table %d (%s):\n", i, (i < 2) ? "DC" : "AC");
            printf("  id: %d, ac_dc: %d, nb_symbols: %d\n", ht->id, ht->ac_dc, ht->nb_symbols);
            printf("  Tailles: ");
            for (int j = 0; j < 16; j++) {
                printf("%d ", ht->tailles[j]);
            }
            printf("\n  Symbols: ");
            for (int j = 0; j < ht->nb_symbols; j++) {
                printf("%02x ", ht->symbols[j]);
            }
            printf("\n");
        }
    }

    printf("Section SOS:\n");
    printf("  Nb composants SOS: %d\n", image->nb_components_sos);
    for (int i = 0; i < image->nb_components_sos; i++) {
        printf("    Composant %d: id=%d, dc_table_id=%d, ac_table_id=%d\n",
            i,
            image->COMPONENTS_SOS[i].id,
            image->COMPONENTS_SOS[i].dc_table_id,
            image->COMPONENTS_SOS[i].ac_table_id
        );
    }
    printf("  Ss: %d, Se: %d, Ah: %d, Al: %d\n", image->Ss, image->Se, image->Ah, image->Al);

    printf("Compressed data (taille: %zu):\n", image->compressed_size);
    size_t to_print = image->compressed_size < 32 ? image->compressed_size : 32;
    for (size_t i = 0; i < to_print; i++) {
        printf("%02x ", image->compressed_data[i]);
    }
    if (image->compressed_size > to_print) {
        printf("...");
    }
    printf("\n");


    if (image->COMMENTAIRE) {
        printf("Commentaire: %s\n", image->COMMENTAIRE);
    }
}

// int main() {

//     IMAGE* image=read_jpeg("./images/invader.jpeg");
//     print_image(image);

//     printf("Lecture du JPEG termine.\n");
//     return 0;
// }