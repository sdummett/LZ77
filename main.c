#include "lz77.h"

void display_tuples(t_tuple *tuples, int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("(%d, %d, %c)\n", tuples[i].offset, tuples[i].size, tuples[i].next_value);
	}
}

int main(int ac, char *av[])
{
	char *plaintext = "BASILE BAVE DANS SON BAVOIR";
	// char *plaintext = av[1];
	printf("[+] Original text:\n%s\n\n", plaintext);

	size_t plaintext_len = strlen(plaintext);

	uint64_t tuples_len;

	int look_ahead_size = 3;
	int search_size = 8;
	t_tuple *tuples = lz77_compress((uint8_t *)plaintext, (uint64_t)plaintext_len, search_size, look_ahead_size, &tuples_len);

	// printf("tuples_len = %ld\n", tuples_len);
	// display_tuples(tuples, tuples_len);

	// remove the O_TRUNC
	// and add detailed message error
	int fd = open("data.lz77", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		printf("open failed\n");
		free(tuples);
		exit(44);
	}

	// display_tuples(tuples, tuples_index);

	if (write(fd, tuples, sizeof(t_tuple) * tuples_len) < 0)
	{
		printf("write failed\n");
		free(tuples);
		exit(45);
	}

	close(fd);
	return 0;
}
