#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "idct.h"
#define N 8
#define PI 3.14159265358979323846
// Function to find the coeff_c
double coeff_c(int xi) {
    if(xi < 0 || xi >= N) {
        fprintf(stderr, "Error: xi must be in the range [0, %d)\n", N);
        exit(0);
    }
    if (xi==0){
        return 1.0 / sqrt(2.0);
    }
    return  1.0;
}
// Function to perform the IDCT unefficiently
void idct_ptr(int **freq_block, unsigned char ***spatial_block) {
    *spatial_block = malloc(N * sizeof(unsigned char *));
    double sum = 0.0;
    double ci=0.0;
    double cj=0.0;
    double cos1=0.0;
    double cos2=0.0;
    double val=0.0;
    for (int k = 0; k < N; k++) {
        (*spatial_block)[k] = malloc(N * sizeof(unsigned char *));
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            sum = 0.0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    ci = coeff_c(i);
                    cj = coeff_c(j);
                    cos1 = cos(((2 * x + 1) * i * PI) / (2.0 * N));
                    cos2 = cos(((2 * y + 1) * j * PI) / (2.0 * N));
                    sum += ci * cj * cos1 * cos2 * freq_block[i][j];
                }
            }
            val = sum / sqrt(2.0 * N) + 128.0;
            if (val < 0.0) val = 0.0;
            if (val > 255.0) val = 255.0;
            (*spatial_block)[x][y] = (unsigned char)(round(val));
        }
    }
}

// Test function

// int main(){
//     int16_t **freq_block;
//     unsigned char **spatial_block;
//     int16_t data[8][8] = {
//         {0x007c, 0x0000, 0xfee4, 0x0000, 0x0000, 0x0000, 0xffec, 0x0000},
//         {0xfffa, 0x0000, 0xfedb, 0x0000, 0x006a, 0x0000, 0xff7d, 0x0000},
//         {0xfeb3, 0x0000, 0xff8b, 0x0000, 0x0045, 0x0000, 0x009a, 0x0000},
//         {0xff36, 0x0000, 0xffa0, 0x0000, 0x0019, 0x0000, 0x0004, 0x0000},
//         {0x0080, 0x0000, 0x011c, 0x0000, 0x00ff, 0x0000, 0x0014, 0x0000},
//         {0xffa6, 0x0000, 0x0013, 0x0000, 0x007d, 0x0000, 0xfe1f, 0x0000},
//         {0xff76, 0x0000, 0x001a, 0x0000, 0xff59, 0x0000, 0x00f4, 0x0000},
//         {0x00dd, 0x0000, 0xffa8, 0x0000, 0xffb9, 0x0000, 0xff3c, 0x0000}
//     };
//     freq_block = malloc(N * sizeof(int *));
//     for (int i = 0; i < N; i++) {
//         freq_block[i] = malloc(N * sizeof(int));
//         for (int j = 0; j < N; j++) {
//             freq_block[i][j] = data[i][j];
//         }
//     }
//     idct_ptr(freq_block, &spatial_block);
//     printf("Bloc spatial (aprÃ¨s IDCT) :\n");
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             printf("%3d ", spatial_block[i][j]);
//         }
//         printf("\n");
//     }
//     for (int k=0;k<N;k++){
//         free(freq_block[k]);
//         free(spatial_block[k]);
//     }
//     free(freq_block);
//     free(spatial_block);
//     return 0;
// }
