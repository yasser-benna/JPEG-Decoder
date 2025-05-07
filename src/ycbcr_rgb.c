#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ycbcr_rgb.h"

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