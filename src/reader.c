#include "bitsmanip.h"
#include "reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define SOI 0xffd8
void read_jpeg(const char* file_name,IMAGE*image){
    BitStream* bs=bitstream_init(file_name);
    read_soi(bs,image);
}
void read_soi(BitStream*bs,IMAGE*image){
    uint16_t marqueur=bitstream_read_bits(bs,16);
    if(marqueur!=SOI){
        printf("ERROR SOI\n");
        exit(0);
    }
    
}