#include "lz77.h"

int main(int ac, char *av[])
{
	program_options_t options;
	if (!parse_args(ac, av, &options))
		return 1;

	if (options.mode == MODE_COMPRESS)
		return compress(&options);
	else if (options.mode == MODE_DECOMPRESS)
		return decompress(&options);

	return 0;
}
