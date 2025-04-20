#include "lz77.h"

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

void display_tuples(tuple_t *tuples, int size)
{
	for (int i = 0; i < size; i++)
	{
		printf("(%d, %d, %c)\n", tuples[i].offset, tuples[i].size, tuples[i].next_value);
	}
}
