// ---------------------------------------------------------------------------
// lz77.c — Naïve but readable implementation of the LZ77 algorithm.
// ---------------------------------------------------------------------------

#include "lz77.h"

/* Scan the search buffer from the end backwards and return the index of the
 * *last* occurrence of `pattern`. Returns -1 when no match is found. */
static int lz77_find_last_match(const unsigned char *buffer,
								size_t buffer_len,
								const unsigned char *pattern,
								size_t pattern_len)
{
	if (pattern_len > buffer_len)
		return -1;

	for (int i = (int)buffer_len - (int)pattern_len; i >= 0; --i)
		if (memcmp(buffer + i, pattern, pattern_len) == 0)
			return i;

	return -1;
}

/* Try to extend the match as long as it remains profitable. A match is deemed
 * profitable when it saves at least one byte compared to storing the same
 * data as literals. */
static bool lz77_find_longest_match(char *search_buf,
									int search_size,
									char *lookahead_buf,
									int lookahead_size,
									match_t *match)
{
	for (; lookahead_size > 0; --lookahead_size)
	{
		int idx = lz77_find_last_match((unsigned char *)search_buf,
									   search_size,
									   (unsigned char *)lookahead_buf,
									   lookahead_size);
		if (idx >= 0)
		{
			int distance = search_size - idx;
			int match_len = lookahead_size;
			int token_cost = 3; /* flag + 2‑byte offset  */
			int gain = match_len - token_cost;
			if (gain <= 0)
				continue; /* not worth it, try a shorter match */

			match->offset = (uint16_t)distance;
			match->size = (uint16_t)match_len;
			return true;
		}
	}
	return false;
}

#define MAX_LIT 128 /* we flush literals after this many bytes */

/* Keep window sizes consistent while the cursor moves through the input. */
void lz77_update_window_lens(int *search_len,
							 const int *search_size,
							 int *search_offset,
							 int *lookahead_len,
							 const int *lookahead_offset,
							 const int *lookahead_size,
							 const uint8_t *input,
							 const uint64_t *input_len)
{
	*search_offset = *lookahead_offset < *search_size
						 ? 0
						 : *lookahead_offset - *search_size;

	*search_len = *search_size;
	if (*search_offset + *search_len > *lookahead_offset)
		*search_len = *lookahead_offset;

	*lookahead_len = *lookahead_size;
	if (input + (*lookahead_offset + *lookahead_len) > input + *input_len)
		*lookahead_len = (int)(*input_len - *lookahead_offset);
}

/* Convenience: print the 8 bits of a byte as ASCII ‘0’ and ‘1’. */
void print_bits(uint8_t byte)
{
	for (int i = 7; i >= 0; --i)
		putchar((byte >> i) & 1 ? '1' : '0');
	putchar('\n');
}

/* The flag byte packs a 1‑bit type tag followed by a 7‑bit length. */
void pack_flag_and_length(uint8_t *dst, uint8_t flag, uint8_t length)
{
	*dst = (uint8_t)((flag << 7) | (length & 0x7F));
}

void unpack_flag_and_length(const uint8_t *src, uint8_t *flag, uint8_t *length)
{
	*flag = (uint8_t)((*src >> 7) & 0x01);
	*length = (uint8_t)(*src & 0x7F);
}

/* Encode a run of literals into `encoded`, return the new write cursor. */
size_t lz77_encode_literals(uint8_t *input, size_t lit_start, uint16_t lit_len,
							uint8_t *encoded, size_t out_idx)
{
	uint8_t flag_byte;
	pack_flag_and_length(&flag_byte, TYPE_LITERAL, (uint8_t)(lit_len - 1));

	encoded[out_idx++] = flag_byte;
	memcpy(encoded + out_idx, input + lit_start, lit_len);
	return out_idx + lit_len;
}

/* Encode a match (flag + offset). The match length is stored in the flag byte. */
size_t lz77_encode_match(match_t *match, uint8_t *encoded, size_t out_idx)
{
	uint8_t flag_byte;
	pack_flag_and_length(&flag_byte, TYPE_MATCH, (uint8_t)(match->size - 1));

	encoded[out_idx++] = flag_byte;
	memcpy(encoded + out_idx, &match->offset, 2); /* little‑endian */
	return out_idx + 2;
}

