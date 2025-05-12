#pragma once
#include <stdint.h>


typedef struct COMPONENT{
    uint8_t id;
    uint8_t h_factor;
    uint8_t v_factor;
    uint8_t qt_id;
} COMPONENT;

typedef struct HUFF_TAB{
    uint8_t id;
    uint8_t ac_dc;
    uint8_t symbols[256];
    uint8_t tailles[16];
    int nb_symbols;
}HUFFMAN_TABLE;

typedef struct {
    uint8_t id;
    uint8_t dc_table_id;
    uint8_t ac_table_id;
} SOS_COMPONENT;


typedef struct IMAGE{
    int APPX;
    char* COMMENTAIRE;
    uint16_t* Quant_Table[4];
    uint16_t Hauteur;
    uint16_t Largeur;
    uint8_t nb_components;
    COMPONENT COMPONENTS[4];
    HUFFMAN_TABLE HUFFMAN_tables[4];
    uint8_t nb_components_sos;
    SOS_COMPONENT COMPONENTS_SOS[4];
    uint8_t Ss, Se;
    uint8_t Ah, Al; 
    uint8_t* compressed_data;
    size_t compressed_size;
}IMAGE;




IMAGE* init_image();
IMAGE* read_jpeg(const char* file_name);
void read_soi(BitStream*bs);
void read_eoi(BitStream*bs);
void read_appx(BitStream*bs,IMAGE*image);
void read_com(BitStream*bs,IMAGE*image);
void read_dqt(BitStream*bs,IMAGE*image);
void read_sofx(BitStream*bs,IMAGE*image);
void read_dht(BitStream*bs,IMAGE*image);
void read_sos(BitStream*bs,IMAGE*image);
void print_image(const IMAGE* image);