#include "lz77.h"

int find_last_occurrence(const unsigned char *data, size_t data_len, const unsigned char *pattern, size_t pattern_len)
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

tuple_t get_tuple(char *search_buf, int search_size, char *look_ahead_buf, int look_ahead_size)
{
	// printf("? > search -> %.*s\n", search_size, search_buf);
	// printf("? > look   -> %.*s\n", look_ahead_size, look_ahead_buf);
	for (; look_ahead_size > 0; look_ahead_size--)
	{
		int index = find_last_occurrence(search_buf, search_size, look_ahead_buf, look_ahead_size);
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
tuple_t *lz77_compress(uint8_t *data, uint64_t data_len, int search_size, int look_ahead_size, size_t *tuples_len)
{
	size_t cur_alloc_size = sizeof(tuple_t) * 1024;
	tuple_t *tuples = malloc(cur_alloc_size);
	if (tuples == NULL)
		return NULL;

	int lab_offset = 0;
	int tuples_index = 0;
	while (lab_offset < data_len)
	{
		int sb_offset = lab_offset - search_size < 0 ? 0 : lab_offset - search_size;

		int sb_sz = search_size;
		if (sb_offset + sb_sz > lab_offset)
		{
			if (lab_offset < search_size)
			{
				sb_sz = lab_offset;
			}
		}

		int lab_sz = look_ahead_size;
		if (data + (lab_offset + lab_sz) > data + data_len)
		{
			lab_sz = (lab_offset + lab_sz) - data_len;
			lab_sz = look_ahead_size - lab_sz;
		}

		tuple_t t = get_tuple(data + sb_offset, sb_sz, data + lab_offset, lab_sz);
		t.next_value = *(data + lab_offset + t.size);

		tuples[tuples_index] = t;
		lab_offset += t.size + 1;
		tuples_index += 1;

		if (tuples_index > cur_alloc_size / sizeof(tuple_t))
		{
			size_t old_alloc_size = cur_alloc_size;
			cur_alloc_size += (sizeof(tuple_t) * 1024);
			tuple_t *new_alloc = malloc(cur_alloc_size);
			if (!new_alloc)
			{
				free(tuples);
				return NULL;
			}
			memcpy(new_alloc, tuples, old_alloc_size);
			free(tuples);
			tuples = new_alloc;
		}
	}
	*tuples_len = tuples_index;
	return tuples;
}
