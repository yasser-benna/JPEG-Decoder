#pragma once
#include <stdint.h>
#include "reader.h"

void YCbCr_to_rgb(uint8_t ***Y_blocks, uint8_t **Cb, uint8_t **Cr, 
                 uint8_t ***R, uint8_t ***G, uint8_t ***B, 
                 int H_Y, int V_Y, int nb_blocks, int sampling);

