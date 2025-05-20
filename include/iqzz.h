#pragma once
#include <stdint.h>
// Fonction pour appliquer la quantification inverse aux coefficients DCT
void quantification_inverse(int *dct_coeff,const uint16_t *quantification_table);