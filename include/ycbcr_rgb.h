#pragma once

#include <stdint.h>
void YCbCr_to_rgb(uint8_t **Y, uint8_t **Cb, uint8_t **Cr, uint8_t **R, uint8_t **G, uint8_t **B, uint8_t l, uint8_t c);