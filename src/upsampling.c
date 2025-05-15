#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <upsampling.h>
#define N 8

void up_sampling4_2_2_Cb(unsigned char**Cb,unsigned char ***new_Cb){
    *new_Cb = malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        (*new_Cb)[i] = malloc(2 * N * sizeof(int));
        
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            (*new_Cb)[i][2*j] = Cb[i][j];            
            (*new_Cb)[i][2*j + 1] = Cb[i][j];
            
        }
    }   
}

void up_sampling4_2_2_Cr(unsigned char**Cr,unsigned char ***new_Cr){
    *new_Cr = malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        (*new_Cr)[i] = malloc(2 * N * sizeof(int));
        
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            (*new_Cr)[i][2*j] = Cr[i][j];            
            (*new_Cr)[i][2*j + 1] = Cr[i][j];
            
        }
    }   
}


void up_sampling4_2_0_Cb(unsigned char**Cb, unsigned char ***new_Cb) {
    *new_Cb = malloc(2 * N * sizeof(int *));
    for (int i = 0; i < 2 * N; i++) {
        (*new_Cb)[i] = malloc(2 * N * sizeof(int));
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int val_cb = Cb[i][j];
            
            (*new_Cb)[2*i][2*j]     = val_cb;
            (*new_Cb)[2*i][2*j+1]   = val_cb;
            (*new_Cb)[2*i+1][2*j]   = val_cb;
            (*new_Cb)[2*i+1][2*j+1] = val_cb;

        }
    }
}


void up_sampling4_2_0_Cr(unsigned char **Cr, unsigned char ***new_Cr) {
    *new_Cr = malloc(2 * N * sizeof(int *));
    for (int i = 0; i < 2 * N; i++) {
        (*new_Cr)[i] = malloc(2 * N * sizeof(int));
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int val_cr = Cr[i][j];            
            (*new_Cr)[2*i][2*j]     = val_cr;
            (*new_Cr)[2*i][2*j+1]   = val_cr;
            (*new_Cr)[2*i+1][2*j]   = val_cr;
            (*new_Cr)[2*i+1][2*j+1] = val_cr;

        }
    }
}





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
//             int val_cb = Cb[i][j];
//             int val_cr = Cr[i][j];
//             (*new_Cb)[2*i][2*j]     = val_cb;
//             (*new_Cb)[2*i][2*j+1]   = val_cb;
//             (*new_Cb)[2*i+1][2*j]   = val_cb;
//             (*new_Cb)[2*i+1][2*j+1] = val_cb;

//             (*new_Cr)[2*i][2*j]     = val_cr;
//             (*new_Cr)[2*i][2*j+1]   = val_cr;
//             (*new_Cr)[2*i+1][2*j]   = val_cr;
//             (*new_Cr)[2*i+1][2*j+1] = val_cr;
//         }
//     }
// }
