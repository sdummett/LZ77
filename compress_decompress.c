#include "lz77.h"

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
	tuple_t *tuples = lz77_encode((uint8_t *)file_data, (uint64_t)file_len, options->search_size, options->lookahead_size, &tuples_len);

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
	uint8_t *data = lz77_decode((tuple_t *)(file_data + sizeof(size_t)), (file_len - sizeof(size_t)) / sizeof(tuple_t), data_len);

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
