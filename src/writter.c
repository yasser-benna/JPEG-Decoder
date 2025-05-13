#include "writter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

IMAGE_D* init_image_d(){
    IMAGE_D* image = malloc(sizeof(IMAGE_D));
    if (!image) {
        fprintf(stderr, "Erreur d'allocation IMAGE_D\n");
        exit(1);
    }
    image->R = NULL;
    image->G = NULL;
    image->B = NULL;
    image->width = 0;
    image->height = 0;
    image->colors = 0;
    return image;
}



void free_image(IMAGE_D* image){
    if(image->R != NULL) {
        free(image->R);
    }
    if(image->G != NULL) {
        free(image->G);
    }
    if(image->B != NULL) {
        free(image->B);
    }
    //free(image);
}
void write_image(const char* file_name,IMAGE_D image){
    FILE* file = fopen(file_name, "wa");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", file_name);
        return;
    }
    
    if(image.colors){
        fprintf(file, "P6\n%d %d\n255\n", image.width, image.height);
    }else{
        fprintf(file, "P5\n%d %d\n255\n", image.width, image.height);
    }
    for(int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            if(image.colors){
                fprintf(file,"%04u%04u%04u ",image.R[i * image.width + j],image.G[i * image.width + j],image.B[i * image.width + j]);
            }else{
                fprintf(file,"%04u ",image.R[i * image.width + j]);
            }
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
    free_image(&image);
    fclose(file);

}