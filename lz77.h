#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// int LOOK_AHEAD_BUFFER_SIZE = 3;
// int SEARCH_BUFFER_SIZE = 8;

typedef struct __attribute__((__packed__)) s_tuple
{
	uint16_t offset;
	uint16_t size;
	uint8_t next_value;
} t_tuple;

t_tuple *lz77_compress(uint8_t *data, uint64_t data_len, int search_size, int look_ahead_size, uint64_t *tuples_len);

#endif
