#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "idct.h"
#define N 8
#define PI 3.14159265358979323846
#define INV_SQRT2 0.70710678

double coeff_c(int xi) {
    return (xi==0) ? 1.0 / sqrt(2.0) : 1.0;
}
void idct_ptr(int16_t **freq_block, unsigned char ***spatial_block) {
    *spatial_block = malloc(N * sizeof(unsigned char *));
    for (int k = 0; k < N; k++) {
        (*spatial_block)[k] = malloc(N * sizeof(unsigned char));
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            double sum = 0.0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    double ci = coeff_c(i);
                    double cj = coeff_c(j);
                    double cos1 = cos(((2 * x + 1) * i * PI) / (2.0 * N));
                    double cos2 = cos(((2 * y + 1) * j * PI) / (2.0 * N));
                    sum += ci * cj * cos1 * cos2 * freq_block[i][j];
                }
            }
            double val = sum / sqrt(2.0 * N) + 128.0;
            if (val < 0.0) val = 0.0;
            if (val > 255.0) val = 255.0;

            (*spatial_block)[x][y] = (unsigned char)(round(val));
        }
    }
}


void step1_idct(int16_t x[8], double y[8]){
    y[0] = x[0];
    y[1] = x[4];
    y[2] = x[2];
    y[3] = x[6];
    y[4] = (x[1] - x[7]) / 2;
    y[5] = x[3] * INV_SQRT2;
    y[6] = x[5] * INV_SQRT2;
    y[7] = (x[1] + x[7]) / 2;
}

void step1_idct_double(double x[8], double y[8]){
    y[0] = x[0];
    y[1] = x[4];
    y[2] = x[2];
    y[3] = x[6];
    y[4] = (x[1] - x[7]) / 2;
    y[5] = x[3] * INV_SQRT2;
    y[6] = x[5] * INV_SQRT2;
    y[7] = (x[1] + x[7]) / 2;
}
void step2_idct(double x[8], double y[8], double c6_1, double c6_2){
    y[0] = (x[0] + x[1])/2;
    y[1] = (x[0] - x[1])/2;
    y[2] = (c6_1 * x[2] - c6_2 * x[3]) * INV_SQRT2;
    y[3] = (c6_1 * x[3] + c6_2 * x[2]) * INV_SQRT2;
    y[4] = (x[4] + x[6]) / 2;
    y[5] = (x[7] - x[5]) / 2;
    y[6] = (x[4] - x[6]) / 2;
    y[7] = (x[7] + x[5]) / 2;
}

void step3_idct(double x[8], double y[8], double c1_1, double c1_2, double c3_1, double c3_2){
    y[0] = (x[0] + x[3])/2;
    y[1] = (x[1] + x[2])/2;
    y[2] = (x[1] - x[2]) / 2;
    y[3] = (x[0] - x[3]) / 2;
    y[4] = c3_1 * x[4] - c3_2 * x[7];
    y[5] = c1_1 * x[5] - c1_2 * x[6];
    y[6] = c1_1 * x[6] + c1_2 * x[5];
    y[7] = c3_1 * x[7] + c3_2 * x[4];
}

void step4_idct(double x[8], double y[8]){
    y[0] = (x[0] + x[7]) / 2;
    y[1] = (x[1] + x[6]) / 2;
    y[2] = (x[2] + x[5]) / 2;
    y[3] = (x[3] + x[4]) / 2;
    y[4] = (x[3] - x[4]) / 2;
    y[5] = (x[2] - x[5]) / 2;
    y[6] = (x[1] - x[6]) / 2;
    y[7] = (x[0] - x[7]) / 2;
}

void idct_fast_asf_boi(int16_t **freq_block, unsigned char ***spatial_block) {
    double c6_1 = cos(3*PI/8);
    double c6_2 = sin(3*PI/8);
    double c3_1 = cos(3*PI/16);
    double c3_2 = sin(3*PI/16);
    double c1_1 = cos(PI/16);
    double c1_2 = sin(PI/16);
    double buff1[8], buff2[8];
    double tmp[8][8];
    double values[8];
    *spatial_block = malloc(N * sizeof(unsigned char *));
    for (int i = 0; i < 8; i++) {
        (*spatial_block)[i] = malloc(N * sizeof(unsigned char));
    }
    for (int i = 0; i< 8; i++) {
        step1_idct(freq_block[i], buff1);
        step2_idct(buff1, buff2, c6_1, c6_2);
        step3_idct(buff2, buff1, c1_1, c1_2, c3_1, c3_2);
        step4_idct(buff1, buff2);
        for (int j = 0; j < 8; j++)
            tmp[i][j] = buff2[j];
    }
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++){
            values[i] = tmp[i][j];
        }
        step1_idct_double(values, buff1);
        step2_idct(buff1, buff2, c6_1, c6_2);
        step3_idct(buff2, buff1, c1_1, c1_2, c3_1, c3_2);
        step4_idct(buff1, buff2);
        for (int i = 0; i < 8; ++i) {
            double val = buff2[i] * 8 + 128;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            (*spatial_block)[i][j] = (unsigned char)(round(val));
        }
    }
}

