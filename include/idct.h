#pragma once
#include <stdint.h>
// IDCT naïve
// Calcul du coefficient C(u) pour la fonction cosinus
double coeff_c(int xi);
// Implémentation naïve de la DCT inverse
void idct_naive(int16_t **bloc_freq, unsigned char ***bloc_spatial);
// IDCT rapide
// Implémentation rapide de la DCT inverse
void idct_rapide(int16_t **bloc_freq, unsigned char ***bloc_spatial);
// Étape 4 de l'algorithme IDCT rapide
void etape4_idct_rapide(double x[8], double y[8]);
// Étape 3 de l'algorithme IDCT rapide
void etape3_idct_rapide(double x[8], double y[8], double c1_1, double c1_2, double c3_1, double c3_2);
// Étape 2 de l'algorithme IDCT rapide
void etape2_idct_rapide(double x[8], double y[8], double c6_1, double c6_2);
// Étape 1 de l'algorithme IDCT rapide (version double)
void etape1_idct_rapide_double(double x[8], double y[8]);
// Étape 1 de l'algorithme IDCT rapide
void etpae1_idct_rapide(int16_t x[8], double y[8]);