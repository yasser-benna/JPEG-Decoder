#pragma once
#include <stdint.h>
// fuction to find the coeff_c
double coeff_c(int xi);
// idct unefficient
void idct_ptr(int16_t **freq_block, unsigned char ***spatial_block);
void idct_fast_asf_boi(int16_t **freq_block, unsigned char ***spatial_block);
void step4_idct(double x[8], double y[8]);
void step3_idct(double x[8], double y[8], double c1_1, double c1_2, double c3_1, double c3_2);
void step2_idct(double x[8], double y[8], double c6_1, double c6_2);
void step1_idct_double(double x[8], double y[8]);
void step1_idct(int16_t x[8], double y[8]);