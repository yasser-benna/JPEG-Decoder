#pragma once
#include <stdint.h>
double coeff_c(int xi);
void idct_ptr(int **freq_block, unsigned char ***spatial_block);