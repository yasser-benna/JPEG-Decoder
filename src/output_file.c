#include <string.h>
#include <stdlib.h>
#include "output_file.h"
char* create_output_filename(const char* input_path) {
    const char* basename = input_path;
    const char* last_slash = strrchr(input_path, '/');
    if (last_slash != NULL) {
        basename = last_slash + 1;
    }
    size_t basename_len = strlen(basename);
    char* output_filename = malloc(basename_len + 5);
    if (!output_filename) {
        return NULL;
    }
    strcpy(output_filename, basename);
    char* last_dot = strrchr(output_filename, '.');
    if (last_dot != NULL) {
        *last_dot = '\0';
    }
    strcat(output_filename, ".ppm");
    return output_filename;
}