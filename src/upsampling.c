#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "upsampling.h"
#define N 8

//Up_sampling 4:2:2 horizontal
void up_sampling4_2_2_horizontal(uint8_t **C, uint8_t ***new_C, int H_Y){
    *new_C = malloc(N * sizeof(uint8_t*));
    for (int i = 0; i < N; i++) {
        (*new_C)[i] = malloc(N * H_Y * sizeof(uint8_t));
        
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            (*new_C)[i][2*j] = C[i][j];            
            (*new_C)[i][2*j + 1] = C[i][j];
            
        }
    }   
}

//Up_sampling 4:2:2 vertical
void up_sampling4_2_2_vertical(uint8_t**C,uint8_t ***new_C, int V_Y){
    *new_C = malloc(V_Y * N * sizeof(uint8_t*));
    for (int i = 0; i < V_Y * N; i++) {
        (*new_C)[i] = malloc(N * sizeof(uint8_t));
        
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <  N; j++) {
            (*new_C)[2*i][j] = C[i][j];            
            (*new_C)[2*i+1][j] = C[i][j];
            
        }
    }   
}

//Up_sampling 4:2:0
void up_sampling4_2_0(uint8_t **C, uint8_t ***new_C, int H_Y, int V_Y) {
    *new_C = malloc(V_Y * N * sizeof(uint8_t *));
    for (int i = 0; i < V_Y * N; i++) {
        (*new_C)[i] = malloc(H_Y * N * sizeof(uint8_t));
    }

    for (int i = 0; i <  N; i++) {
        for (int j = 0; j < N; j++) {
            (*new_C)[2*i][2*j]     = C[i][j];
            (*new_C)[2*i][2*j+1]   = C[i][j];
            (*new_C)[2*i+1][2*j]   = C[i][j];
            (*new_C)[2*i+1][2*j+1] = C[i][j];

        }
    }
}

//Test de up_sampling
// void afficher_matrice(int **matrice, int lignes, int colonnes, const char *nom) {
//     printf("Matrice ww %s :\n", nom);
//     for (int i = 0; i < lignes; i++) {
//         for (int j = 0; j < colonnes; j++) {
//             printf("%4d", matrice[i][j]);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

// int main() {
//     int **Cb = malloc(N * sizeof(int *));
//     int **Cr = malloc(N * sizeof(int *));
//     for (int i = 0; i < N; i++) {
//         Cb[i] = malloc(N * sizeof(int));
//         Cr[i] = malloc(N * sizeof(int));
//         for (int j = 0; j < N; j++) {
//             Cb[i][j] = i + j;          
//             Cr[i][j] = (i + 1) * (j + 1); 
//         }
//     }

//     int **new_Cb, **new_Cr;

//     up_sampling4_2_2(Cr, Cb, &new_Cr, &new_Cb);

//     afficher_matrice(Cb, N, N, "Cb (avant upsampling)");
//     afficher_matrice(new_Cb, N, 2*N, "Cb (après upsampling)");
//     afficher_matrice(Cr, N, N, "Cr (avant upsampling)");
//     afficher_matrice(new_Cr, N, 2*N, "Cr (après upsampling)");

//     // Libération mémoire
//     for (int i = 0; i < N; i++) {
//         free(Cb[i]);
//         free(Cr[i]);
//         free(new_Cb[i]);
//         free(new_Cr[i]);
//     }
//     free(Cb);
//     free(Cr);
//     free(new_Cb);
//     free(new_Cr);

//     return 0;
// }




// void up_sampling4_2_2(int **Cr, int **Cb, int ***new_Cr, int ***new_Cb){
//     *new_Cr = malloc(N * sizeof(int*));
//     *new_Cb = malloc(N * sizeof(int*));
//     for (int i = 0; i < N; i++) {
//         (*new_Cb)[i] = malloc(2 * N * sizeof(int));
//         (*new_Cr)[i] = malloc(2 * N * sizeof(int));
//     }
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             (*new_Cb)[i][2*j] = Cb[i][j];
//             (*new_Cr)[i][2*j] = Cr[i][j];
//             (*new_Cb)[i][2*j + 1] = Cb[i][j];
//             (*new_Cr)[i][2*j + 1] = Cr[i][j];
//         }
//     }   
// }


// void up_sampling4_2_0(int **Cr, int **Cb, int ***new_Cr, int ***new_Cb) {
//     *new_Cr = malloc(2 * N * sizeof(int *));
//     *new_Cb = malloc(2 * N * sizeof(int *));
//     for (int i = 0; i < 2 * N; i++) {
//         (*new_Cr)[i] = malloc(2 * N * sizeof(int));
//         (*new_Cb)[i] = malloc(2 * N * sizeof(int));
//     }

//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             int C[i][j] = Cb[i][j];
//             int val_cr = Cr[i][j];
//             (*new_Cb)[2*i][2*j]     = C[i][j];
//             (*new_Cb)[2*i][2*j+1]   = C[i][j];
//             (*new_Cb)[2*i+1][2*j]   = C[i][j];
//             (*new_Cb)[2*i+1][2*j+1] = C[i][j];

//             (*new_Cr)[2*i][2*j]     = val_cr;
//             (*new_Cr)[2*i][2*j+1]   = val_cr;
//             (*new_Cr)[2*i+1][2*j]   = val_cr;
//             (*new_Cr)[2*i+1][2*j+1] = val_cr;
//         }
//     }
// }
