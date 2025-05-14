#include <stdio.h>  
#include <stdlib.h> 
#include <stdint.h>   
#include <string.h> 
#include "huffman.h"
#include "bitsmanip.h"
#include "extraction_bloc.h"
#include "reader.h"


void extraire_bloc(const uint8_t *data, size_t *bit_pos,
                   char **table_huffman_dc_Y, uint8_t symboles_dc_Y[256], int nbr_sym_dc_Y,
                   char **table_huffman_ac_Y, uint8_t symboles_ac_Y[256], int nbr_sym_ac_Y,
                   char **table_huffman_dc_C, uint8_t symboles_dc_C[256], int nbr_sym_dc_C,
                   char **table_huffman_ac_C, uint8_t symboles_ac_C[256], int nbr_sym_ac_C,
                   int H_Y, int H_Cb,int H_Cr, int V_Y, int V_Cb,int V_Cr,
                   int largeur_image, int hauteur_image,
                   int ****Y, int ****Cb, int ****Cr)
{
    int mcu_width = H_Y * 8;
    int mcu_height = V_Y * 8;

    int mcu_cols = (largeur_image+(largeur_image%mcu_width)) / mcu_width;
    int mcu_rows = (hauteur_image+(largeur_image%mcu_height)) / mcu_height;
    int total_mcus = mcu_cols * mcu_rows;

    int nb_blocs_Y = H_Y * V_Y;
    int nb_blocs_Cb = H_Cb * V_Cb;
    int nb_blocs_Cr = H_Cr * V_Cr;

    // Allocation
    *Y  = malloc(total_mcus * sizeof(int **));    
    *Cb = malloc(total_mcus * sizeof(int **));
    *Cr = malloc(total_mcus * sizeof(int **));
    


    int dc_prec_Y = 0;
    int dc_prec_Cb = 0;
    int dc_prec_Cr = 0;

    for (int mcu = 0; mcu < total_mcus; mcu++) {
        (*Y)[mcu] = malloc(nb_blocs_Y * sizeof(int *));
        for (int i = 0; i < nb_blocs_Y; i++) {
            (*Y)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                         table_huffman_dc_Y, symboles_dc_Y, nbr_sym_dc_Y,
                         table_huffman_ac_Y, symboles_ac_Y, nbr_sym_ac_Y,
                         &dc_prec_Y, (*Y)[mcu][i]);
        }
        
        (*Cb)[mcu] = malloc(nb_blocs_Cb * sizeof(int *));
        for (int i = 0; i < nb_blocs_Cb; i++) {
            (*Cb)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                            table_huffman_dc_C, symboles_dc_C, nbr_sym_dc_C,
                            table_huffman_ac_C, symboles_ac_C, nbr_sym_ac_C,
                            &dc_prec_Cb, (*Cb)[mcu][i]);
        }

        (*Cr)[mcu] = malloc(nb_blocs_Cr * sizeof(int *));
        for (int i = 0; i < nb_blocs_Cr; i++) {
            (*Cr)[mcu][i] = malloc(64 * sizeof(int));
            decoder_bloc(data, bit_pos,
                            table_huffman_dc_C, symboles_dc_C, nbr_sym_dc_C,
                            table_huffman_ac_C, symboles_ac_C, nbr_sym_ac_C,
                            &dc_prec_Cr, (*Cr)[mcu][i]);
        }
        
    }

}

