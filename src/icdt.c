#include <stdio.h>
#include <math.h>
#include "icdt.h"

#define N 8
#define PI 3.14159265358979323846

double coeff_c(int xi) {
    if (xi==0){
        return 1.0 / sqrt(2.0 * N);
    }
    return 1.0;
}
void idct_naive(const double *freq_block, unsigned char *spatial_block) {
    // Initialisation des variables pour le calcul de la DCT
    double sum = 0.0; 
    double ci = 0.0;
    double cj = 0.0;
    double cos1=0.0;
    double cos2=0.0;
    double val=0.0;
    // Boucle sur les pixels de l'image de sortie
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            sum = 0.0;
            // i c'est lambda et j c'est mu
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    ci = coeff_c(i);
                    cj = coeff_c(j);
                    cos1 = cos(((2 * x + 1) * i * PI) / (2.0 * N));
                    cos2 = cos(((2 * y + 1) * j * PI) / (2.0 * N));
                    sum += ci * cj * cos1 * cos2 * freq_block[i*N + j];
                }
            }
            val = sum / sqrt(2.0 * N) + 128.0;
            if (val < 0.0) {
                val = 0.0;
            }
            else if (val > 255.0) {
                val = 255.0;
            }

            spatial_block[x * N + y] = (unsigned char)(round(val));
        }
    }
}