/* Compress `input` and return a freshly allocated buffer containing the
 * encoded stream. The caller owns the resulting memory. */
/* The returned buf looks like
 * | original size (sizeof(size_t)) | compressed data |
 */
uint8_t *lz77_encode(uint8_t *input, uint64_t input_len,
					 int search_size, int lookahead_size,
					 size_t *encoded_len_out)
{
	size_t original_input_len = input_len;
	size_t capacity = 8192; /* grows in 8 kB chunks */
	uint8_t *encoded = malloc(capacity);
	if (!encoded)
		return NULL;

	int lookahead_offset = 0;
	size_t out_idx = 0;

	while (lookahead_offset < (int)input_len)
	{
		int search_offset, search_len, lookahead_len;
		lz77_update_window_lens(&search_len, &search_size,
								&search_offset,
								&lookahead_len, &lookahead_offset,
								&lookahead_size,
								input, &input_len);

		size_t lit_start = (size_t)lookahead_offset;
		bool match_found = false;
		match_t match;

		while (lookahead_offset < (int)input_len)
		{
			lz77_update_window_lens(&search_len, &search_size,
									&search_offset,
									&lookahead_len, &lookahead_offset,
									&lookahead_size,
									input, &input_len);

			match_found = lz77_find_longest_match((char *)input + search_offset,
												  search_len,
												  (char *)input + lookahead_offset,
												  lookahead_len,
												  &match);
			if (match_found)
			{
				lookahead_offset += match.size;
				break;
			}

			++lookahead_offset;
			if (lookahead_offset - (int)lit_start == MAX_LIT) /* time to flush */
				break;
		}

		uint16_t lit_len = (uint16_t)(lookahead_offset - (int)lit_start);

		if (match_found)
		{
			lit_len = (uint16_t)(lit_len - match.size); /* true literal count */
			if (lit_len)
			{
				if (out_idx + 1 + lit_len > capacity)
				{
					capacity += 8192;
					encoded = realloc(encoded, capacity);
					if (!encoded)
						return NULL;
				}
				out_idx = lz77_encode_literals(input, lit_start, lit_len, encoded, out_idx);
			}
			if (out_idx + 3 > capacity)
			{ /* flag + 2‑byte offset */
				capacity += 8192;
				encoded = realloc(encoded, capacity);
				if (!encoded)
					return NULL;
			}
			out_idx = lz77_encode_match(&match, encoded, out_idx);
		}
		else if (lit_len)
		{
			if (out_idx + 1 + lit_len > capacity)
			{
				capacity += 8192;
				encoded = realloc(encoded, capacity);
				if (!encoded)
					return NULL;
			}
			out_idx = lz77_encode_literals(input, lit_start, lit_len, encoded, out_idx);
		}
	}

	*encoded_len_out = out_idx + sizeof(size_t); /* let the caller know how many bytes we produced */
	if (*encoded_len_out > capacity)
	{
		capacity += sizeof(size_t);
		encoded = realloc(encoded, capacity);
	}
	// we add the original size at the beggining of the buffer
	memmove(encoded + sizeof(size_t), encoded, *encoded_len_out);
	memcpy(encoded, &original_input_len, sizeof(size_t));
	return encoded;
}

/* Decompress `encoded` into a freshly allocated buffer of size `original_len`. */
uint8_t *lz77_decode(uint8_t *encoded, size_t encoded_len, size_t original_len)
{
	uint8_t *output = malloc(original_len);
	if (!output)
		return NULL;

	size_t out_idx = 0;
	for (size_t enc_pos = 0; enc_pos < encoded_len;)
	{
		uint8_t flag, length;
		unpack_flag_and_length(encoded + enc_pos++, &flag, &length);
		++length; /* stored as length‑1 */

		if (flag == TYPE_LITERAL)
		{
			memcpy(output + out_idx, encoded + enc_pos, length);
			out_idx += length;
			enc_pos += length;
		}
		else
		{ /* flag == TYPE_MATCH */
			uint16_t offset;
			memcpy(&offset, encoded + enc_pos, 2);
			enc_pos += 2;
			memmove(output + out_idx, output + out_idx - offset, length);
			out_idx += length;
		}
	}
	return output;
}
