// #include "huffman.h"
// #include "bitsmanip.h"
// #include "writter.h"
// #include "reader.h"
// #include "iqzz.h"
// #include "zigzag.h"
// #include "idct.h"
// #include "ycbcr_rgb.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>

// int main(int argc,char* argv[]){
//     if(argc !=2){
//         printf("Usage: %s <input_file>\n", argv[0]);
//         return 1;
//     }
//     IMAGE* image = read_jpeg(argv[1]);
//     IMAGE_D* image_d = malloc(sizeof(IMAGE_D));
//     char** huffman_dc=generer_codes_huffman(image->HUFFMAN_tables[0].symbols,image->HUFFMAN_tables[0].tailles,image->HUFFMAN_tables[0].nb_symbols);
//     char** huffman_ac=generer_codes_huffman(image->HUFFMAN_tables[2].symbols,image->HUFFMAN_tables[2].tailles,image->HUFFMAN_tables[2].nb_symbols);
//     int bloc[64]={0};
//     int dc_precedent=0;
//     size_t bit_pos = 0;
//     decoder_bloc(
//         image->compressed_data,&bit_pos,
//         huffman_dc,image->HUFFMAN_tables[0].symbols,image->HUFFMAN_tables[0].nb_symbols,
//         huffman_ac,image->HUFFMAN_tables[2].symbols,image->HUFFMAN_tables[2].nb_symbols,
//         &dc_precedent,bloc);
//     for (int i = 0; i < 64; i++) {
//         printf("%04hx ", (uint16_t)bloc[i]);  // affichage hex sur 4 digits
//     if ((i + 1) % 8 == 0) printf("\n");
//     }
//     quantification_inverse(bloc,image->Quant_Table[0]);
//     printf("Quantification inverse:\n");
//     for (int i = 0; i < 64; i++) {
//         printf("%04hx ", (uint16_t)bloc[i]);  // affichage hex sur 4 digits
//         if ((i + 1) % 8 == 0) printf("\n");
//     }
//     int** bloc_ap=malloc(8*sizeof(int*));
//     for(int i=0;i<8;i++){
//         bloc_ap[i]=malloc(8*sizeof(int));
//     }
//     inverse_zigzag(bloc,bloc_ap);
//     printf("Inverse zigzag:\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%04hx ", (uint16_t)bloc_ap[i][j]);  // affichage hex sur 4 digits
//         }
//         printf("\n");
//     }
//     unsigned char** spatial_block;
//     idct_ptr(bloc_ap,&spatial_block);
//     printf("IDCT:\n");
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             printf("%3d ", spatial_block[i][j]);  // affichage hex sur 4 digits
//         }
//         printf("\n");
//     }
//     return 0;
// }

// #include "huffman.h"
// #include "bitsmanip.h"
// #include "writter.h"
// #include "reader.h"
// #include "iqzz.h"
// #include "zigzag.h"
// #include "idct.h"
// #include "ycbcr_rgb.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>

// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         printf("Usage: %s <input_file>\n", argv[0]);
//         return 1;
//     }

//     IMAGE* image = read_jpeg(argv[1]);
//     if (!image) {
//         fprintf(stderr, "Erreur de lecture de l'image\n");
//         return 1;
//     }

//     size_t bit_pos = 0;
//     int nb_components = image->nb_components_sos;

//     // Générer les tables Huffman pour chaque composant SOS
//     char** huffman_dc[3] = { NULL };
//     char** huffman_ac[3] = { NULL };

//     for (int i = 0; i < nb_components; i++) {
//         int id_dc = image->HUFFMAN_tables[image->COMPONENTS_SOS[i].dc_table_id].id;
//         int id_ac = image->HUFFMAN_tables[image->COMPONENTS_SOS[i].ac_table_id].id;

//         huffman_dc[i] = generer_codes_huffman(
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].dc_table_id].symbols,
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].dc_table_id].tailles,
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].dc_table_id].nb_symbols
//         );

//         huffman_ac[i] = generer_codes_huffman(
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].ac_table_id].symbols,
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].ac_table_id].tailles,
//             image->HUFFMAN_tables[image->COMPONENTS_SOS[i].ac_table_id].nb_symbols
//         );
//     }

//     // Calcul du nombre de blocs dans l'image (chaque MCU = 1 bloc par composant)
//     int nb_mcus_x = image->Largeur / 8;
//     int nb_mcus_y = image->Hauteur / 8;
//     int total_mcus = nb_mcus_x * nb_mcus_y;

