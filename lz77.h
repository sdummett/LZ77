// lz77.h — Public interface for a tiny LZ77 codec
//
// The goal is to expose a simple, dependency‑free C API that can be used from
// any C (or C++) project.
//
// All sizes are expressed in bytes unless stated otherwise.

#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>

/* Ensure ‘lookahead_size’ never exceeds 128.
 *   - The length field is packed into 7 bits of the flag byte
 *     -> maximum real length = 2^7 − 1 + 1 = 128. */

/* Ensure ‘search_size’ never exceeds 65 535.
 *   - The match offset is stored in a 16-bit uint16_t
 *     -> maximum backward distance = 65 535 bytes. */
#define MAX_LOOKAHEAD_SIZE 128
#define MAX_SEARCH_SIZE 65535

/* Default sliding‑window sizes.
 * You are free to tweak those at run‑time via the program‑options structure.
 */
#define DEFAULT_LOOKAHEAD_SIZE 32 /* bytes in front of the cursor  */
#define DEFAULT_SEARCH_SIZE 4096  /* bytes behind the cursor       */
#define DEFAULT_OUTPUT_FILE "data.lz77"

/* Compressed stream layout:
 * ┌───────────────────────┬──────────────────┐
 * │ uint64_t original_len │ encoded payload  │
 * └───────────────────────┴──────────────────┘
 */

enum
{
	TYPE_LITERAL = 0,
	TYPE_MATCH = 1
}; /* Stored in the flag byte */

/* Describes a back‑reference found by the matcher. */
typedef struct
{
	uint16_t offset; /* distance to the source substring (1‑based) */
	uint16_t size;	 /* match length                               */
} match_t;

/* ── Core API ─────────────────────────────────────────────────────────────── */
uint8_t *lz77_encode(uint8_t *input,
					 uint64_t input_len,
					 int search_size,
					 int lookahead_size,
					 size_t *encoded_len_out);

uint8_t *lz77_decode(uint8_t *encoded,
					 size_t encoded_len,
					 size_t original_len);

/* ── CLI plumbing ─────────────────────────────────────────────────────────── */
typedef enum
{
	MODE_NONE,
	MODE_COMPRESS,
	MODE_DECOMPRESS
} mode__t;

typedef struct
{
	mode__t mode;		/* operation selected by the user      */
	char *input_file;	/* path of the source file              */
	char *output_file;	/* where to write the result            */
	int lookahead_size; /* custom window sizes (or defaults)    */
	int search_size;
} program_options_t;

int compress(program_options_t *opt);
int decompress(program_options_t *opt);

/* ── Helpers ──────────────────────────────────────────────────────────────── */
char *read_entire_file(const char *filename, size_t *out_size);
int parse_args(int argc, char *argv[], program_options_t *opt);

#endif /* LZ77_H */
