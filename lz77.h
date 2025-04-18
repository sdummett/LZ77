#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>

#define DEFAULT_LOOKAHEAD_SIZE 3
#define DEFAULT_SEARCH_SIZE 8
#define DEFAULT_OUTPUT_FILE "data.lz77"

// LZ77 file format
//        size_t          uint8[]
// | ORIGINAL_FILE_SIZE |  DATA   |

typedef struct __attribute__((__packed__)) tuple_s
{
	uint16_t offset;
	uint16_t size;
	uint8_t next_value;
} tuple_t;

// --- LZ77 Core Functions --- //
tuple_t *lz77_encode(uint8_t *data, uint64_t data_len, int search_size, int look_ahead_size, size_t *tuple_count);
uint8_t *lz77_decode(tuple_t *tuples, size_t tuple_count, size_t data_len);

typedef enum e_mode
{
	MODE_NONE,
	MODE_COMPRESS,
	MODE_DECOMPRESS
} mode__t;

typedef struct s_program_options
{
	mode__t mode;
	char *input_file;
	char *output_file;
	int lookahead_size;
	int search_size;
} program_options_t;

void compress(program_options_t *options);
void decompress(program_options_t *options);

// --- Helpers --- //
char *read_entire_file(const char *filename, size_t *out_size);
int parse_args(int argc, char *argv[], program_options_t *options);

#endif
