#pragma once
#include <stdint.h>
// IDCT naive
double coeff_c(int xi);
void idct_naive(int16_t **bloc_freq, unsigned char ***bloc_spatial);
// IDCT rapide
void idct_rapide(int16_t **bloc_freq, unsigned char ***bloc_spatial);
void etape4_idct_rapide(double x[8], double y[8]);
void etape3_idct_rapide(double x[8], double y[8], double c1_1, double c1_2, double c3_1, double c3_2);
void etape2_idct_rapide(double x[8], double y[8], double c6_1, double c6_2);
void etape1_idct_rapide_double(double x[8], double y[8]);
void etpae1_idct_rapide(int16_t x[8], double y[8]);