//     int dc_prec[3] = { 0 }; // Une valeur DC précédente par composant

//     for (int mcu = 0; mcu < total_mcus; mcu++) {
//         printf("=== MCU %d ===\n", mcu);
//         for (int c = 0; c < nb_components; c++) {
//             int bloc[64] = { 0 };

//             decoder_bloc(
//                 image->compressed_data, &bit_pos,
//                 huffman_dc[c], image->HUFFMAN_tables[image->COMPONENTS_SOS[c].dc_table_id].symbols,
//                 image->HUFFMAN_tables[image->COMPONENTS_SOS[c].dc_table_id].nb_symbols,
//                 huffman_ac[c], image->HUFFMAN_tables[image->COMPONENTS_SOS[c].ac_table_id].symbols,
//                 image->HUFFMAN_tables[image->COMPONENTS_SOS[c].ac_table_id].nb_symbols,
//                 &dc_prec[c], bloc
//             );

//             uint8_t qt_id = image->COMPONENTS[c].qt_id;
//             quantification_inverse(bloc, image->Quant_Table[qt_id]);

//             int** bloc_ap = malloc(8 * sizeof(int*));
//             for (int i = 0; i < 8; i++) {
//                 bloc_ap[i] = malloc(8 * sizeof(int));
//             }
//             inverse_zigzag(bloc, bloc_ap);

//             unsigned char** spatial_block;
//             idct_ptr(bloc_ap, &spatial_block);

//             // Affichage debug
//             printf("--- composant %d ---\n", c);
//             for (int i = 0; i < 8; i++) {
//                 for (int j = 0; j < 8; j++) {
//                     printf("%3d ", spatial_block[i][j]);
//                 }
//                 printf("\n");
//             }

//         }
//     }
//     return 0;
// }

// #include "huffman.h"
// #include "bitsmanip.h"
// #include "writter.h"
// #include "reader.h"
// #include "iqzz.h"
// #include "zigzag.h"
// #include "idct.h"
// #include "ycbcr_rgb.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>

// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         printf("Usage: %s <input_file>\n", argv[0]);
//         return 1;
//     }

//     IMAGE* image = read_jpeg(argv[1]);
//     if (!image) {
//         fprintf(stderr, "Erreur de lecture du fichier JPEG.\n");
//         return 2;
//     }

//     IMAGE_D* image_d = malloc(sizeof(IMAGE_D));
//     if (!image_d) {
//         fprintf(stderr, "Erreur d'allocation mémoire.\n");
//         return 3;
//     }

//     size_t bit_pos = 0;
//     int dc_precedent = 0;
//      int bloc[64] = {0};
//     int nb_mcus_x = image->Largeur / 8;
//     int nb_mcus_y = image->Hauteur / 8;
//     int nbr_mcus = nb_mcus_x * nb_mcus_y;

//     for (int mcu = 0; mcu < nbr_mcus; mcu++) {

//         // Générer à nouveau les tables Huffman (au cas où elles changent dynamiquement)
//         char** huffman_dc = generer_codes_huffman(
//             image->HUFFMAN_tables[0].symbols,
//             image->HUFFMAN_tables[0].tailles,
//             image->HUFFMAN_tables[0].nb_symbols);

//         char** huffman_ac = generer_codes_huffman(
//             image->HUFFMAN_tables[2].symbols,
//             image->HUFFMAN_tables[2].tailles,
//             image->HUFFMAN_tables[2].nb_symbols);

//         if (!huffman_dc || !huffman_ac) {
//             fprintf(stderr, "Erreur de génération des tables Huffman au MCU %d\n", mcu);
//             continue;
//         }

//         // Décompression du bloc
//         decoder_bloc(
//             image->compressed_data, &bit_pos,
//             huffman_dc, image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].nb_symbols,
//             huffman_ac, image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].nb_symbols,
//             &dc_precedent, bloc);

//         printf("MCU %d - Bloc après décodage Huffman:\n", mcu + 1);
//         for (int i = 0; i < 64; i++) {
//             printf("%04hx ", (uint16_t)bloc[i]);
//             if ((i + 1) % 8 == 0) printf("\n");
//         }

//         quantification_inverse(bloc, image->Quant_Table[0]);
//         printf("Quantification inverse:\n");
//         for (int i = 0; i < 64; i++) {
//             printf("%04hx ", (uint16_t)bloc[i]);
//             if ((i + 1) % 8 == 0) printf("\n");
//         }

