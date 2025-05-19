#include "../include/writter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

IMAGE_D* init_image_d(int width, int height, int colors) {
    IMAGE_D* image = malloc(sizeof(IMAGE_D));
    if (!image) {
        fprintf(stderr, "Erreur d'allocation IMAGE_D\n");
        exit(1);
    }
    if(colors == 0){
        image->R = malloc(height * sizeof(uint8_t*));
        image->G = NULL;
        image->B = NULL;
        for (int i = 0; i < height; i++) {
            image->R[i] = malloc(width * sizeof(uint8_t));
        }
    }else{
        image->R = malloc(height * sizeof(uint8_t*));
        image->G = malloc(height * sizeof(uint8_t*));
        image->B = malloc(height * sizeof(uint8_t*));
        for (int i = 0; i < height; i++) {
            image->R[i] = malloc(width * sizeof(uint8_t));
            image->G[i] = malloc(width * sizeof(uint8_t));
            image->B[i] = malloc(width * sizeof(uint8_t));
        }
    }
    image->width = width;
    image->height = height;
    image->colors = colors;
    return image;
}



void free_image_d(IMAGE_D* image){
    if (image->colors == 0) {
        for (int i = 0; i < image->height; i++) {
            free(image->R[i]);
        }
        free(image->R);
    } else {
        for (int i = 0; i < image->height; i++) {
            free(image->R[i]);
            free(image->G[i]);
            free(image->B[i]);
        }
        free(image->R);
        free(image->G);
        free(image->B);
    }
    free(image);
}

void write_image_d(FILE*file, IMAGE_D image){
    if (image.colors == 0) {
        fprintf(file, "P5\n%d %d\n255\n", image.width, image.height);
        for (int i = 0; i < image.height; i++) {
            for(int j=0;j<image.width;j++){
                fputc(image.R[i][j], file);
            }
        }
    } else {
        fprintf(file, "P6\n%d %d\n255\n", image.width, image.height);
        for (int i = 0; i < image.height; i++) {
            for(int j=0;j<image.width;j++){
                fputc(image.R[i][j], file); // Red
                fputc(image.G[i][j], file); // Green
                fputc(image.B[i][j], file); // Blue
                }
        }
            }
            
    }
void copy_mcu_to_image(IMAGE_D* image_d, uint8_t** R, uint8_t** G, 
                       uint8_t** B, int mcu, int nb_mcus_x, 
                       int H_Y, int V_Y) {
    // Calculez la position du MCU dans l'image
    int mcu_x = mcu % nb_mcus_x;
    int mcu_y = mcu / nb_mcus_x;
    
    // Position de départ (en pixels) pour ce MCU dans l'image
    int start_x = mcu_x * 8 * H_Y;
    int start_y = mcu_y * 8 * V_Y;
    
    // Pour chaque pixel du MCU
    for (int y = 0; y < 8 * V_Y; y++) {
        for (int x = 0; x < 8 * H_Y; x++) {
            // Position de ce pixel dans l'image finale
            int img_x = start_x + x;
            int img_y = start_y + y;
            
            // Vérifier si on est dans les limites de l'image
            if (img_x < image_d->width && img_y < image_d->height) {
                
                // Copier les valeurs RGB
                image_d->R[img_y][img_x] = R[y][x];
                if(image_d->colors == 1){
                image_d->G[img_y][img_x] = G[y][x];
                image_d->B[img_y][img_x] = B[y][x];
            }
            }
        }
    }
}