// int main(){
//     int16_t **freq;
//     unsigned char **spatial;
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
// //     int16_t data[8][8] = {
// //     { 0x7A3F, 0xC12D, 0x1340, 0x89FE, 0x45A1, 0x22B7, 0x9D3C, 0x6F10 },
// //     { 0xBEEF, 0x0172, 0xD443, 0x388A, 0x7041, 0xAC93, 0x1FAB, 0x533C },
// //     { 0xDEAD, 0x4321, 0xA7B0, 0x0F0F, 0xEE11, 0x1200, 0x77FE, 0x3C9D },
// //     { 0xABCD, 0x1357, 0xACE0, 0x04B3, 0x29FA, 0xF1E0, 0x6A2C, 0xDCBA },
// //     { 0x1023, 0x7FFF, 0x8001, 0x1111, 0x9999, 0xB2B2, 0x2E2E, 0x3F3F },
// //     { 0x4455, 0x8899, 0xAAEE, 0xFF00, 0x00FF, 0xC0DE, 0xBABE, 0xFEED },
// //     { 0x1A2B, 0x3C4D, 0x5E6F, 0x7F80, 0x8A9B, 0xA1B2, 0xC3D4, 0xE5F6 },
// //     { 0x0000, 0xFFFF, 0x135E, 0x24AC, 0x5D7B, 0x8FC0, 0xD2D2, 0xE1E1 }
// // };
//     freq = malloc(N * sizeof(int*));
//     for (int i = 0; i < N; i++) {
//         freq[i] = malloc(N * sizeof(int));
//         for (int j = 0; j < N; j++) {
//             freq[i][j] = data[i][j];
//         }
//     }
//     idct_fast_asf_boi(freq, &spatial);
//     printf("Bloc spatial (aprÃ¨s IDCT) :\n");
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             printf("%3d ", spatial[i][j]);
//         }
//         printf("\n");
//     }
//     for (int k=0;k<N;k++){
//         free(freq[k]);
//         free(spatial[k]);
//     }
//     free(freq);
//     free(spatial);
//     return 0;
// }



// int main(){
//     int16_t **freq_block;
//     unsigned char **spatial_block;
//     // int data[8][8] = {
//     //     {0x007c, 0x0000, 0xfee4, 0x0000, 0x0000, 0x0000, 0xffec, 0x0000},
//     //     {0xfffa, 0x0000, 0xfedb, 0x0000, 0x006a, 0x0000, 0xff7d, 0x0000},
//     //     {0xfeb3, 0x0000, 0xff8b, 0x0000, 0x0045, 0x0000, 0x009a, 0x0000},
//     //     {0xff36, 0x0000, 0xffa0, 0x0000, 0x0019, 0x0000, 0x0004, 0x0000},
//     //     {0x0080, 0x0000, 0x011c, 0x0000, 0x00ff, 0x0000, 0x0014, 0x0000},
//     //     {0xffa6, 0x0000, 0x0013, 0x0000, 0x007d, 0x0000, 0xfe1f, 0x0000},
//     //     {0xff76, 0x0000, 0x001a, 0x0000, 0xff59, 0x0000, 0x00f4, 0x0000},
//     //     {0x00dd, 0x0000, 0xffa8, 0x0000, 0xffb9, 0x0000, 0xff3c, 0x0000}
//     // };

//     int16_t data[8][8] = {
//         {0xffdf, 0xff6c, 0xfff6, 0x0018, 0x0010, 0xffe8, 0xfff8, 0x0014},
//         {0x006c, 0xffc8, 0xffe4, 0x0006, 0xfff8, 0x001b, 0x0000, 0xfff7},
//         {0xffea, 0xfff0, 0x0030, 0x0018, 0xffac, 0x0036, 0x000b, 0xffee},
//         {0xffa0, 0x0012, 0x0030, 0x0019, 0x0008, 0xffe4, 0xfff3, 0x0014},
//         {0xffa0, 0x0038, 0x0006, 0x0012, 0xffea, 0x0011, 0xfff0, 0xfff4},
//         {0x0018, 0xffc4, 0x0024, 0x000a, 0xffe6, 0x0000, 0x0012, 0x0000},
//         {0xffd0, 0x0028, 0x0000, 0xfff2, 0x0010, 0x0000, 0x0000, 0x0010},
//         {0x0000, 0x0000, 0x0000, 0x0000, 0xffee, 0x0010, 0x0010, 0x0000}
//     };
//     freq_block = malloc(N * sizeof(int *));
//     for (int i = 0; i < N; i++) {
//         freq_block[i] = malloc(N * sizeof(int));
//         for (int j = 0; j < N; j++) {
//             freq_block[i][j] = data[i][j];
//         }
//     }
//     idct_ptr(freq_block, &spatial_block);
//     printf("Bloc spatial (après IDCT) :\n");
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