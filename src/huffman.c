#include <stdio.h>  
#include <stdlib.h> 
#include <stdint.h>   
#include <string.h>  
#include "huffman.h"
#include "bitsmanip.h"

uint8_t read_bit(const uint8_t *data, size_t *bit_pos) {
    static size_t prev_byte_index = SIZE_MAX;
    size_t byte_index = *bit_pos / 8;
    size_t bit_offset = *bit_pos % 8;   
    if (byte_index != prev_byte_index && byte_index > 0 &&
        data[byte_index - 1] == 0xFF && data[byte_index] == 0x00) {
        byte_index++;
        *bit_pos += 8; 
    }

    if (byte_index >= SIZE_MAX) {
        prev_byte_index = byte_index;
    }

    uint8_t byte = data[byte_index];
    uint8_t bit = (byte >> (7 - bit_offset)) & 1;

    (*bit_pos)++;
    prev_byte_index = byte_index;
    return bit;
}

uint32_t read_bits(const uint8_t *data, size_t *bit_pos, int n) {
    uint32_t value = 0;
    for (int i = 0; i < n; i++) {
        value = (value << 1) | read_bit(data, bit_pos);
    }
    return value;
}


char **generer_codes_huffman(uint8_t symbols[256], uint8_t taille[16], int n_symboles) {
    int i =0;
    int j=0;
    int k= 0;   
    int code = 0;
    int longueur_code=1;
    int nbr_code;
    char **table_huffman = malloc(n_symboles * sizeof(char *));


    for (longueur_code = 1; longueur_code <= 16; longueur_code++) {
        nbr_code = taille[longueur_code - 1];
        for (j = 0; j < nbr_code; j++) {
            if (i >= n_symboles) {
                break;  
            }
            table_huffman[i] = malloc((longueur_code + 1) * sizeof(char));
            for (k = 0; k < longueur_code; k++) {
                table_huffman[i][k] = ((code >> (longueur_code - 1 - k)) & 1) + '0';
            }
            table_huffman[i][longueur_code] = '\0';
            printf("Symbole: %u, Code: %s\n", symbols[i], table_huffman[i]);
            i++;
            code++;
        }
        code <<= 1;
    }

    return table_huffman;
    
}

void free_huffman_table(char **table_huffman, int taille) {
    for (int i = 0; i < taille; i++) {
        free(table_huffman[i]); 
    } 
    free(table_huffman);
}




int decoder_huffman(const uint8_t *data, size_t *bit_pos, char **table_huffman, int n_symboles) {
    int compteur_de_bit = 0;
    char code_lu[17] = {0};  

    while (compteur_de_bit < 16) {
        uint8_t bit_courant = read_bit(data, bit_pos);
        code_lu[compteur_de_bit] = bit_courant + '0';
        code_lu[compteur_de_bit + 1] = '\0';
        compteur_de_bit++;
        for (int i = 0; i < n_symboles; i++) {
            if (strcmp(code_lu, table_huffman[i]) == 0) {
                return i;  
            }
        }
    }
    return -1; 
}

int magnitude(int taille, uint32_t valeur) {
    // ici on travaille avec la magnitude et donc le nombre 
    //avec 1 à gauche et que des 0 à droite est le plus petit entier positif
    uint32_t nbr_critique;
    // si la taille de valeur est 0, on retourne 0
    if (taille == 0){
        return 0;  
    }
    nbr_critique = 1 << (taille - 1); // nbr_critique est le plus petit entier positif 
    if (valeur < nbr_critique)
        return valeur - ((1 << taille) - 1);
    else 
        return valeur;
}


void decoder_bloc(
    const uint8_t *data, size_t *bit_pos,
    char **table_huffman_dc, uint8_t symboles_dc[256], int nbr_sym_dc,
    char **table_huffman_ac, uint8_t symboles_ac[256], int nbr_sym_ac,
    int *dc_precedent, int bloc[64]) {

    for (int i = 0; i <= 63; i++) {
        bloc[i] = 0;
    }
    // Lire symbole DC (taille)
    int indice_dc = decoder_huffman(data, bit_pos, table_huffman_dc, nbr_sym_dc);
    uint8_t taille_dc = symboles_dc[indice_dc];
    uint32_t bits_dc = read_bits(data, bit_pos, taille_dc);
    int valeur_dc = magnitude(taille_dc, bits_dc);
    bloc[0] = *dc_precedent + valeur_dc;
    *dc_precedent = bloc[0];

    int j = 1;
    while (j <= 63) {
        int indice_ac = decoder_huffman(data, bit_pos, table_huffman_ac, nbr_sym_ac);
        uint8_t symbole_ac = symboles_ac[indice_ac];
        uint8_t run = symbole_ac >> 4;
        uint8_t taille = symbole_ac & 0x0F;

        if (symbole_ac == 0x00) {
            break;
        }
        if (symbole_ac == 0xF0) {
            j += 15;
        } else {
            j += run;
        }
        if (j >= 64) {
            break;
        }
        uint32_t bits_ac = read_bits(data, bit_pos, taille);
        int valeur_ac = magnitude(taille, bits_ac);
        printf("AC[%d] = %d (run=%d, size=%d, bits=0x%x)\n", j-1, valeur_ac, run, taille, bits_ac);
        bloc[j] = valeur_ac;
        j++;
    }
}