#pragma once

#include <stdint.h>

// Function to perform the inverse quantification
void quantification_inverse(int *dct_coeff,const uint16_t *quantification_table);