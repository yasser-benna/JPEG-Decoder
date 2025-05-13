#pragma once
#include <stdint.h>
typedef struct{
    uint8_t*R;
    uint8_t*B;
    uint8_t*G;
    int width;
    int height;
    int colors;
}IMAGE_D;
void free_image(IMAGE_D* image);
void write_image(const char* file_name,IMAGE_D image);
IMAGE_D* init_image_d();