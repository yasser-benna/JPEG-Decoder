#pragma once
#include <stdint.h>
#include "reader.h"

void YCbCr_to_rgb(uint8_t **Y, uint8_t **Cb, uint8_t **Cr, uint8_t **R, uint8_t **G, uint8_t **B, uint8_t l, uint8_t c);
void remplir_YCbCr_matrices(uint8_t **Y_matrix, uint8_t **Cb_matrix, uint8_t **Cr_matrix,
                            unsigned char ****Y_final, unsigned char ****Cb_final, unsigned char ****Cr_final,
                            IMAGE *image, int nb_mcus_x, int nb_mcus_y,
                            int H_Y, int V_Y, int H_C, int V_C) ;

void remplir_matrice(uint8_t **matrice, unsigned char ****blocs, 
                     int nb_mcus_x, int nb_mcus_y, 
                     int H_fact, int V_fact, 
                     int largeur, int hauteur) ;
