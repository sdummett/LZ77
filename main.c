#include "lz77.h"

void display_tuples(tuple_t *tuples, int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("(%d, %d, %c)\n", tuples[i].offset, tuples[i].size, tuples[i].next_value);
	}
}

void log_selected_options(program_options_t *options)
{
	if (options->mode == MODE_COMPRESS)
	{
		printf("[+] Compressing %s into %s\n", options->input_file, options->output_file);
		printf("[+] Using %d bytes for the search buffer and %d bytes for the look ahead buffer\n", options->search_size, options->lookahead_size);
	}
	else
	{
		printf("[+] Decompressing %s into %s\n", options->input_file, options->output_file);
	}
}

void compress(program_options_t *options)
{
	char *data = "BASILE BAVE DANS SON BAVOIR";
	printf("[+] Original text:\n%s\n\n", data);

	size_t data_len = strlen(data);

	uint64_t tuples_len;

	tuple_t *tuples = lz77_compress((uint8_t *)data, (uint64_t)data_len, options->search_size, options->lookahead_size, &tuples_len);

	// printf("tuples_len = %ld\n", tuples_len);
	// display_tuples(tuples, tuples_len);

	// remove the O_TRUNC
	// and add detailed message error
	int fd = open(options->output_file, O_CREAT | O_WRONLY, 0644);
	if (fd < 0)
	{
		fprintf(stderr, "[-] Opening file %s failed: %s\n", options->output_file, strerror(fd));
		free(tuples);
		exit(44);
	}

	// display_tuples(tuples, tuples_index);

	int ret = write(fd, tuples, sizeof(tuple_t) * tuples_len);
	if (ret < 0)
	{
		fprintf(stderr, "[-] Writing to file %s failed: %s\n", options->output_file, strerror(ret));
		free(tuples);
		exit(45);
	}

	close(fd);
}

int main(int ac, char *av[])
{
	program_options_t options;
	if (!parse_args(ac, av, &options))
		return 1;

	if (options.lookahead_size == 0)
		options.lookahead_size = DEFAULT_LOOKAHEAD_SIZE;
	if (options.search_size == 0)
		options.search_size = DEFAULT_SEARCH_SIZE;
	if (!options.output_file)
		options.output_file = DEFAULT_OUTPUT_FILE;

	log_selected_options(&options);

	if (options.mode == MODE_COMPRESS)
		compress(&options);

	return 0;
}
