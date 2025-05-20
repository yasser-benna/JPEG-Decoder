#pragma once
#include <stdint.h>
#include <stdio.h>
typedef struct{
    uint8_t** R;        // Canal rouge
    uint8_t** B;        // Canal bleu
    uint8_t** G;        // Canal vert
    int width;          // Largeur de l'image
    int height;         // Hauteur de l'image
    int colors;         // Nombre de couleurs
}IMAGE_D;

typedef struct{
    uint8_t tab[8][8];  // Tableau 8x8 pour stocker les données de cellule


}cell;
// Libérer la mémoire d'une image
void free_image_d(IMAGE_D* image);
// Écrire l'image dans un fichier
void write_image_d(FILE*file,IMAGE_D image);
// Initialiser une structure d'image
IMAGE_D* init_image_d(int width,int height,int colors);
// Copier les données MCU vers l'image finale
void copy_mcu_to_image(IMAGE_D* image_d, unsigned char** R, unsigned char** G, 
                       unsigned char** B, int mcu, int nb_mcus_x, 
                       int H_Y, int V_Y);
