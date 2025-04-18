#include "lz77.h"

uint8_t *lz77_decompress(tuple_t *tuples, size_t tuples_len, size_t data_len)
{
	uint8_t *data = malloc(data_len);
	if (data == NULL)
		return NULL;

	size_t data_index = 0;
	for (size_t i = 0; i < tuples_len; i++)
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