//         int** bloc_ap = malloc(8 * sizeof(int*));
//         for (int i = 0; i < 8; i++) {
//             bloc_ap[i] = malloc(8 * sizeof(int));
//         }
//         inverse_zigzag(bloc, bloc_ap);

//         printf("Inverse Zigzag:\n");
//         for (int i = 0; i < 8; i++) {
//             for (int j = 0; j < 8; j++) {
//                 printf("%04hx ", (uint16_t)bloc_ap[i][j]);
//             }
//             printf("\n");
//         }

//         unsigned char** spatial_block;
//         idct_ptr(bloc_ap, &spatial_block);

//         printf("IDCT:\n");
//         for (int i = 0; i < 8; i++) {
//             for (int j = 0; j < 8; j++) {
//                 printf("%04hx ", (uint16_t)spatial_block[i][j]);
//             }
//             printf("\n");
//         }

//         // Libérations
//         for (int i = 0; i < 8; i++) free(bloc_ap[i]);
//         free(bloc_ap);
//         free_huffman_table(huffman_dc, image->HUFFMAN_tables[0].nb_symbols);
//         free_huffman_table(huffman_ac, image->HUFFMAN_tables[2].nb_symbols);
//     }

//     free(image_d);
//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include "writter.h"
// #include "reader.h"
// #include "huffman.h"
// #include "bitsmanip.h"
// #include "extraction_bloc.h"
// #include "iqzz.h"
// #include "zigzag.h"
// #include "idct.h"
// #include "upsampling.h"
// #include "ycbcr_rgb.h"


// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         printf("Usage: %s <input_file>\n", argv[0]);
//         return 1;
//     }

//     IMAGE* image = read_jpeg(argv[1]);
//     if (!image) {
//         fprintf(stderr, "Erreur de lecture du fichier JPEG.\n");
//         return 2;
//     }
//     // Vérifier les facteurs de sous-échantillonnage pour les composants Cb et Cr
//     int H_Y = image->COMPONENTS[0].h_factor;  
//     int V_Y = image->COMPONENTS[0].v_factor;
//     int H_C = image->COMPONENTS[1].h_factor;  
//     int V_C = image->COMPONENTS[1].v_factor;

//     int forbloc;
//     if (H_Y == H_C && V_Y == V_C) forbloc = 0;       // 4:4:4
//     else if (H_Y == 2 * H_C && V_Y == V_C) forbloc = 1;  // 4:2:2
//     else if (H_Y == 2 * H_C && V_Y == 2 * V_C) forbloc = 2; // 4:2:0
//     else {
//         fprintf(stderr, "Forbloc de sous-échantillonnage non supporté.\n");
//         return 3;
//     }

//     // Dimensions en blocs
//     int nb_mcus_x = (image->Largeur  ) / (8 * H_Y);
//     int nb_mcus_y = (image->Hauteur ) / (8 * V_Y);
//     int total_mcus = nb_mcus_x * nb_mcus_y;
//     int nb_blocs_Y = H_Y * V_Y;
//     int nb_blocs_C = H_C * V_C;

//     int ****Y_blocs, ****Cb_blocs = NULL, ****Cr_blocs = NULL;
//     size_t bit_pos = 0;

//     char** huffman_dc_Y = generer_codes_huffman(
//         image->HUFFMAN_tables[0].symbols,
//         image->HUFFMAN_tables[0].tailles,
//         image->HUFFMAN_tables[0].nb_symbols);

//     char** huffman_ac_Y = generer_codes_huffman(
//         image->HUFFMAN_tables[2].symbols,
//         image->HUFFMAN_tables[2].tailles,
//         image->HUFFMAN_tables[2].nb_symbols);
//     char** huffman_dc_C = generer_codes_huffman(
//         image->HUFFMAN_tables[1].symbols,
//         image->HUFFMAN_tables[1].tailles,
//         image->HUFFMAN_tables[1].nb_symbols);

//     char** huffman_ac_C = generer_codes_huffman(
//         image->HUFFMAN_tables[3].symbols,
//         image->HUFFMAN_tables[3].tailles,
//         image->HUFFMAN_tables[3].nb_symbols);

