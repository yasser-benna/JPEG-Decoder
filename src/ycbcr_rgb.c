#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ycbcr_rgb.h"
#include "reader.h"

void YCbCr_to_rgb(unsigned char ***Y_blocks, unsigned char **Cb, unsigned char **Cr, 
                 unsigned char ***R, unsigned char ***G, unsigned char ***B, 
                 int H_Y, int V_Y, int nb_blocks, int sampling) {
    *R = malloc(8 * V_Y * sizeof(unsigned char*));
    *G = malloc(8 * V_Y * sizeof(unsigned char*));
    *B = malloc(8 * V_Y * sizeof(unsigned char*));
    for (int i = 0; i < 8 * V_Y; i++){
        (*R)[i] = malloc(8 * H_Y * sizeof(unsigned char*));
        (*G)[i] = malloc(8 * H_Y * sizeof(unsigned char*));
        (*B)[i] = malloc(8 * H_Y * sizeof(unsigned char*));
    }
    
    for (int b = 0; b < nb_blocks; b++) {
        for (int ligne = 0; ligne < 8; ligne++) {
            for (int colonne = 0; colonne < 8; colonne++) {
                int global_ligne, global_colonne;
                
                if (sampling == 3) { // 4 2 0
                    global_ligne = (int)(b / V_Y) * 8 + ligne;
                    global_colonne = (b % H_Y) * 8 + colonne;
                } 
                else if (sampling == 1) { // 4 2 2 horizontal
                    global_ligne = ligne;
                    global_colonne = (b % H_Y) * 8 + colonne;
                } 
                else { // 4 2 2 vertical
                    global_ligne = (int)(b / H_Y) * 8 + ligne;
                    global_colonne = colonne;
                }

                unsigned char Y = Y_blocks[b][ligne][colonne];
                unsigned char cb = Cb[global_ligne][global_colonne];
                unsigned char cr = Cr[global_ligne][global_colonne];

                int r = Y + 1.402 * (cr - 128);
                int g = Y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128);
                int b = Y + 1.772 * (cb - 128);

                (*R)[global_ligne][global_colonne] = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                (*G)[global_ligne][global_colonne] = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                (*B)[global_ligne][global_colonne] = (b < 0) ? 0 : ((b > 255) ? 255 : b);
            }
        }
    }
}


// int main() {
//     printf("\n=== Test 4:2:2 Horizontal ===\n");
//     unsigned char **R;
//     unsigned char **G;
//     unsigned char **B;
//     // Dimensions
//     int H_Y = 2, V_Y = 1;  // 1x2 blocs Y -> 8x16
//     int nb_blocks = H_Y * V_Y;  // 2 blocs 8x8
//     unsigned char ***Y_blocks;
//     unsigned char **Cr;
//     unsigned char **Cb;

//     // ===== Allocation mémoire =====
//     // Allocation pour Y_blocks[2][8][8]
//     Y_blocks = malloc(nb_blocks * sizeof(unsigned char **));
//     for (int b = 0; b < nb_blocks; b++) {
//         Y_blocks[b] = malloc(8 * sizeof(unsigned char *));
//         for (int i = 0; i < 8; i++) {
//             Y_blocks[b][i] = malloc(8 * sizeof(unsigned char));
//         }
//     }

//     // Allocation pour Cb[8][8] et Cr[8][8]
//     Cb = malloc(8 * sizeof(unsigned char *));
//     Cr = malloc(8 * sizeof(unsigned char *));
//     for (int i = 0; i < 8; i++) {
//         Cb[i] = malloc(8 * sizeof(unsigned char));
//         Cr[i] = malloc(8 * sizeof(unsigned char));
//     }

//     // Allocation pour R[8][16], G[8][16], B[8][16]
//     R = malloc(8 * sizeof(unsigned char *));
//     G = malloc(8 * sizeof(unsigned char *));
//     B = malloc(8 * sizeof(unsigned char *));
//     for (int i = 0; i < 8; i++) {
//         R[i] = malloc(16 * sizeof(unsigned char));
//         G[i] = malloc(16 * sizeof(unsigned char));
//         B[i] = malloc(16 * sizeof(unsigned char));
//     }

//     // ===== Remplissage avec des valeurs de test =====
//     for (int b = 0; b < nb_blocks; b++) {
//         for (int i = 0; i < 8; i++) {
//             for (int j = 0; j < 8; j++) {
//                 Y_blocks[b][i][j] = 180;  // Y clair
//             }
//         }
//     }
    
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             Cb[i][j] = 120;  // Cb moyen
//             Cr[i][j] = 180;  // Cr élevé (rouge)
//         }
//     }
    
//     // ===== Appel de la fonction =====
//     YCbCr_to_rgb(Y_blocks, Cb, Cr, &R, &G, &B, H_Y, V_Y, nb_blocks, 1);  // 1 = 4:2:2 horizontal
    
//     // ===== Vérification =====
//     printf("R[0][0] = %d (attendu ~255)\n", R[0][0]);
//     printf("G[0][0] = %d (attendu ~150)\n", G[0][0]);
//     printf("B[0][0] = %d (attendu ~200)\n", B[0][0]);
//     for (int i = 0; i < 8 * V_Y; i++) {
//         for (int j = 0; j < 8* H_Y; j++) {
//             printf("%3d ", R[i][j]);
//         }
//         printf("\n");
//     }
    
//     // ===== Libération mémoire =====
//     // Libération de Y_blocks[2][8][8]
//     for (int b = 0; b < nb_blocks; b++) {
//         for (int i = 0; i < 8; i++) {
//             free(Y_blocks[b][i]);
//         }
//         free(Y_blocks[b]);
//     }
//     free(Y_blocks);

//     // Libération de Cb[8][8] et Cr[8][8]
//     for (int i = 0; i < 8; i++) {
//         free(Cb[i]);
//         free(Cr[i]);
//     }
//     free(Cb);
//     free(Cr);

//     // Libération de R[8][16], G[8][16], B[8][16]
//     for (int i = 0; i < 8; i++) {
//         free(R[i]);
//         free(G[i]);
//         free(B[i]);
//     }
//     free(R);
//     free(G);
//     free(B);

//     return 0;
// }

















































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
