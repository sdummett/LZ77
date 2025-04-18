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

	if (options.mode == MODE_DECOMPRESS)
	{
		decompress(&options);
	}

	return 0;
}