//     // Extraction blocs DCT
//     extraire_bloc(
//         image->compressed_data, &bit_pos,
//         huffman_dc_Y, image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].nb_symbols,
//         huffman_ac_Y, image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].nb_symbols,
//         huffman_dc_C, image->HUFFMAN_tables[1].symbols, image->HUFFMAN_tables[1].nb_symbols,
//         huffman_ac_C, image->HUFFMAN_tables[3].symbols, image->HUFFMAN_tables[3].nb_symbols,
//         H_Y, H_C, V_Y, V_C,
//         image->Largeur, image->Hauteur,
//         &Y_blocs, &Cb_blocs, &Cr_blocs
//     );

//     unsigned char ****Y_final = malloc(total_mcus * sizeof(unsigned char***));
//     for (int mcu = 0; mcu < total_mcus; mcu++) {
//         Y_final[mcu] = malloc(nb_blocs_Y * sizeof(unsigned char**));
//         for (int b = 0; b < nb_blocs_Y; b++) {
//             int **Y_bloc = malloc(8 * sizeof(int*));
//             for (int i = 0; i < 8; i++) Y_bloc[i] = malloc(8 * sizeof(int));

//             quantification_inverse(Y_blocs[mcu][b], image->Quant_Table[0]);
//             inverse_zigzag(Y_blocs[mcu][b], Y_bloc);
//             idct_ptr(Y_bloc, &Y_final[mcu][b]);

//             for (int i = 0; i < 8; i++) free(Y_bloc[i]);
//             free(Y_bloc);
//         }
//     }

    
//     unsigned char ****Cb_final = malloc(total_mcus * sizeof(unsigned char***));
//     unsigned char ****Cr_final = malloc(total_mcus * sizeof(unsigned char***));

//     for (int mcu = 0; mcu < total_mcus; mcu++) {
//         Cb_final[mcu] = malloc(nb_blocs_C * sizeof(unsigned char**));
//         Cr_final[mcu] = malloc(nb_blocs_C * sizeof(unsigned char**));

//         for (int b = 0; b < nb_blocs_C; b++) {
//             int **cb_bloc = malloc(8 * sizeof(int*));
//             int **cr_bloc = malloc(8 * sizeof(int*));
//             for (int i = 0; i < 8; i++) {
//                 cb_bloc[i] = malloc(8 * sizeof(int));
//                 cr_bloc[i] = malloc(8 * sizeof(int));
//             }

//             quantification_inverse(Cb_blocs[mcu][b], image->Quant_Table[1]);
//             quantification_inverse(Cr_blocs[mcu][b], image->Quant_Table[2]);
//             inverse_zigzag(Cb_blocs[mcu][b], cb_bloc);
//             inverse_zigzag(Cr_blocs[mcu][b], cr_bloc);

//             unsigned char **cb_spatial, **cr_spatial;
//             idct_ptr(cb_bloc, &cb_spatial);
//             idct_ptr(cr_bloc, &cr_spatial);

//             if (forbloc == 0) {
//                 Cb_final[mcu][b] = cb_spatial;
//                 Cr_final[mcu][b] = cr_spatial;
//             } else if (forbloc == 1) {
//                 up_sampling4_2_2(cb_spatial, cr_spatial, &Cb_final[mcu][b], &Cr_final[mcu][b]);
//             } else if (forbloc == 2) {
//                 up_sampling4_2_0(cb_spatial, cr_spatial, &Cb_final[mcu][b], &Cr_final[mcu][b]);
//             }

//             for (int i = 0; i < 8; i++) {
//                 free(cb_bloc[i]);
//                 free(cr_bloc[i]);
//             }
//             free(cb_bloc);
//             free(cr_bloc);
//         }
//     }

//     uint8_t **Y_matrix = malloc(image->Hauteur * sizeof(uint8_t *));
//     for (int i = 0; i < image->Hauteur; i++) {
//         Y_matrix[i] = malloc(image->Largeur * sizeof(uint8_t));
//     }

//     uint8_t **Cb_matrix = NULL;
//     uint8_t **Cr_matrix = NULL;
//     uint8_t **R_matrix = NULL;
//     uint8_t **G_matrix = NULL;
//     uint8_t **B_matrix = NULL;

//     if (image->nb_components ==3) {
//         Cb_matrix = malloc(image->Hauteur * sizeof(uint8_t *));
//         Cr_matrix = malloc(image->Hauteur * sizeof(uint8_t *));
//         R_matrix  = malloc(image->Hauteur * sizeof(uint8_t *));
//         G_matrix  = malloc(image->Hauteur * sizeof(uint8_t *));
//         B_matrix  = malloc(image->Hauteur * sizeof(uint8_t *));

