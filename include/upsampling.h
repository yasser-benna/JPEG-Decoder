#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
void up_sampling4_2_2_horizontal( uint8_t **Cb, uint8_t ***new_Cb, int H_Y, int V_Y);
void up_sampling4_2_2_vertical( uint8_t**Cr, uint8_t ***new_Cr, int H_Y, int V_Y);
void up_sampling4_2_0( uint8_t**Cb, uint8_t***new_Cb, int H_Y, int V_Y);
