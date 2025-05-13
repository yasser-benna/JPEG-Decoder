#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bitsmanip.h"
#include "huffman.h"


void extraire_bloc(const uint8_t *data, size_t *bit_pos,
                   char **table_huffman_dc_Y, uint8_t symboles_dc_Y[256], int nbr_sym_dc_Y,
                   char **table_huffman_ac_Y, uint8_t symboles_ac_Y[256], int nbr_sym_ac_Y,
                   char **table_huffman_dc_C, uint8_t symboles_dc_C[256], int nbr_sym_dc_C,
                   char **table_huffman_ac_C, uint8_t symboles_ac_C[256], int nbr_sym_ac_C,
                   int H_Y, int H_C, int V_Y, int V_C,
                   int largeur_image, int hauteur_image,
                   int ****Y, int ****Cb, int ****Cr);