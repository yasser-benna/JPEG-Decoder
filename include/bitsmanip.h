#pragma once

#include <stdio.h>
#include <stdint.h>

typedef struct {
    FILE *fp;           // Pointeur de fichier
    uint8_t buffer;     // Octet en cours de traitement
    int bits_available; // Nombre de bits disponibles dans le tampon
    long bytes_read;    // Nombre total d'octets lus à partir du fichier
    int is_eof;         // Indicateur de fin de fichier
} BitStream;

// Initialiser un flux de bits
BitStream* bitstream_init(const char* filename);

// Fermer et libérer le flux de bits
void bitstream_close(BitStream* bs);

// Lire n bits du flux de bits
uint32_t bitstream_read_bits(BitStream* bs, int n);

// Lire un seul bit
uint8_t bitstream_read_bit(BitStream* bs);

// Consulter les n prochains bits sans les consommer
uint32_t bitstream_peek_bits(BitStream* bs, int n);

// Obtenir fp
FILE*get_fp(BitStream bs);

// Obtenir le tampon
uint8_t get_buffer(BitStream bs);

// Obtenir les bits disponibles
int get_bits_available(BitStream bs);

// Obtenir le nombre total d'octets lus
long get_bytes_read(BitStream bs);

// Obtenir l'indicateur de fin de fichier
int get_is_eof(BitStream bs);


