#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
// Sur-échantillonnage horizontal 4:2:2
void up_sampling4_2_2_horizontal( uint8_t **Cb, uint8_t ***new_Cb, int H_Y);
// Sur-échantillonnage vertical 4:2:2
void up_sampling4_2_2_vertical( uint8_t**Cr, uint8_t ***new_Cr, int V_Y);
// Sur-échantillonnage 4:2:0
void up_sampling4_2_0( uint8_t**Cb, uint8_t***new_Cb, int H_Y, int V_Y);
