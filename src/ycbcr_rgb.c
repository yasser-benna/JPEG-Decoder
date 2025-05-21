#include <stdlib.h>
#include <stdint.h>
#include "ycbcr_rgb.h"
#include "reader.h"

/*La fonction YCbCr_to_rgb permet la conversion de YCbCr en RGB en se basant sur les formules simplifiées fournies. Sans
oublier également les conditions de saturation.*/

void YCbCr_to_rgb(uint8_t ***Y_bloc, uint8_t ***Cb_bloc, uint8_t ***Cr_bloc, 
                 uint8_t ***R, uint8_t ***G, uint8_t ***B, 
                 int H_Y, int V_Y, int nb_blocks, int nb_block_C, int sampling) {

    //  Allocation mémoire 
    *R = malloc(8 * V_Y * sizeof(uint8_t*));
    *G = malloc(8 * V_Y * sizeof(uint8_t*));
    *B = malloc(8 * V_Y * sizeof(uint8_t*));
    for (int i = 0; i < 8 * V_Y; i++){
        (*R)[i] = malloc(8 * H_Y * sizeof(uint8_t*));
        (*G)[i] = malloc(8 * H_Y * sizeof(uint8_t*));
        (*B)[i] = malloc(8 * H_Y * sizeof(uint8_t*));
    }
    uint8_t Y, cb, cr;
    for (int i = 0; i < nb_blocks; i++) {
        for (int ligne = 0; ligne < 8; ligne++) {
            for (int colonne = 0; colonne < 8; colonne++) {
                int global_ligne, global_colonne;   // les coordonnées globales dans les matrices cb et cr      

                if (sampling == 3) { // cas du upsampling 4:2:0
                    global_ligne = (int)(i / V_Y) * 8 + ligne;
                    global_colonne = (i % H_Y) * 8 + colonne;
                    Y = Y_bloc[i][ligne][colonne];
                    cb = Cb_bloc[0][global_ligne][global_colonne];
                    cr = Cr_bloc[0][global_ligne][global_colonne];
                } 
                else if (sampling == 2) { // cas du upsampling 4:2:2 vertical
                    global_ligne = (int)(i / H_Y) * 8 + ligne;
                    global_colonne = colonne;
                    Y = Y_bloc[i][ligne][colonne];
                    if (nb_block_C > 1){
                        cb = Cb_bloc[(i%2)][global_ligne][global_colonne];
                        cr = Cr_bloc[(i%2)][global_ligne][global_colonne];
                    }
                    else{
                        cb = Cb_bloc[0][global_ligne][global_colonne];
                        cr = Cr_bloc[0][global_ligne][global_colonne];
                    }
                } 
                else { // cas du upsampling 4:2:2 horizontal
                    global_ligne = ligne;
                    global_colonne = (i % H_Y) * 8 + colonne;
                    Y = Y_bloc[i][ligne][colonne];
                    cb = Cb_bloc[(int)(i/2)][ligne][global_colonne];
                    cr = Cr_bloc[(int)(i/2)][ligne][global_colonne];
                }
                // Utilisation des formules de conversion 
                int r = Y + 1.402 * (cr - 128);
                int g = Y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128);
                int b = Y + 1.772 * (cb - 128);

                // Conditions de saturation
                if (sampling == 2 && nb_block_C > 1){
                    (*R)[global_ligne][(i % H_Y) * 8 + colonne] = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                    (*G)[global_ligne][(i % H_Y) * 8 + colonne] = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                    (*B)[global_ligne][(i % H_Y) * 8 + colonne] = (b < 0) ? 0 : ((b > 255) ? 255 : b);
                }
                else if (sampling == 1 && nb_block_C > 1){
                    (*R)[(int)(i / V_Y) * 8 + ligne][global_colonne] = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                    (*G)[(int)(i / V_Y) * 8 + ligne][global_colonne] = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                    (*B)[(int)(i / V_Y) * 8 + ligne][global_colonne] = (b < 0) ? 0 : ((b > 255) ? 255 : b);
                }
                else {
                    (*R)[global_ligne][global_colonne] = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                    (*G)[global_ligne][global_colonne] = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                    (*B)[global_ligne][global_colonne] = (b < 0) ? 0 : ((b > 255) ? 255 : b);
                }
            }
        }
    }
}

//Test de YCbCr to RGB
// int main() {
//     printf("\n=== Test 4:2:2 Horizontal ===\n");
//     uint8_t **R;
//     uint8_t **G;
//     uint8_t **B;
//     // Dimensions
//     int H_Y = 2, V_Y = 1;  // 1x2 blocs Y -> 8x16
//     int nb_blocks = H_Y * V_Y;  // 2 blocs 8x8
//     uint8_t ***Y_bloc;
//     uint8_t **Cr;
//     uint8_t **Cb;

//     // ===== Allocation mémoire =====
//     // Allocation pour Y_bloc[2][8][8]
//     Y_bloc = malloc(nb_blocks * sizeof(uint8_t **));
//     for (int b = 0; b < nb_blocks; b++) {
//         Y_bloc[b] = malloc(8 * sizeof(uint8_t *));
//         for (int i = 0; i < 8; i++) {
//             Y_bloc[b][i] = malloc(8 * sizeof(uint8_t));
//         }
//     }

//     // Allocation pour Cb[8][8] et Cr[8][8]
//     Cb = malloc(8 * sizeof(uint8_t *));
//     Cr = malloc(8 * sizeof(uint8_t *));
//     for (int i = 0; i < 8; i++) {
//         Cb[i] = malloc(8 * sizeof(uint8_t));
//         Cr[i] = malloc(8 * sizeof(uint8_t));
//     }

//     // Allocation pour R[8][16], G[8][16], B[8][16]
//     R = malloc(8 * sizeof(uint8_t *));
//     G = malloc(8 * sizeof(uint8_t *));
//     B = malloc(8 * sizeof(uint8_t *));
//     for (int i = 0; i < 8; i++) {
//         R[i] = malloc(16 * sizeof(uint8_t));
//         G[i] = malloc(16 * sizeof(uint8_t));
//         B[i] = malloc(16 * sizeof(uint8_t));
//     }

//     // ===== Remplissage avec des valeurs de test =====
//     for (int b = 0; b < nb_blocks; b++) {
//         for (int i = 0; i < 8; i++) {
//             for (int j = 0; j < 8; j++) {
//                 Y_bloc[b][i][j] = 180;  // Y clair
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
//     YCbCr_to_rgb(Y_bloc, Cb, Cr, &R, &G, &B, H_Y, V_Y, nb_blocks, 1);  // 1 = 4:2:2 horizontal
    
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
//     // Libération de Y_bloc[2][8][8]
//     for (int b = 0; b < nb_blocks; b++) {
//         for (int i = 0; i < 8; i++) {
//             free(Y_bloc[b][i]);
//         }
//         free(Y_bloc[b]);
//     }
//     free(Y_bloc);

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



