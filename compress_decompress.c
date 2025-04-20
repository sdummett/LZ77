#include "lz77.h"

int compress(program_options_t *options)
{
	char error_msg[256];

	size_t file_len;
	uint8_t *file_data = read_entire_file(options->input_file, &file_len);
	if (!file_data)
	{
		return 1;
	}

	int fd = open(options->output_file, O_CREAT | O_WRONLY | O_EXCL, 0644);
	if (fd < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Opening file '%s' failed", options->output_file);
		perror(error_msg);
		return 1;
	}

	printf("[+] Compressing '%s' into '%s'\n", options->input_file, options->output_file);
	printf("[+] Search buffer: %d bytes | Look ahead buffer: %d bytes\n", options->search_size, options->lookahead_size);

	uint64_t tuples_len;
	tuple_t *tuples = lz77_encode((uint8_t *)file_data, (uint64_t)file_len, options->search_size, options->lookahead_size, &tuples_len);

	if (!tuples)
	{
		perror("[-] lz77_encode failed");
		free(file_data);
		close(fd);
		return 1;
	}

	free(file_data);

	// display_tuples(tuples, tuples_len);

	int ret = write(fd, &file_len, sizeof(size_t));
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file '%s' failed", options->output_file);
		perror(error_msg);
		free(tuples);
		return 1;
	}

	ret = write(fd, tuples, sizeof(tuple_t) * tuples_len);
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file '%s' failed", options->output_file);
		perror(error_msg);
		free(tuples);
		return 1;
	}

	free(tuples);
	close(fd);
	return 0;
}

int decompress(program_options_t *options)
{
	char error_msg[256];

	size_t file_len;
	uint8_t *file_data = read_entire_file(options->input_file, &file_len);
	if (!file_data)
	{
		return 1;
	}

	int fd = open(options->output_file, O_CREAT | O_WRONLY | O_EXCL, 0644);
	if (fd < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Opening file '%s' failed", options->output_file);
		perror(error_msg);
		return 1;
	}

	printf("[+] Decompressing '%s' into '%s'\n", options->input_file, options->output_file);

	size_t data_len = *(size_t *)file_data;
	uint8_t *data = lz77_decode((tuple_t *)(file_data + sizeof(size_t)), (file_len - sizeof(size_t)) / sizeof(tuple_t), data_len);

	if (!data)
	{
		perror("[-] lz77_decode failed");
		free(file_data);
		close(fd);
		return 1;
	}

	free(file_data);

	int ret = write(fd, data, data_len);
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file '%s' failed", options->output_file);
		perror(error_msg);
		free(data);
		return 1;
	}

	free(data);
	close(fd);
	return 0;
}
