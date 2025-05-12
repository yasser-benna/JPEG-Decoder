#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8

void quantification_inverse(int *dct_coeff, const uint16_t *quant_table) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dct_coeff[i * N + j] *= quant_table[i * N + j];
        }
    }
}

// int main() {
//     int *dct_coeff = malloc(N * N * sizeof(int));
//     uint16_t quant_table[N * N] = {
//         1, 2, 3, 4, 5, 6, 7, 8,
//         9, 10, 11, 12, 13, 14, 15, 16,
//         17, 18, 19, 20, 21, 22, 23, 24,
//         25, 26, 27, 28, 29, 30, 31, 32,
//         33, 34, 35, 36, 37, 38, 39, 40,
//         41, 42, 43, 44, 45, 46, 47, 48,
//         49, 50, 51, 52, 53, 54, 55, 56,
//         57, 58, 59, 60, 61, 62, 63, 64
//     };
//     int dct_data[N * N] = {
//         2, 2, 2, 2, 2, 2, 2, 2,
//         2, 2, 2, 2, 2, 2, 2, 2,
//         0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0,
//         0, 0, 0, 0, 0, 0, 0, 0
//     };
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             dct_coeff[i *N +j] = dct_data[i * N +j];
//         }
//     }

//     quantification_inverse(dct_coeff, quant_table);

//     printf("Résultat de la quantification inverse :\n");
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             printf("%4d ", dct_coeff[i * N +j]);
//         }
//         printf("\n");
//     }

//     free(dct_coeff);

//     return 0;
// }