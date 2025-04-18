#include "lz77.h"

static int find_last_match(const unsigned char *data, size_t data_len, const unsigned char *pattern, size_t pattern_len)
{
	if (pattern_len > data_len)
		return -1;

	for (int i = data_len - pattern_len; i >= 0; --i)
	{
		if (memcmp(data + i, pattern, pattern_len) == 0)
		{
			return i;
		}
	}

	return -1;
}

static tuple_t lz77_find_longest_match(char *search_buf, int search_size, char *look_ahead_buf, int look_ahead_size)
{
	// printf("? > search -> %.*s\n", search_size, search_buf);
	// printf("? > look   -> %.*s\n", look_ahead_size, look_ahead_buf);
	for (; look_ahead_size > 0; look_ahead_size--)
	{
		int index = find_last_match(search_buf, search_size, look_ahead_buf, look_ahead_size);
		if (index >= 0)
		{
			// printf("look_ahead_size %d\n", look_ahead_size);
			// printf("Dernière occurrence à l'indice : %d\n", index);
			// printf(">> LZ77 offset: %d\n", search_size - index);
			// printf(">> LZ77 size  : %d\n", look_ahead_size);
			// printf("> %s\n", &search_buf[index]);
			tuple_t t = {
				.offset = search_size - index,
				.size = look_ahead_size,
				.next_value = 0,
			};
			return t;
		}
	}
	tuple_t t = {
		.offset = 0,
		.size = 0,
		.next_value = 0,
	};
	return t;
}

// described the required args (also in the readme)
tuple_t *lz77_encode(uint8_t *data, uint64_t data_len, int search_size, int look_ahead_size, size_t *tuple_count)
{
	size_t allocated_bytes = sizeof(tuple_t) * 1024;
	tuple_t *tuples = malloc(allocated_bytes);
	if (tuples == NULL)
		return NULL;

	int lookahead_offset = 0;
	int tuples_index = 0;
	while (lookahead_offset < data_len)
	{
		int search_offset = lookahead_offset - search_size < 0 ? 0 : lookahead_offset - search_size;

		int search_len = search_size;
		if (search_offset + search_len > lookahead_offset)
		{
			if (lookahead_offset < search_size)
			{
				search_len = lookahead_offset;
			}
		}

		int lookahead_len = look_ahead_size;
		if (data + (lookahead_offset + lookahead_len) > data + data_len)
		{
			lookahead_len = (lookahead_offset + lookahead_len) - data_len;
			lookahead_len = look_ahead_size - lookahead_len;
		}

		tuple_t t = lz77_find_longest_match(data + search_offset, search_len, data + lookahead_offset, lookahead_len);
		t.next_value = *(data + lookahead_offset + t.size);

		tuples[tuples_index] = t;
		lookahead_offset += t.size + 1;
		tuples_index += 1;

		if (tuples_index >= allocated_bytes / sizeof(tuple_t))
		{
			size_t old_allocated_bytes = allocated_bytes;
			allocated_bytes += (sizeof(tuple_t) * 1024);
			tuple_t *new_alloc = malloc(allocated_bytes);
			if (!new_alloc)
			{
				free(tuples);
				return NULL;
			}
			memcpy(new_alloc, tuples, old_allocated_bytes);
			free(tuples);
			tuples = new_alloc;
		}
	}
	*tuple_count = tuples_index;
	return tuples;
}

uint8_t *lz77_decode(tuple_t *tuples, size_t tuple_count, size_t data_len)
{
	uint8_t *data = malloc(data_len);
	if (data == NULL)
		return NULL;

	size_t data_index = 0;
	for (size_t i = 0; i < tuple_count; i++)
	{
		if (tuples[i].offset)
		{
			memmove(&data[data_index], &data[data_index - tuples[i].offset], tuples[i].size);
			data[data_index + tuples[i].size] = tuples[i].next_value;
		}
		if (tuples[i].offset == 0)
		{
			data[data_index] = tuples[i].next_value;
		}

		data_index += tuples[i].size + 1;
	}
	printf("\n");
	return data;
}
