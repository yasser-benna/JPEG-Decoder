#include <stdlib.h>
#include <stdio.h>
#include "bitsmanip.h"



BitStream* bitstream_init(const char* filename) {
    BitStream* bs = (BitStream*)malloc(sizeof(BitStream));
    if (!bs) return NULL;
    
    bs->fp = fopen(filename, "rb");
    if (!bs->fp) {
        free(bs);
        return NULL;
    }
    
    bs->buffer = 0;
    bs->bits_available = 0;
    bs->bytes_read = 0;
    bs->is_eof = 0;
    
    return bs;
}

void bitstream_close(BitStream* bs) {
    if (bs) {
        if (bs->fp) fclose(bs->fp);
        free(bs);
    }
}

static void bitstream_fill_buffer(BitStream* bs) {
    if (bs->bits_available == 0 && !bs->is_eof) {
        int byte = fgetc(bs->fp);
        if (byte == EOF) {
            bs->is_eof = 1;

        } else {
            bs->buffer = (uint8_t)byte;
            bs->bits_available = 8;
            bs->bytes_read++;

        }
    }
}

uint8_t bitstream_read_bit(BitStream* bs) {
    bitstream_fill_buffer(bs);

    if (bs->bits_available == 0) {
        return 0; // EOF or error
    }
    
    // Get the most significant bit
    uint8_t bit = (bs->buffer >> 7) & 1;
    
    // Shift buffer left by 1
    bs->buffer <<= 1;
    bs->bits_available--;
    
    return bit;
}

uint32_t bitstream_read_bits(BitStream* bs, int n) {
    uint32_t result = 0;
    
    for (int i = 0; i < n; i++) {
        result = (result << 1) | bitstream_read_bit(bs);//8(0)and sets the ones
    }
    
    return result;
}

uint32_t bitstream_peek_bits(BitStream* bs, int n) {
    // Save current state
    uint8_t saved_buffer = bs->buffer;
    int saved_bits = bs->bits_available;
    long saved_pos = ftell(bs->fp);// or bs->bytes_read
    int saved_eof = bs->is_eof;
    
    // Read bits
    uint32_t result = bitstream_read_bits(bs, n);
    
    // Restore state
    bs->buffer = saved_buffer;
    bs->bits_available = saved_bits;
    fseek(bs->fp, saved_pos, SEEK_SET);
    bs->is_eof = saved_eof;
    
    return result;
}