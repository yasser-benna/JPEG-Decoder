#pragma once
#include <stdint.h>
#include "reader.h"

void YCbCr_to_rgb(unsigned char ***Y_blocks, unsigned char **Cb, unsigned char **Cr, 
                 unsigned char ***R, unsigned char ***G, unsigned char ***B, 
                 int H_Y, int V_Y, int nb_blocks, int sampling);

