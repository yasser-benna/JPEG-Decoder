#pragma once
#include <stdint.h>
// fuction to find the coeff_c
double coeff_c(int xi);
// idct unefficient
void idct_ptr(int **freq_block, unsigned char ***spatial_block);