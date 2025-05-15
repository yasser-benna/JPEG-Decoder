#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void up_sampling4_2_2_horizontal( unsigned char **Cb, unsigned char ***new_Cb, int H_Y, int V_Y);
void up_sampling4_2_2_vertical( unsigned char**Cr, unsigned char ***new_Cr, int H_Y, int V_Y);
void up_sampling4_2_0( unsigned char**Cb, unsigned char***new_Cb, int H_Y, int V_Y);
