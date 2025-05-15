#pragma once
#include <stdint.h>
#include "reader.h"

void YCbCr_to_rgb(unsigned char ***Y_blocks, unsigned char **Cb, unsigned char **Cr, 
                 unsigned char ***R, unsigned char ***G, unsigned char ***B, 
                 int H_Y, int V_Y, int nb_blocks, int sampling);
void remplir_YCbCr_matrices(uint8_t **Y_matrix, uint8_t **Cb_matrix, uint8_t **Cr_matrix,
                            unsigned char ****Y_final, unsigned char ****Cb_final, unsigned char ****Cr_final,
                            IMAGE *image, int nb_mcus_x, int nb_mcus_y,
                            int H_Y, int V_Y, int H_C, int V_C) ;

void remplir_matrice(uint8_t **matrice, unsigned char ****blocs, 
                     int nb_mcus_x, int nb_mcus_y, 
                     int H_fact, int V_fact, 
                     int largeur, int hauteur) ;
