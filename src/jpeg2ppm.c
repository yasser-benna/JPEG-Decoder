#include "huffman.h"
#include "bitsmanip.h"
#include "writter.h"
#include "reader.h"
#include "iqzz.h"
#include "zigzag.h"
#include "idct.h"
#include "ycbcr_rgb.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    IMAGE* image = read_jpeg(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur de lecture du fichier JPEG.\n");
        return 2;
    }

    IMAGE_D* image_d = malloc(sizeof(IMAGE_D));
    if (!image_d) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return 3;
    }

    size_t bit_pos = 0;
    int dc_precedent = 0;
     int bloc[64] = {0};
    int nb_mcus_x = image->Largeur / 8;
    int nb_mcus_y = image->Hauteur / 8;
    int nbr_mcus = nb_mcus_x * nb_mcus_y;

    for (int mcu = 0; mcu < nbr_mcus; mcu++) {
        
       

        // Générer à nouveau les tables Huffman (au cas où elles changent dynamiquement)
        char** huffman_dc = generer_codes_huffman(
            image->HUFFMAN_tables[0].symbols,
            image->HUFFMAN_tables[0].tailles,
            image->HUFFMAN_tables[0].nb_symbols);

        char** huffman_ac = generer_codes_huffman(
            image->HUFFMAN_tables[2].symbols,
            image->HUFFMAN_tables[2].tailles,
            image->HUFFMAN_tables[2].nb_symbols);

        if (!huffman_dc || !huffman_ac) {
            fprintf(stderr, "Erreur de génération des tables Huffman au MCU %d\n", mcu);
            continue;
        }

        // Décompression du bloc
        decoder_bloc(
            image->compressed_data, &bit_pos,
            huffman_dc, image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].nb_symbols,
            huffman_ac, image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].nb_symbols,
            &dc_precedent, bloc);

        printf("MCU %d - Bloc après décodage Huffman:\n", mcu + 1);
        for (int i = 0; i < 64; i++) {
            printf("%04hx ", (uint16_t)bloc[i]);
            if ((i + 1) % 8 == 0) printf("\n");
        }

        quantification_inverse(bloc, image->Quant_Table[0]);
        printf("Quantification inverse:\n");
        for (int i = 0; i < 64; i++) {
            printf("%04hx ", (uint16_t)bloc[i]);
            if ((i + 1) % 8 == 0) printf("\n");
        }

        int** bloc_ap = malloc(8 * sizeof(int*));
        for (int i = 0; i < 8; i++) {
            bloc_ap[i] = malloc(8 * sizeof(int));
        }
        inverse_zigzag(bloc, bloc_ap);

        printf("Inverse Zigzag:\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%04hx ", (uint16_t)bloc_ap[i][j]);
            }
            printf("\n");
        }

        unsigned char** spatial_block;
        idct_ptr(bloc_ap, &spatial_block);

        printf("IDCT:\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                printf("%04hx ", (uint16_t)spatial_block[i][j]);
            }
            printf("\n");
        }

        // Libérations
        for (int i = 0; i < 8; i++) free(bloc_ap[i]);
        free(bloc_ap);
        free_huffman_table(huffman_dc, image->HUFFMAN_tables[0].nb_symbols);
        free_huffman_table(huffman_ac, image->HUFFMAN_tables[2].nb_symbols);
    }
    if(image->nb_components == 3){
        image_d->colors = 1;
        image_d->R = malloc(image->Largeur * image->Hauteur);
        image_d->G = malloc(image->Largeur * image->Hauteur);
        image_d->B = malloc(image->Largeur * image->Hauteur);
        //ycbcr_to_rgb(image, image_d);
    }else{
        image_d->colors = 0;
        image_d->R = malloc(image->Largeur * image->Hauteur);
        image_d->G = NULL;
        image_d->B = NULL;
        for(int i=0;i<image->Largeur*image->Hauteur;i++){
            image_d->R[i] = (uint8_t)bloc[i];
        }
        write_image("output.pgm", *image_d);
    }
    return 0;
}