//         for (int i = 0; i < image->Hauteur; i++) {
//             Cb_matrix[i] = malloc(image->Largeur * sizeof(uint8_t));
//             Cr_matrix[i] = malloc(image->Largeur * sizeof(uint8_t));
//             R_matrix[i]  = malloc(image->Largeur * sizeof(uint8_t));
//             G_matrix[i]  = malloc(image->Largeur * sizeof(uint8_t));
//             B_matrix[i]  = malloc(image->Largeur * sizeof(uint8_t));
//         }    
//     }

//     remplir_YCbCr_matrices(Y_matrix, Cb_matrix, Cr_matrix, 
//                        Y_final, Cb_final, Cr_final, 
//                        image, nb_mcus_x, nb_mcus_y, 
//                        H_Y, V_Y, H_C, V_C);

//     IMAGE_D img_out;
//     img_out.width = image->Largeur;
//     img_out.height = image->Hauteur;
//     img_out.colors =  (image->nb_components ==3)? 1 : 0;

//     int total_pixels = img_out.width * img_out.height;
//     img_out.R = malloc(total_pixels * sizeof(uint8_t));
//     if (image->nb_components ==3) {
//         img_out.G = malloc(total_pixels * sizeof(uint8_t));
//         img_out.B = malloc(total_pixels * sizeof(uint8_t));
//     } else {
//         img_out.G = NULL;
//         img_out.B = NULL;
//     }

//     if (image->nb_components ==3) {
//         YCbCr_to_rgb(Y_matrix, Cb_matrix, Cr_matrix, R_matrix, G_matrix, B_matrix, image->Hauteur, image->Largeur);
//         for (int i = 0; i < image->Hauteur; i++) {
//             for (int j = 0; j < image->Largeur; j++) {
//                 int index = i * image->Largeur + j;
//                 img_out.R[index] = R_matrix[i][j];
//                 img_out.G[index] = G_matrix[i][j];
//                 img_out.B[index] = B_matrix[i][j];
//             }
//         }
//     } else {
//         for (int i = 0; i < image->Hauteur; i++) {
//             for (int j = 0; j < image->Largeur; j++) {
//                 int index = i * image->Largeur + j;
//                 img_out.R[index] = Y_matrix[i][j];
//             }
//         }
//     }

    
//     write_image("output.pnm", img_out);
//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

