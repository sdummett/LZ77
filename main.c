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

char *read_entire_file(const char *filename, size_t *out_size)
{
	char error_msg[256];
	FILE *fp = fopen(filename, "rb"); // Open in binary mode
	if (!fp)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Opening file %s failed", filename);
		perror(error_msg);
		return NULL;
	}

	// Seek to end to find size
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Seeking file %s failed", filename);
		perror(error_msg);
		fclose(fp);
		return NULL;
	}

	long size = ftell(fp);
	if (size < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Telling file %s failed", filename);
		perror(error_msg);
		fclose(fp);
		return NULL;
	}

	rewind(fp); // Go back to start of file

	char *buffer = malloc(size);
	if (!buffer)
	{
		perror("[-] Allocating memory failed");
		fclose(fp);
		return NULL;
	}

	size_t read_size = fread(buffer, 1, size, fp);
	if (read_size != (size_t)size)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Reading file %s failed", filename);
		perror(error_msg);
		free(buffer);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	if (out_size)
		*out_size = size;

	return buffer;
}

void compress(program_options_t *options)
{
	char error_msg[256];

	size_t file_len;
	uint8_t *file_data = read_entire_file(options->input_file, &file_len);
	if (!file_data)
	{
		exit(42);
	}

	int fd = open(options->output_file, O_CREAT | O_WRONLY | O_EXCL, 0644);
	if (fd < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Opening file %s failed", options->output_file);
		perror(error_msg);
		exit(44);
	}

	uint64_t tuples_len;
	tuple_t *tuples = lz77_compress((uint8_t *)file_data, (uint64_t)file_len, options->search_size, options->lookahead_size, &tuples_len);

	if (!tuples)
	{
		perror("[-] LZ77_compress core function failed");
		free(tuples);
		close(fd);
		exit(44);
	}

	free(file_data);

	// display_tuples(tuples, tuples_len);

	int ret = write(fd, &file_len, sizeof(size_t));
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file %s failed", options->output_file);
		perror(error_msg);
		free(tuples);
		exit(45);
	}

	ret = write(fd, tuples, sizeof(tuple_t) * tuples_len);
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file %s failed", options->output_file);
		perror(error_msg);
		free(tuples);
		exit(45);
	}

	close(fd);
}

void decompress(program_options_t *options)
{
	char error_msg[256];

	size_t file_len;
	uint8_t *file_data = read_entire_file(options->input_file, &file_len);
	if (!file_data)
	{
		exit(42);
	}

	int fd = open(options->output_file, O_CREAT | O_WRONLY | O_EXCL, 0644);
	if (fd < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Opening file %s failed", options->output_file);
		perror(error_msg);
		exit(44);
	}

	size_t data_len = *(size_t *)file_data;
	uint8_t *data = lz77_decompress((tuple_t *)(file_data + sizeof(size_t)), (file_len - sizeof(size_t)) / sizeof(tuple_t), data_len);

	// free(file_data);

	int ret = write(fd, data, data_len);
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file %s failed", options->output_file);
		perror(error_msg);
		// free(data);
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

	if (options.mode == MODE_DECOMPRESS)
	{
		decompress(&options);
	}

	return 0;
}
