#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "writter.h"
#include "reader.h"
#include "huffman.h"
#include "zigzag.h"
#include "iqzz.h"
#include "idct.h"

int traitement_grayscale(IMAGE* image,IMAGE_D*image_d){
    size_t bit_pos = 0;
    int dc_precedent = 0;
    int bloc[64] = {0};
    int nb_mcus_x = (image->Largeur+7) / 8;
    int nb_mcus_y = (image->Hauteur+7) / 8;
    int nbr_mcus = nb_mcus_x * nb_mcus_y;
    int16_t** bloc_ap = NULL;
    uint8_t** spatial_block = NULL;
    // Générer à nouveau les tables Huffman (au cas où elles changent dynamiquement)
    char** huffman_dc = generer_codes_huffman(
        image->HUFFMAN_tables[0].tailles,
        image->HUFFMAN_tables[0].nb_symbols);
    char** huffman_ac = generer_codes_huffman(
        image->HUFFMAN_tables[2].tailles,
        image->HUFFMAN_tables[2].nb_symbols);
        
    if (!huffman_dc || !huffman_ac) {
        fprintf(stderr, "Erreur de génération des tables Huffman au MCU \n");
        exit(1);
    }

    bloc_ap = malloc(8 * sizeof(int*));
    for (int i = 0; i < 8; i++) {
        bloc_ap[i] = malloc(8 * sizeof(int));
    }
    for (int mcu = 0; mcu < nbr_mcus; mcu++) {
        // Décompression du bloc
        decoder_bloc(
            image->compressed_data, &bit_pos,
            huffman_dc, image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].nb_symbols,
            huffman_ac, image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].nb_symbols,
            &dc_precedent, bloc);

        quantification_inverse(bloc, image->Quant_Table[0]);

        inverse_zigzag(bloc, bloc_ap);
                            
        idct_rapide(bloc_ap, &spatial_block);
        copy_mcu_to_image(image_d, spatial_block, NULL, NULL, mcu, nb_mcus_x, 1, 1);
        for (int i = 0; i < 8; i++) {
            free(spatial_block[i]);
        }
        free(spatial_block);
        spatial_block = NULL;
        
    }
    // frees 
    for (int i = 0; i < 8; i++){
        free(bloc_ap[i]);
    } 
    free(bloc_ap);
    free_huffman_table(huffman_dc, image->HUFFMAN_tables[0].nb_symbols);
    free_huffman_table(huffman_ac, image->HUFFMAN_tables[2].nb_symbols);
    return 0;
}