void print_bloc(unsigned char **bloc, int mcu_index, int bloc_index, const char *component,int H_Y,int V_Y) {
    printf("\n--- MCU #%d - Bloc #%d (%s) ---\n", mcu_index, bloc_index, component);
    for (int i = 0; i < 8*V_Y; i++) {
        for (int j = 0; j < 8*H_Y; j++) {
            printf("%4hx ", bloc[i][j]);
        }
        printf("\n");
    }
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

    size_t bit_pos = 0;
    int dc_precedent = 0;
    int bloc[64] = {0};
    int nb_mcus_x = image->Largeur / 8;
    int nb_mcus_y = image->Hauteur / 8;
    int nbr_mcus = nb_mcus_x * nb_mcus_y;
    int16_t** bloc_ap = NULL;
    unsigned char** spatial_block = NULL;
    if (image->nb_components == 1){
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
                fprintf(stderr, "Erreur de génération des tables Huffman au MCU \n");
                //continue;
            }
            for (int mcu = 0; mcu < nbr_mcus; mcu++) {
                
                
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
                    
                    bloc_ap = malloc(8 * sizeof(int*));
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
                    
                    
                    idct_fast_asf_boi(bloc_ap, &spatial_block);
                    //idct_ptr(bloc_ap, &spatial_block);
                    printf("IDCT:\n");
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            printf("%04hx ", (unsigned char)spatial_block[i][j]);
                        }
                        printf("\n");
                    }
                    
                    // Libérations
                    for (int i = 0; i < 8; i++) free(bloc_ap[i]);
                    free(bloc_ap);
                }
                free_huffman_table(huffman_dc, image->HUFFMAN_tables[0].nb_symbols);
                free_huffman_table(huffman_ac, image->HUFFMAN_tables[2].nb_symbols);
                for(int i=0;i<8;i++){
                        free(spatial_block[i]);
                }
            }
    else{
                int H_Y = image->COMPONENTS[0].h_factor;  
                int V_Y = image->COMPONENTS[0].v_factor;
                int H_Cb = image->COMPONENTS[1].h_factor;  
                int V_Cb = image->COMPONENTS[1].v_factor;
                int H_Cr = image->COMPONENTS[2].h_factor;  
                int V_Cr = image->COMPONENTS[2].v_factor;
                int forbloc;
                // Génération des tables de Huffman pour DC et AC des composants Y et Cb, Cr
                char** huffman_dc_Y = generer_codes_huffman(image->HUFFMAN_tables[0].symbols, image->HUFFMAN_tables[0].tailles, image->HUFFMAN_tables[0].nb_symbols);
                char** huffman_ac_Y = generer_codes_huffman(image->HUFFMAN_tables[2].symbols, image->HUFFMAN_tables[2].tailles, image->HUFFMAN_tables[2].nb_symbols);
                char** huffman_dc_C = generer_codes_huffman(image->HUFFMAN_tables[1].symbols, image->HUFFMAN_tables[1].tailles, image->HUFFMAN_tables[1].nb_symbols);
                char** huffman_ac_C = generer_codes_huffman(image->HUFFMAN_tables[3].symbols, image->HUFFMAN_tables[3].tailles, image->HUFFMAN_tables[3].nb_symbols);
                if (H_Y == H_Cr && H_Cb==H_Cr && V_Y == V_Cr && V_Cb==V_Cr) forbloc = 0;       // 4:4:4 
                else if (H_Y == 2 * H_Cr && V_Y == V_Cb && H_Cb==H_Cr && V_Y == V_Cr && V_Cb==V_Cr) forbloc = 1;
                else if (H_Y == H_Cr && V_Y == 2 * V_Cb && H_Cb==H_Cr && V_Y == 2 * V_Cr && V_Cb==V_Cr) forbloc = 2;  // 4:2:2 ila kant V_Y==1 rah horizontale o ila kan l3kss verticale 
                else if (H_Y == 2 * H_Cr &&H_Cb==H_Cr && V_Y == 2 * V_Cr && V_Cb==V_Cr) forbloc = 3; // 4:2:0
                else {
                    fprintf(stderr, "Forbloc de sous-échantillonnage non supporté.\n");
                    return 3;
                }
        
            int nb_mcus_x = (image->Largeur+(image->Largeur)%(8*H_Y)) / (8 * H_Y);
            int nb_mcus_y = (image->Hauteur+(image->Hauteur%(8*V_Y))) / (8 * V_Y);
            int total_mcus = nb_mcus_x * nb_mcus_y;
            int nb_blocs_Y = H_Y * V_Y;
            int nb_blocs_Cr = H_Cr * V_Cr;
            int nb_blocs_Cb = H_Cb * V_Cb;
        
            // Allocation correcte pour les blocs à 4 niveaux de pointeurs
            int ***Y_blocs = NULL, ***Cb_blocs = NULL, ***Cr_blocs = NULL;
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
            
            
                //Décodage du composant Y
                unsigned char ****Y_final = malloc(total_mcus * sizeof(unsigned char***));
                for (int mcu = 0; mcu < total_mcus; mcu++) {
                        Y_final[mcu] = malloc(nb_blocs_Y * sizeof(unsigned char**));
                        for (int b = 0; b < nb_blocs_Y; b++) {
                                int16_t **Y_bloc = malloc(8 * sizeof(int*));
                                for (int i = 0; i < 8; i++){
                                        Y_bloc[i] = malloc(8 * sizeof(int));
                                    } 
                                    if (mcu==1080 && b==0 ){
                                        printf("coila\n");
                                    }
                                    int* holder0=Y_blocs[mcu];
                                    int holder1=Y_blocs[mcu][b];
                                    uint8_t holder2= image->Quant_Table[0];
                                    quantification_inverse(Y_blocs[mcu][b], image->Quant_Table[0]);
                                    inverse_zigzag(Y_blocs[mcu][b], Y_bloc);
                                    idct_ptr(Y_bloc, &Y_final[mcu][b]);
                                    print_bloc(Y_final[mcu][b], mcu, b, "Y",1,1);
                        
                                    for (int i = 0; i < 8; i++) {
                                            free(Y_bloc[i]);
                                        }
                                        free(Y_bloc);
                                    }
                            
                                }
                // Initialisation correcte de Cb_final et Cr_final
                unsigned char ****Cb_final = malloc(total_mcus * sizeof(unsigned char***));
                unsigned char ****Cr_final = malloc(total_mcus * sizeof(unsigned char***));
                unsigned char **cb_spatial, **cr_spatial;
            
                for (int mcu = 0; mcu < total_mcus; mcu++) {
                    Cb_final[mcu] = malloc(nb_blocs_Cb * sizeof(unsigned char**));
                    Cr_final[mcu] = malloc(nb_blocs_Cr * sizeof(unsigned char**));
                    for (int b = 0; b < nb_blocs_Cb; b++) {
                        int16_t **cb_bloc = malloc(8 * sizeof(int*));
                        for (int i = 0; i < 8; i++) {
                            cb_bloc[i] = malloc(8 * sizeof(int));
                        }
                        quantification_inverse(Cb_blocs[mcu][b], image->Quant_Table[1]);
                        inverse_zigzag(Cb_blocs[mcu][b], cb_bloc);
                        idct_ptr(cb_bloc,&cb_spatial);
                        
                        if (forbloc == 0) {
                            Cb_final[mcu][b] = cb_spatial;
                        }
                        else if (forbloc ==1) {
                            up_sampling4_2_2_horizontal(cb_spatial,&Cb_final[mcu][b], H_Y, V_Y);
                        }
                        else if (forbloc == 2){
                            up_sampling4_2_2_vertical(cb_spatial,&Cb_final[mcu][b], H_Y, V_Y);
                        }
                        else{
                            up_sampling4_2_0(cb_spatial, &Cb_final[mcu][b], H_Y, V_Y);
                        }
                        print_bloc(Cb_final[mcu][b],mcu,b,"Cb",H_Y,V_Y);
                        for (int i = 0; i < 8; i++) {
                            free(cb_bloc[i]);
                        }
                        free(cb_bloc);
                    }
                    for (int b = 0; b < nb_blocs_Cr; b++) {
                        int16_t **cr_bloc = malloc(8 * sizeof(int*));
                        for (int i = 0; i < 8; i++) {
                                cr_bloc[i] = malloc(8 * sizeof(int));
                            }
                        quantification_inverse(Cr_blocs[mcu][b], image->Quant_Table[1]);
                        inverse_zigzag(Cr_blocs[mcu][b], cr_bloc);
                        idct_ptr(cr_bloc, &cr_spatial);
                        
                        if (forbloc == 0) {
                            Cr_final[mcu][b] = cr_spatial;
                        }
                        else if (forbloc ==1) {
                            up_sampling4_2_2_horizontal(cr_spatial,&Cr_final[mcu][b], H_Y, V_Y);
                        }
                        else if (forbloc == 2){
                            up_sampling4_2_2_vertical(cr_spatial, &Cr_final[mcu][b], H_Y, V_Y);
                        }
                        else{
                            up_sampling4_2_0(cr_spatial, &Cr_final[mcu][b], H_Y, V_Y);
                        }

                        print_bloc(Cr_final[mcu][b],mcu,b,"Cr",H_Y,V_Y);

                        for (int i = 0; i < 8; i++) {
                            free(cr_bloc[i]);
                        }
                        free(cr_bloc);
                    }
                }
    }
    
                    // if (forbloc == 0) {
                    //         Cb_final[mcu][b] = cb_spatial;
                    //         Cr_final[mcu][b] = cr_spatial;
                    //     } else if (forbloc == 1) {
                    //             up_sampling4_2_2(cb_spatial, cr_spatial, &Cb_final[mcu][b], &Cr_final[mcu][b]);
                    //         } else if (forbloc == 2) {
                    //                 up_sampling4_2_0(cb_spatial, cr_spatial, &Cb_final[mcu][b], &Cr_final[mcu][b]);
                    //             }
                    //             print_bloc(Cb_final[mcu][b], mcu, b, "Cb");
                    //             print_bloc(Cr_final[mcu][b], mcu, b, "Cr");
                                
                                //             for (int i = 0; i < 8; i++) {
                                    //                 free(cb_bloc[i]);
                                    //                 free(cr_bloc[i]);
                                    //             }
                                    //             free(cb_bloc);
                                    //             free(cr_bloc);
                                    //         }
                                    //     }
                                    // }

                                    // // Libération mémoire
                                    // free(Y_final);
                                    
                                    
                                    // free spactial_block
    free(spatial_block);
    free_image(image);
    return 0;
}
                            