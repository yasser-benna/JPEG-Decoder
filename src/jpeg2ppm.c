#include "huffman.h"
#include "bitsmanip.h"
#include "writter.h"
#include "reader.h"
#include "iqzz.h"
#include "zigzag.h"
#include "idct.h"
#include "ycbcr_rgb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc,char* argv[]){
    if(argc !=2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    IMAGE* image = read_jpeg(argv[1]);
    IMAGE_D* image_d = malloc(sizeof(IMAGE_D));
    char** huffman_dc=generer_codes_huffman(image->HUFFMAN_tables[0].symbols,image->HUFFMAN_tables[0].tailles,image->HUFFMAN_tables[0].nb_symbols);
    char** huffman_ac=generer_codes_huffman(image->HUFFMAN_tables[2].symbols,image->HUFFMAN_tables[2].tailles,image->HUFFMAN_tables[2].nb_symbols);
    int bloc[64]={0};
    int dc_precedent=0;
    size_t bit_pos = 0;
    decoder_bloc(
        image->compressed_data,&bit_pos,
        huffman_dc,image->HUFFMAN_tables[0].symbols,image->HUFFMAN_tables[0].nb_symbols,
        huffman_ac,image->HUFFMAN_tables[2].symbols,image->HUFFMAN_tables[2].nb_symbols,
        &dc_precedent,bloc);
    for (int i = 0; i < 64; i++) {
        printf("%04hx ", (uint16_t)bloc[i]);  // affichage hex sur 4 digits
    if ((i + 1) % 8 == 0) printf("\n");
    }
    quantification_inverse(bloc,image->Quant_Table[0]);
    printf("Quantification inverse:\n");
    for (int i = 0; i < 64; i++) {
        printf("%04hx ", (uint16_t)bloc[i]);  // affichage hex sur 4 digits
        if ((i + 1) % 8 == 0) printf("\n");
    }
    int** bloc_ap=malloc(8*sizeof(int*));
    for(int i=0;i<8;i++){
        bloc_ap[i]=malloc(8*sizeof(int));
    }
    inverse_zigzag(bloc,bloc_ap);
    printf("Inverse zigzag:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%04hx ", (uint16_t)bloc_ap[i][j]);  // affichage hex sur 4 digits
        }
        printf("\n");
    }
    unsigned char** spatial_block;
    idct_ptr(bloc_ap,&spatial_block);
    printf("IDCT:\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%3d ", spatial_block[i][j]);  // affichage hex sur 4 digits
        }
        printf("\n");
    }
    return 0;
}