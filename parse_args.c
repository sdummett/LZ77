#include "lz77.h"

void print_help(const char *prog_name)
{
	printf("Usage:\n");
	printf("  %s -c <file> [options]\n", prog_name);
	printf("  %s -d <file> [options]\n\n", prog_name);

	printf("Required options (choose one):\n");
	printf("  -c, --compress <file>       Compress the specified input file\n");
	printf("  -d, --decompress <file>     Decompress the specified input file\n\n");

	printf("Optional parameters:\n");
	printf("  -l, --lookahead <size>      Set the lookahead buffer size (default: %d)\n", DEFAULT_LOOKAHEAD_SIZE);
	printf("  -s, --search <size>         Set the search buffer size (default: %d)\n", DEFAULT_SEARCH_SIZE);
	printf("  -o, --output <file>         Specify the output file name (default: data.lz77)\n");
	printf("      --help                  Display this help message\n\n");

	printf("Examples:\n");
	printf("  %s -c input.txt -o compressed.lz77 --lookahead 16 --search 64\n", prog_name);
	printf("  %s --decompress compressed.lz77 -o original.txt\n\n", prog_name);

	printf("Notes:\n");
	printf("  - If the -o option is not used, output will be written to 'data.lz77'.\n");
	printf("  - If lookahead or search sizes are not specified, defaults are used:\n");
	printf("      Lookahead: %d, Search: %d\n", DEFAULT_LOOKAHEAD_SIZE, DEFAULT_SEARCH_SIZE);
}

int parse_args(int argc, char *argv[], program_options_t *options)
{
	int opt;
	int option_index = 0;

	static struct option long_options[] = {
		{"compress", required_argument, 0, 'c'},
		{"decompress", required_argument, 0, 'd'},
		{"lookahead", required_argument, 0, 'l'},
		{"search", required_argument, 0, 's'},
		{"output", required_argument, 0, 'o'},
		{0, 0, 0, 0}};

	options->mode = MODE_NONE;
	options->input_file = NULL;
	options->output_file = NULL;
	options->lookahead_size = 0;
	options->search_size = 0;

	while ((opt = getopt_long(argc, argv, "c:d:l:s:o:", long_options, &option_index)) != -1)
	{
		switch (opt)
		{
		case 'c':
			if (options->mode != MODE_NONE)
			{
				fprintf(stderr, "Error: Cannot use both --compress and --decompress.\n");
				return 0;
			}
			options->mode = MODE_COMPRESS;
			options->input_file = optarg;
			break;
		case 'd':
			if (options->mode != MODE_NONE)
			{
				fprintf(stderr, "Error: Cannot use both --compress and --decompress.\n");
				return 0;
			}
			options->mode = MODE_DECOMPRESS;
			options->input_file = optarg;
			break;
		case 'l':
			options->lookahead_size = atoi(optarg);
			break;
		case 's':
			options->search_size = atoi(optarg);
			break;
		case 'o':
			options->output_file = optarg;
			break;
		default:
			return 0;
		}
	}

	if (options->mode == MODE_NONE || options->input_file == NULL)
	{
		print_help("lz77");
		return 0;
	}

	return 1;
}
