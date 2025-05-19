#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "writter.h"
#include "reader.h"
#include "huffman.h"
#include "bitsmanip.h"
#include "extraction_bloc.h"
#include "iqzz.h"
#include "zigzag.h"
#include "idct.h"
#include "upsampling.h"
#include "ycbcr_rgb.h"
#include "output_file.h"
#include "jpeg2ppm.h"
void print_bloc(uint8_t **bloc, int mcu_index, char * component, int H_Y, int V_Y) {
    printf("\n--- MCU #%d -(%s) ---\n", mcu_index, component);
    for (int i = 0; i < 8*V_Y; i++) {
        for (int j = 0; j < 8*H_Y; j++) {
            printf("%4hx ", bloc[i][j]);
        }
        printf("\n");
    }
}


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
    // Libérations
    for (int i = 0; i < 8; i++){
        free(bloc_ap[i]);
    } 
    free(bloc_ap);
    free_huffman_table(huffman_dc, image->HUFFMAN_tables[0].nb_symbols);
    free_huffman_table(huffman_ac, image->HUFFMAN_tables[2].nb_symbols);
    // Libération de la mémoire
    if (spatial_block != NULL) {
        for (int i = 0; i < 8; i++) {
            if (spatial_block[i] != NULL) {
                free(spatial_block[i]);
            }
        }
        free(spatial_block);
    }
    return 0;
}
int traitement_color(IMAGE* image, IMAGE_D* image_d){
    int H_Y = image->COMPONENTS[0].h_factor;  
    int V_Y = image->COMPONENTS[0].v_factor;
    int H_Cb = image->COMPONENTS[1].h_factor;  
    int V_Cb = image->COMPONENTS[1].v_factor;
    int H_Cr = image->COMPONENTS[2].h_factor;  
    int V_Cr = image->COMPONENTS[2].v_factor;
    SamplingFormat forbloc;
    if (H_Y == H_Cr && H_Cb == H_Cr && V_Y == V_Cr && V_Cb == V_Cr) {forbloc=SAMPLING_444;}
    else if (H_Y == 2 * H_Cr && V_Y == V_Cb && H_Cb == H_Cr && V_Y == V_Cr && V_Cb == V_Cr) {forbloc=SAMPLING_422_H;}
    else if (H_Y == H_Cr && V_Y == 2 * V_Cb && H_Cb == H_Cr && V_Y == 2 * V_Cr && V_Cb == V_Cr) {forbloc=SAMPLING_422_V;}
    else if (H_Y == 2 * H_Cr && H_Cb == H_Cr && V_Y == 2 * V_Cr && V_Cb == V_Cr) {forbloc=SAMPLING_420;}
    else {
        forbloc=SAMPLING_UNSUPPORTED;
        fprintf(stderr, "Forbloc de sous-échantillonnage non supporté.\n");
        exit(1);
    }
    char** huffman_dc_Y = generer_codes_huffman( image->HUFFMAN_tables[0].tailles, image->HUFFMAN_tables[0].nb_symbols);
    char** huffman_ac_Y = generer_codes_huffman( image->HUFFMAN_tables[2].tailles, image->HUFFMAN_tables[2].nb_symbols);
    char** huffman_dc_C = generer_codes_huffman( image->HUFFMAN_tables[1].tailles, image->HUFFMAN_tables[1].nb_symbols);
    char** huffman_ac_C = generer_codes_huffman( image->HUFFMAN_tables[3].tailles, image->HUFFMAN_tables[3].nb_symbols);
    if (!huffman_dc_Y || !huffman_ac_Y || !huffman_dc_C || !huffman_ac_C) {
        fprintf(stderr, "Error generating Huffman tables\n");
        exit(1);
    }

    int nb_mcus_x = (image->Largeur + 8 * H_Y - 1) / (8 * H_Y);
    int nb_mcus_y = (image->Hauteur + 8 * V_Y - 1) / (8 * V_Y);
    int total_mcus = nb_mcus_x * nb_mcus_y;
    int nb_blocs_Y = H_Y * V_Y;
    int nb_blocs_Cr = H_Cr * V_Cr;
    int nb_blocs_Cb = H_Cb * V_Cb;
    // Allocation correcte pour les blocs à 4 niveaux de pointeurs
    int *** Y_blocs = NULL, ***Cb_blocs = NULL, ***Cr_blocs = NULL;
    size_t bit_pos = 0;
    // Extraction des blocs pour Y, Cb et Cr
    extraire_bloc(
            image->compressed_data, &bit_pos,
            huffman_dc_Y, image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].nb_symbols,
            huffman_ac_Y, image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].nb_symbols,
            huffman_dc_C, image->HUFFMAN_tables[1].symbols, image->HUFFMAN_tables[1].nb_symbols,
            huffman_ac_C, image->HUFFMAN_tables[3].symbols, image->HUFFMAN_tables[3].nb_symbols,
            H_Y, H_Cb,H_Cr, V_Y, V_Cb,V_Cr,
            image->Largeur, image->Hauteur,
            &Y_blocs, &Cb_blocs, &Cr_blocs
        );
    if (!Y_blocs || !Cb_blocs || !Cr_blocs) {
        fprintf(stderr, "Block extraction failed\n");
        exit(1);
    }
    //Décodage du composant Y
    uint8_t ****Y_final = malloc(total_mcus * sizeof(uint8_t***));
    uint8_t ****Cb_final = malloc(total_mcus * sizeof(uint8_t***));
    uint8_t ****Cr_final = malloc(total_mcus * sizeof(uint8_t***));
    if (!Y_final || !Cb_final || !Cr_final) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // Allocate working blocks
    int16_t **Y_bloc = malloc(8 * sizeof(int16_t*));
    int16_t **cb_bloc = malloc(8 * sizeof(int16_t*));
    int16_t **cr_bloc = malloc(8 * sizeof(int16_t*));
    
    for (int i = 0; i < 8; i++) {
        Y_bloc[i] = malloc(8 * sizeof(int16_t));
        cb_bloc[i] = malloc(8 * sizeof(int16_t));
        cr_bloc[i] = malloc(8 * sizeof(int16_t));
        if (!Y_bloc[i] || !cb_bloc[i] || !cr_bloc[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    // Process each MCU
    uint8_t **cb_spatial, **cr_spatial;
    uint8_t **R, **G, **B;
    int free_cb_spatial = 1;
    int free_cr_spatial = 1;

    for (int mcu = 0; mcu < total_mcus; mcu++) {
        Y_final[mcu] = malloc(nb_blocs_Y * sizeof(uint8_t**));
        for (int b = 0; b < nb_blocs_Y; b++) {  
            quantification_inverse(Y_blocs[mcu][b], image->Quant_Table[0]);
            inverse_zigzag(Y_blocs[mcu][b], Y_bloc);
            idct_rapide(Y_bloc, &Y_final[mcu][b]);
        }
        Cb_final[mcu] = malloc(nb_blocs_Cb * sizeof(uint8_t**));
        Cr_final[mcu] = malloc(nb_blocs_Cr * sizeof(uint8_t**));
        if (!Cb_final[mcu] || !Cr_final[mcu]){
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        for (int b = 0; b < nb_blocs_Cb; b++) {
            quantification_inverse(Cb_blocs[mcu][b], image->Quant_Table[1]);
            inverse_zigzag(Cb_blocs[mcu][b], cb_bloc);
            idct_rapide(cb_bloc,&cb_spatial);
            free_cb_spatial = 1;
            switch(forbloc){
                case SAMPLING_444:
                    Cb_final[mcu][b] = cb_spatial;
                    free_cb_spatial = 0;
                    break;
                case SAMPLING_422_H:
                    up_sampling4_2_2_horizontal(cb_spatial,&Cb_final[mcu][b], H_Y, V_Y);
                    break;
                case SAMPLING_422_V:
                    up_sampling4_2_2_vertical(cb_spatial,&Cb_final[mcu][b], H_Y, V_Y);
                    break;
                case SAMPLING_420:
                    up_sampling4_2_0(cb_spatial, &Cb_final[mcu][b], H_Y, V_Y);
                    break;
                default:
                    fprintf(stderr, "Unsupported sampling format for Cb\n");
                    exit(1);
            }
            if (free_cb_spatial) {
                for (int i = 0; i < 8; i++) {
                    free(cb_spatial[i]);
                }
                free(cb_spatial);
            }

        }
        for (int b = 0; b < nb_blocs_Cr; b++) {
            quantification_inverse(Cr_blocs[mcu][b], image->Quant_Table[1]);
            inverse_zigzag(Cr_blocs[mcu][b], cr_bloc);
            idct_rapide(cr_bloc, &cr_spatial);
            free_cr_spatial = 1;
            switch(forbloc){
                case SAMPLING_444:
                    Cr_final[mcu][b] = cr_spatial;
                    free_cr_spatial = 0;
                    break;
                case SAMPLING_422_H:
                    up_sampling4_2_2_horizontal(cr_spatial,&Cr_final[mcu][b], H_Y, V_Y);
                    break;
                case SAMPLING_422_V:
                    up_sampling4_2_2_vertical(cr_spatial, &Cr_final[mcu][b], H_Y, V_Y);
                    break;
                case SAMPLING_420:
                    up_sampling4_2_0(cr_spatial, &Cr_final[mcu][b], H_Y, V_Y);
                    break;
                default:
                    fprintf(stderr, "Unsupported sampling format for Cr\n");
                    exit(1);
            }
            if (free_cr_spatial) {
                for (int i = 0; i < 8; i++) {
                    free(cr_spatial[i]);
                }
                free(cr_spatial);
            }

        }
        YCbCr_to_rgb(Y_final[mcu], Cb_final[mcu], Cr_final[mcu], &R, &G, &B, H_Y, V_Y, nb_blocs_Y, nb_blocs_Cb, forbloc);
        copy_mcu_to_image(image_d, R, G, B, mcu, nb_mcus_x, H_Y, V_Y);
        // Free RGB arrays
        for (int i = 0; i < 8 * V_Y; i++) {
            free(R[i]);
            free(G[i]);
            free(B[i]);
        }
        free(R);
        free(G);
        free(B);
        //free Yfinal, Cb_final, Cr_final
        for (int b = 0; b < nb_blocs_Y; b++) {
            for (int i = 0; i <8; i++) {
                free(Y_final[mcu][b][i]);
            }
            free(Y_final[mcu][b]);
        }
        free(Y_final[mcu]);
        for (int b = 0; b < nb_blocs_Cb; b++) {
            for(int i=0;i< V_Y*8;i++){
                free(Cb_final[mcu][b][i]);
            }
            free(Cb_final[mcu][b]);
        }
        free(Cb_final[mcu]);
        for (int b = 0; b < nb_blocs_Cr; b++) {
            for(int i=0;i< V_Y*8;i++){
                free(Cr_final[mcu][b][i]);
            }
            free(Cr_final[mcu][b]);
        }
        free(Cr_final[mcu]);
    }
    for (int i = 0; i < 8; i++) {
        free(Y_bloc[i]);
        free(cb_bloc[i]);
        free(cr_bloc[i]);
    }
    

    free(Y_bloc);
    free(cb_bloc);
    free(cr_bloc);
    free_huffman_table(huffman_dc_Y, image->HUFFMAN_tables[0].nb_symbols);
    free_huffman_table(huffman_ac_Y, image->HUFFMAN_tables[2].nb_symbols);
    free_huffman_table(huffman_dc_C, image->HUFFMAN_tables[1].nb_symbols);
    free_huffman_table(huffman_ac_C, image->HUFFMAN_tables[3].nb_symbols);
    // free Y_final, Cb_final, Cr_final
    for (int mcu = 0; mcu < total_mcus; mcu++) {
        // Free each Y block
        for (int i = 0; i < nb_blocs_Y; i++) {
            free(Y_blocs[mcu][i]);
        }
        free(Y_blocs[mcu]);
        
        // Free each Cb block
        for (int i = 0; i < nb_blocs_Cb; i++) {
            free(Cb_blocs[mcu][i]);
        }
        free(Cb_blocs[mcu]);
        
        // Free each Cr block
        for (int i = 0; i < nb_blocs_Cr; i++) {
            free(Cr_blocs[mcu][i]);
        }
        free(Cr_blocs[mcu]);
    }
    free(Y_blocs);
    free(Cb_blocs);
    free(Cr_blocs);
    free(Y_final);
    free(Cb_final);
    free(Cr_final);
    return 0;

}
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
    char* output_filename = create_output_filename(argv[1]);
    IMAGE_D* image_d = init_image_d(image->Largeur, image->Hauteur, image->nb_components==3);
    if (image->nb_components == 1){
        traitement_grayscale(image, image_d);
    }
    else{
        traitement_color(image, image_d);
    }
    FILE* file = fopen(output_filename, "w");
    if(!file) {
        fprintf(stderr, "Erreur d'ouverture du fichier de sortie.\n");
        exit(1);
    }
    write_image_d(file, *image_d);
    fclose(file);
    free(output_filename);
    free_image(image);
    free_image_d(image_d);
    return 0;
}
                            