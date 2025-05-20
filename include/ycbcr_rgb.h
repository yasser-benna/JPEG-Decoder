#pragma once
#include <stdint.h>
#include "reader.h"

// Fonction pour convertir YCbCr en RGB
void YCbCr_to_rgb(uint8_t ***Y_bloc, uint8_t ***Cb_bloc, uint8_t ***Cr_bloc, 
                 uint8_t ***R, uint8_t ***G, uint8_t ***B, 
                 int H_Y, int V_Y, int nb_blocks, int nb_block_C, int sampling);

