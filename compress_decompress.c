#include "lz77.h"

static double compression_rate(unsigned int original_size, unsigned int compressed_size)
{
	if (original_size == 0)
		return 0.0; // avoid division by zero
	double rate = (1.0 - ((double)compressed_size / (double)original_size)) * 100.0;
	return rate;
}

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
		free(file_data);
		perror(error_msg);
		return 1;
	}

	printf("[+] Search buffer: %d bytes | Look ahead buffer: %d bytes\n", options->search_size, options->lookahead_size);
	printf("[+] Compressing '%s' into '%s'\n", options->input_file, options->output_file);

	uint64_t encoded_len;
	uint8_t *encoded = lz77_encode((uint8_t *)file_data, (uint64_t)file_len, options->search_size, options->lookahead_size, &encoded_len);

	if (!encoded)
	{
		perror("[-] lz77_encode failed");
		free(file_data);
		close(fd);
		return 1;
	}

	free(file_data);

	int ret = write(fd, &file_len, sizeof(size_t));
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file '%s' failed", options->output_file);
		perror(error_msg);
		free(encoded);
		return 1;
	}

	ret = write(fd, encoded, encoded_len);
	if (ret < 0)
	{
		snprintf(error_msg, sizeof(error_msg), "[-] Writing to file '%s' failed", options->output_file);
		perror(error_msg);
		free(encoded);
		return 1;
	}

	printf("[+] Compression done\n");
	double rate = compression_rate(file_len, encoded_len + sizeof(size_t));
	printf("[*] Compression rate: %.2f%% (original: %ld bytes, compressed: %ld bytes)\n", rate, file_len, encoded_len + sizeof(size_t));

	free(encoded);
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
	uint8_t *data = lz77_decode(file_data + sizeof(size_t), (file_len - sizeof(size_t)), data_len);

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

	printf("[+] Compression done\n");
	free(data);
	close(fd);
	return 0;
}
