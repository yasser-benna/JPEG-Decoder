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

void read_jpeg(const char* file_name){
    BitStream* bs=bitstream_init(file_name);
    IMAGE*image=init_image();// a implementer
    read_soi(bs,image);
}
void read_soi(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOI){
        printf("ERROR SOI\n");
        exit(0);
    } 
    read_appx(bs,image);
    read_eoi(bs,image);
}

void read_eoi(BitStream*bs,IMAGE*image){
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
    bitstream_read_bits(bs,section_size-11);
    read_com(bs,image);
}
void read_com(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=COM){
        printf("ERROR COM\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    image->COMMENTAIRE=malloc((section_size-1*sizeof(char)));
    uint8_t byter=0;
    for(int16_t i=0;i<section_size-2;i++){
        byter=bitstream_read_bits(bs,8);
        image->COMMENTAIRE[i]=(char)byter;
    }
    image->COMMENTAIRE[section_size-2]='\0';
    read_dqt(bs,image);
}
void read_dqt(BitStream*bs,IMAGE*image){
    uint16_t marqueur=0;
    marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=DQT){
        printf("ERROR DQT\n");
        exit(0);
    }
    uint16_t section_size=bitstream_read_bits(bs,16);
    uint16_t byter=0;
    uint8_t precision=bitstream_read_bits(bs,4);
    uint8_t table_id=bitstream_read_bits(bs,4);
    if(table_id>3){
        printf("ERROR: Invalid quantization table ID %d\n", table_id);
        exit(0);
    }

    while (byter<section_size-2){

    }


}