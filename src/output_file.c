#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "output_file.h"
char* create_output_filename(const char* input_path,int flag) {
    size_t path_len = strlen(input_path);
    char* output_filename = malloc(path_len + 5);
    if (!output_filename) {
        free(output_filename);
        fprintf(stderr, "Memory allocation failed create_output_filename\n");
        exit(2);
    }
    strcpy(output_filename, input_path);
    char* last_dot = strrchr(output_filename, '.');
    char* last_slash = strrchr(output_filename, '/');
    if (last_dot != NULL && (last_slash == NULL || last_dot > last_slash)) {
        if(flag==1){
            strcpy(last_dot, ".ppm");
        }
        else{
            strcpy(last_dot, ".pgm");
        }
        return output_filename;
    }
    exit(2);
}