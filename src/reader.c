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

// Fonction pour initialiser la structure IMAGE
IMAGE* init_image() {
    IMAGE* img = malloc(sizeof(IMAGE));
    if (!img) {
        fprintf(stderr,"Erreur d'allocation IMAGE\n");
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

// fonction pour free la structure IMAGE
void free_image(IMAGE* img) {
    if (img) {
        if (img->COMMENTAIRE) {
            free(img->COMMENTAIRE);
        }
        for (int i = 0; i < 4; i++) {
            if (img->Quant_Table[i]) {
                free(img->Quant_Table[i]);
            }
        }
        free(img->compressed_data);
        free(img);
    }
}

// fonction pour lire le fichier jpeg
IMAGE* read_jpeg(const char* file_name){
    BitStream* bs=bitstream_init(file_name);
    if(bs==NULL){
        fprintf(stderr,"ERROR FILE OPEN\n");
        exit(3);

    }
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
                fprintf(stderr,"ERROR MARQUEUR JPEG %4hx\n",MARQUEUR);
                exit(3);
        }
        MARQUEUR=bitstream_peek_bits(bs,16);
    }while (MARQUEUR!=EOI);
    read_eoi(bs);
    bitstream_close(bs);
    return image;
}
// fonction pour vérifier la section SOI
void read_soi(BitStream*bs){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOI){
        fprintf(stderr,"ERROR SOI\n");
        exit(3);
    } 
}
// Fonction pour vérifier la section EOI
void read_eoi(BitStream*bs){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=EOI){
        fprintf(stderr,"ERROR EOI\n");
        exit(3);
    }
}
// Fonction pour vérifier la section APPX
void read_appx(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=APP0){
        fprintf(stderr,"ERROR APP0\n");
        exit(3);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint8_t byter=bitstream_read_bits(bs,8);
    if(byter!=0x4a){
        fprintf(stderr,"ERROR APPX J\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x46){
        fprintf(stderr,"ERROR APPX F\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x49){
        fprintf(stderr,"ERROR APPX I\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x46){
        fprintf(stderr,"ERROR APPX F\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x00){
        fprintf(stderr,"ERROR APPX 00\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x01){
        fprintf(stderr,"ERROR APPX 01\n");
        exit(3);
    }
    byter=bitstream_read_bits(bs,8);
    if(byter!=0x01){
        fprintf(stderr,"ERROR APPX 01\n");
        exit(3);
    }
    image->APPX=0;
    bitstream_read_bits(bs,(section_size-9)*8);
}
// Fonction pour vérifier la section COM
void read_com(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=COM){
        fprintf(stderr,"ERROR COM\n");
        exit(3);
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
// Fonction pour vérifier la section DQT
void read_dqt(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=DQT){
        fprintf(stderr,"ERROR DQT\n");
        exit(3);
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
            fprintf(stderr,"ERROR: Invalid quantization table ID %d\n", table_id);
            exit(3);
        }
        // inutile mais peut etre dans le mode progressif 
        if(precision==0){
            byter_size=8;
        }else{
            byter_size=16;
        }
        image->Quant_Table[table_id]=malloc(64*sizeof(uint16_t));
        for(uint8_t i=0;i<64;i++){
            byter=bitstream_read_bits(bs,byter_size);
            image->Quant_Table[table_id][i]=byter;
        }

    }
}
// Fonction pour vérifier la section SOF0
void read_sofx(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOF0){
        fprintf(stderr,"ERROR SOF0\n");
        exit(3);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint16_t section_counter=2;
    uint8_t precision=bitstream_read_bits(bs,8);
    section_counter++;
    if(precision!=8){
        fprintf(stderr,"ERROR PRECISON SOFX\n");
        exit(3);
    }
    image->Hauteur=bitstream_read_bits(bs,16);
    image->Largeur=bitstream_read_bits(bs,16);
    image->nb_components+=bitstream_read_bits(bs,8);
    section_counter+=5;
    if(image->nb_components>3){
        fprintf(stderr,"ERROR SOFX NB_COMPONENTS\n");
        exit(3);
    }
    for(uint8_t i=0;i<image->nb_components;i++){
        (&(image->COMPONENTS[i]))->id=bitstream_read_bits(bs,8);
        (&(image->COMPONENTS[i]))->h_factor=bitstream_read_bits(bs,4);
        (&(image->COMPONENTS[i]))->v_factor=bitstream_read_bits(bs,4);
        (&(image->COMPONENTS[i]))->qt_id=bitstream_read_bits(bs,8);
        section_counter+=3;
    }
    if(section_counter!=section_size){
        fprintf(stderr,"ERROR SOF0 section size mismatch\n");
        exit(3);
    }

}
// Fonction pour vérifier la section DHT
void read_dht(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=DHT){
        fprintf(stderr,"ERROR DHT\n");
        exit(3);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint16_t byter=2;
    uint8_t AC_DC=0;
    uint8_t table_id=0;
    HUFFMAN_TABLE*current_table;
    int total_symbols = 0;
    while(byter<section_size){
        if(bitstream_read_bits(bs,3)!=0){
        fprintf(stderr,"ERROR DHT 000\n");
        exit(3);
        }
        AC_DC=bitstream_read_bit(bs);
        table_id=bitstream_read_bits(bs,4);
        byter++;
        if(table_id>3){
            fprintf(stderr,"ERROR DHT TABLE ID %u\n",table_id);
            exit(3);
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
            fprintf(stderr,"ERROR: Too many symbols in Huffman table\n");
            exit(3);
        }
        for(int i=0; i<total_symbols; i++){
            current_table->symbols[i] = bitstream_read_bits(bs,8);
            byter++;
        }
    }
}
// Fonction pour vérifier la section SOS
void read_sos(BitStream*bs,IMAGE*image){
    uint16_t marqueur =0;
    marqueur= bitstream_read_bits(bs, 16);
    if(marqueur != SOS){
        fprintf(stderr,"ERROR SOS\n");
        exit(3);
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
        fprintf(stderr,"ERROR SOS section size mismatch\n");
        exit(3);
    }

    size_t compressed_data_buffer_size = 1024*1024;
    uint8_t* holding_buffer = malloc(compressed_data_buffer_size);
    size_t position = 0;
    uint8_t byter=0;
    while(marqueur!=EOI){
        byter = bitstream_read_bits(bs, 8);
        if (position == compressed_data_buffer_size) {
            compressed_data_buffer_size *= 2;
            holding_buffer = realloc(holding_buffer, compressed_data_buffer_size);
        }
        holding_buffer[position++] = byter;
        marqueur = bitstream_peek_bits(bs, 16);
    }
    image->compressed_data=holding_buffer;
    image->compressed_size=position;
}

// Fonction de test
// int main() {

//     IMAGE* image=read_jpeg("./images/invader.jpeg");
//     print_image(image);

//     printf("Lecture du JPEG termine.\n");
//     return 0;
// }