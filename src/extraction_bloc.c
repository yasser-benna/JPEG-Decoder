#include <stdio.h>  
#include <stdlib.h> 
#include <stdint.h>   
#include <string.h> 
#include "../include/huffman.h"
#include "../include/bitsmanip.h"
#include "../include/extraction_bloc.h"
#include "../include/reader.h"

/*Cette fonction permet de décoder l'ensemble des MCUs  de l'image JPEG compressée,et en extrait les blocs 
de coefficients pour chaque composante Y , Cb et Cr en utilisant la fonction décoder bloc qui s'applique 
seulement sur un bloc 8x8.*/

void extraire_bloc(const uint8_t *data, size_t *bit_pos,
                   char **table_huffman_dc_Y, uint8_t symboles_dc_Y[256], int nbr_sym_dc_Y,
                   char **table_huffman_ac_Y, uint8_t symboles_ac_Y[256], int nbr_sym_ac_Y,
                   char **table_huffman_dc_C, uint8_t symboles_dc_C[256], int nbr_sym_dc_C,
                   char **table_huffman_ac_C, uint8_t symboles_ac_C[256], int nbr_sym_ac_C,
                   int H_Y, int H_Cb,int H_Cr, int V_Y, int V_Cb,int V_Cr,
                   int largeur_image, int hauteur_image,
                   int ****Y, int ****Cb, int ****Cr)
{
    
    int mcu_largeur = H_Y * 8; // largeur d'une MCU
    int mcu_hauteur = V_Y * 8; // hauteur d'une MCU

    int mcu_cols = (largeur_image+mcu_largeur-1) / mcu_largeur; // nombre de colonnes 
    int mcu_lignes = (hauteur_image+mcu_hauteur-1) / mcu_hauteur; // nombre de lignes 
    int total_mcus = mcu_cols * mcu_lignes; // nombre totale de MCU dans l'image JPEG

    int nb_blocs_Y = H_Y * V_Y; // nombre de blocs Y dans chaque MCU
    int nb_blocs_Cb = H_Cb * V_Cb; // nombre de blocs Cb dans chaque MCU
    int nb_blocs_Cr = H_Cr * V_Cr; // nombre de blocs Cr dans chaque MCU

    // Allocation mémoire pour les coefficients des composantes
    *Y  = malloc(total_mcus * sizeof(int **));    
    *Cb = malloc(total_mcus * sizeof(int **));
    *Cr = malloc(total_mcus * sizeof(int **));
    

    // initalisation des coefficients dc_precedent par 0 
    int dc_prec_Y = 0;
    int dc_prec_Cb = 0;
    int dc_prec_Cr = 0;

    // Boucle sur le nombre de MCU
    for (int mcu = 0; mcu < total_mcus; mcu++) {
        // Allocation et décodage pour la composante Y
        (*Y)[mcu] = malloc(nb_blocs_Y * sizeof(int *));
        for (int i = 0; i < nb_blocs_Y; i++) {
            (*Y)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                         table_huffman_dc_Y, symboles_dc_Y, nbr_sym_dc_Y,
                         table_huffman_ac_Y, symboles_ac_Y, nbr_sym_ac_Y,
                         &dc_prec_Y, (*Y)[mcu][i]); //on applique la fonction decoder_bloc sur chaque bloc 8x8 de la composante Y dans chaque MCU
        }
        
        // Allocation et décodage pour la composante Cb
        (*Cb)[mcu] = malloc(nb_blocs_Cb * sizeof(int *));
        for (int i = 0; i < nb_blocs_Cb; i++) {
            (*Cb)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                            table_huffman_dc_C, symboles_dc_C, nbr_sym_dc_C,
                            table_huffman_ac_C, symboles_ac_C, nbr_sym_ac_C,
                            &dc_prec_Cb, (*Cb)[mcu][i]); //on applique la fonction decoder_bloc sur chaque bloc 8x8 de la composante Cn dans chaque MCU
        }

        // Allocation et décodage pour la composante Cr
        (*Cr)[mcu] = malloc(nb_blocs_Cr * sizeof(int *));
        for (int i = 0; i < nb_blocs_Cr; i++) {
            (*Cr)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                            table_huffman_dc_C, symboles_dc_C, nbr_sym_dc_C,
                            table_huffman_ac_C, symboles_ac_C, nbr_sym_ac_C,
                            &dc_prec_Cr, (*Cr)[mcu][i]); //on applique la fonction decoder_bloc sur chaque bloc 8x8 de la composante Cr dans chaque MCU
        }
        
    }

}

