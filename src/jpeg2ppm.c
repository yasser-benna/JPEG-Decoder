#include <stdlib.h>
#include <stdio.h>
#include "writter.h"
#include "reader.h"
#include "output_file.h"
#include "jpeg2ppm.h"
#include "traitement_couleur.h"
#include "traitement_gris.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    IMAGE* image = read_jpeg(argv[1]);
    if (!image) {
        fprintf(stderr, "Erreur de lecture du fichier JPEG.\n");
        return 2;
    }
    char* output_filename = create_output_filename(argv[1],image->nb_components==3);
    IMAGE_D* image_d = init_image_d(image->Largeur, image->Hauteur, image->nb_components==3);
    if (image->nb_components == 1){
        traitement_grayscale(image, image_d);
    }
    else{
        traitement_color(image, image_d);
    }
    FILE* file = fopen(output_filename, "w");
    if(!file) {
        fprintf(stderr, "Erreur d'ouverture du fichier de sortie.\n");
        exit(1);
    }
    write_image_d(file, *image_d);
    fclose(file);
    free(output_filename);
    free_image(image);
    free_image_d(image_d);
    return 0;
}
                            