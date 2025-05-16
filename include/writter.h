#pragma once
#include <stdint.h>
#include <stdio.h>
typedef struct{
    uint8_t** R;
    uint8_t** B;
    uint8_t** G;
    int width;
    int height;
    int colors;
}IMAGE_D;

typedef struct{
    uint8_t tab[8][8];


}cell;
void free_image_d(IMAGE_D* image);
void write_image_d(FILE*file,IMAGE_D image);
IMAGE_D* init_image_d(int width,int height,int colors);
void copy_mcu_to_image(IMAGE_D* image_d, unsigned char** R, unsigned char** G, 
                       unsigned char** B, int mcu, int nb_mcus_x, 
                       int H_Y, int V_Y);
