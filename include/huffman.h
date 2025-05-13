#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bitsmanip.h"


uint8_t read_bit(const uint8_t *data, size_t *bit_pos) ;
uint32_t read_bits(const uint8_t *data, size_t *bit_pos, int n);

char **generer_codes_huffman(uint8_t symbols[256], uint8_t taille[16], int n_symboles);

// -taille est un  tableau représentant le nombre de codes de chaque longueur (de 1 à 16 bits).

//ce tableau indique combien de codes ont 1 bit, combien en ont 2 bits jusqu’à 16.

//- symbole est un tableau de chaînes de caractères représentant les symboles que tu veux encoder en Huffman. 

// - n_symbole est le nombre de symbole qu'on a.
void free_huffman_table(char **table_huffman, int taille );

int decoder_huffman(const uint8_t *data, size_t *bit_pos, char **table_huffman, int n_symboles);

int magnitude(int taille, uint32_t valeur);

void decoder_bloc(
    const uint8_t *data, size_t *bit_pos,
    char **table_huffman_dc, uint8_t symboles_dc[256], int nbr_sym_dc,
    char **table_huffman_ac, uint8_t symboles_ac[256], int nbr_sym_ac,
    int *dc_precedent, int bloc[64]);