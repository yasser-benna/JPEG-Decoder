#pragma once

#include <stdio.h>
#include <stdint.h>

typedef struct {
    FILE *fp;           // File pointer
    uint8_t buffer;     // Current byte being processed
    int bits_available; // Number of bits available in buffer
    long bytes_read;    // Total bytes read from file
    int is_eof;         // End of file flag
} BitStream;

// Initialize a bitstream
BitStream* bitstream_init(const char* filename);

// Close and free bitstream
void bitstream_close(BitStream* bs);

// Read n bits from the bitstream
uint32_t bitstream_read_bits(BitStream* bs, int n);

// Read a single bit
uint8_t bitstream_read_bit(BitStream* bs);

// Peek at next n bits without consuming them
uint32_t bitstream_peek_bits(BitStream* bs, int n);

