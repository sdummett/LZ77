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

typedef struct __attribute__((__packed__)) tuple_s
{
	uint16_t offset;
	uint16_t size;
	uint8_t next_value;
} tuple_t;

typedef enum e_mode
{
	MODE_NONE,
	MODE_COMPRESS,
	MODE_DECOMPRESS
} mode_s;

typedef struct s_program_options
{
	mode_t mode;
	char *input_file;
	char *output_file;
	int lookahead_size;
	int search_size;
} program_options_t;

tuple_t *lz77_compress(uint8_t *data, uint64_t data_len, int search_size, int look_ahead_size, size_t *tuples_len);
int parse_args(int argc, char *argv[], program_options_t *options);

#endif
