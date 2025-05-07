#include <stdint.h>
#include "iqzz.h"

void quantification_inverse(double *dct_coeff, const uint8_t *quantification_table) {
    for (int i = 0; i < 64; i++) {
            dct_coeff[i] *= quantification_table[i];
    }
}