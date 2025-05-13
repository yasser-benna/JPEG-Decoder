#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ycbcr_rgb.h"
#include "reader.h"

void YCbCr_to_rgb(uint8_t **Y, uint8_t **Cb, uint8_t **Cr, uint8_t **R, uint8_t **G, uint8_t **B, uint8_t l, uint8_t c) {
    for (uint8_t ligne = 0; ligne < l; ligne++) {
        for (uint8_t colonne = 0; colonne < c; colonne++) {
            // Calcul des valeurs RGB
            R[ligne][colonne] = Y[ligne][colonne] + 1.402 * (Cr[ligne][colonne] - 128);
            G[ligne][colonne] = Y[ligne][colonne] - 0.34414 * (Cb[ligne][colonne] - 128) - 0.71414 * (Cr[ligne][colonne] - 128);
            B[ligne][colonne] = Y[ligne][colonne] + 1.772 * (Cb[ligne][colonne] - 128);

            // saturation des valeurs entre 0 et 255
            if (R[ligne][colonne] < 0){
                R[ligne][colonne] = 0;
            } 
            else if (R[ligne][colonne] > 255){
                R[ligne][colonne] = 255;
            }

            if (G[ligne][colonne] < 0){
                G[ligne][colonne] = 0;
            } 
            else if (G[ligne][colonne] > 255){
                G[ligne][colonne] = 255;
            }

            if (B[ligne][colonne] < 0){
                B[ligne][colonne] = 0;
            } 
            else if (B[ligne][colonne] > 255){
                B[ligne][colonne] = 255;
            }
        }
    }
}



void remplir_matrice(uint8_t **matrice, unsigned char ****blocs, 
                     int nb_mcus_x, int nb_mcus_y, 
                     int H_fact, int V_fact, 
                     int largeur, int hauteur) 
{
    for (int mcu_y = 0; mcu_y < nb_mcus_y; mcu_y++) {
        for (int mcu_x = 0; mcu_x < nb_mcus_x; mcu_x++) {
            int mcu_index = mcu_y * nb_mcus_x + mcu_x;
            for (int bloc_y = 0; bloc_y < V_fact; bloc_y++) {
                for (int bloc_x = 0; bloc_x < H_fact; bloc_x++) {
                    int bloc_index = bloc_y * H_fact + bloc_x;
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            int y = mcu_y * V_fact * 8 + bloc_y * 8 + i;
                            int x = mcu_x * H_fact * 8 + bloc_x * 8 + j;
                            if (y < hauteur && x < largeur) {
                                matrice[y][x] = blocs[mcu_index][bloc_index][i][j];
                            }
                        }
                    }
                }
            }
        }
    }
}

void remplir_YCbCr_matrices(uint8_t **Y_matrix, uint8_t **Cb_matrix, uint8_t **Cr_matrix,
                            unsigned char ****Y_final, unsigned char ****Cb_final, unsigned char ****Cr_final,
                            IMAGE *image, int nb_mcus_x, int nb_mcus_y,
                            int H_Y, int V_Y, int H_C, int V_C) 
{
    
    remplir_matrice(Y_matrix, Y_final, nb_mcus_x, nb_mcus_y, H_Y, V_Y, image->Largeur, image->Hauteur);

    // Remplir Cb et Cr si image couleur
    if (image->nb_components ==3) {
        remplir_matrice(Cb_matrix, Cb_final, nb_mcus_x, nb_mcus_y, H_C, V_C, image->Largeur, image->Hauteur);
        remplir_matrice(Cr_matrix, Cr_final, nb_mcus_x, nb_mcus_y, H_C, V_C, image->Largeur, image->Hauteur);
    }
}
