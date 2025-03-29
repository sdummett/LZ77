#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

int LOOK_AHEAD_BUFFER_SIZE = 3;
int SEARCH_BUFFER_SIZE = 8;

typedef struct s_tuple
{
	int offset;
	int size;
	uint8_t next_value;
} t_tuple;

int find_last_occurrence(const unsigned char *data, size_t data_len, const unsigned char *pattern, size_t pattern_len)
{
	if (pattern_len > data_len)
		return -1;

	for (int i = data_len - pattern_len; i >= 0; --i)
	{
		if (memcmp(data + i, pattern, pattern_len) == 0)
		{
			return i;
		}
	}

	return -1;
}

t_tuple get_tuple(char *search_buf, int search_size, char *look_ahead_buf, int look_ahead_size)
{
	// printf("? > search -> %.*s\n", search_size, search_buf);
	// printf("? > look   -> %.*s\n", look_ahead_size, look_ahead_buf);
	for (; look_ahead_size > 0; look_ahead_size--)
	{
		int index = find_last_occurrence(search_buf, search_size, look_ahead_buf, look_ahead_size);
		if (index >= 0)
		{
			// printf("look_ahead_size %d\n", look_ahead_size);
			// printf("Dernière occurrence à l'indice : %d\n", index);
			// printf(">> LZ77 offset: %d\n", search_size - index);
			// printf(">> LZ77 size  : %d\n", look_ahead_size);
			// printf("> %s\n", &search_buf[index]);
			t_tuple t = {
				.offset = search_size - index,
				.size = look_ahead_size,
				.next_value = 0,
			};
			return t;
		}
	}
	t_tuple t = {
		.offset = 0,
		.size = 0,
		.next_value = 0,
	};
	return t;
}

int main()
{
	// char *search_buf = "abcbcxdab";
	// // char *search_buf = "******";
	// char *look_ahead_buf = "abca";
	// t_tuple t = get_tuple(search_buf, strlen(search_buf), look_ahead_buf, strlen(look_ahead_buf));
	// printf("(%d, %d, ?)\n", t.offset, t.size);
	// return 42;

	// ------------------------------ //

	// size_t pattern_size = strlen(look_ahead_buf);
	// printf("[+] pattern_size: %ld\n", pattern_size);
	// size_t search_size = strlen(search_buf);
	// printf("[+] search_size: %ld\n", search_size);
	// // printf("[+] pattern_size: %d\n", pattern_size);
	// for (; pattern_size > 0; pattern_size--)
	// {
	// 	int index = find_last_occurrence(search_buf, strlen(search_buf), look_ahead_buf, pattern_size);
	// 	if (index >= 0)
	// 	{
	// 		printf("pattern_size %ld\n", pattern_size);
	// 		printf("Dernière occurrence à l'indice : %d\n", index);
	// 		printf(">> LZ77 offset: %ld\n", search_size - index);
	// 		printf(">> LZ77 size  : %ld\n", pattern_size);
	// 		printf("> %s\n", &search_buf[index]);
	// 		break;
	// 	}
	// 	else
	// 	{
	// 		printf("Motif non trouvé\n");
	// 	}
	// }

	// return 0;

	// ------------------------------ //

	char *plaintext = "BASILE BAVE DANS SON BAVOIR";
	printf("[+] Original text:\n%s\n\n", plaintext);

	size_t plaintext_len = strlen(plaintext);

	int lab_offset = 0;
	while (lab_offset < plaintext_len)
	// for (int lab_offset = 0; lab_offset < plaintext_len; lab_offset++)
	{
		int sb_offset = lab_offset - SEARCH_BUFFER_SIZE < 0 ? 0 : lab_offset - SEARCH_BUFFER_SIZE;
		// printf("[+] sb_offset  = %d\n", sb_offset);

		// printf("sb_offset + SEARCH_BUFFER_SIZE = %d\n", sb_offset + SEARCH_BUFFER_SIZE);
		// printf("[+] lab_offset = %d\n", lab_offset);

		int sb_sz = SEARCH_BUFFER_SIZE;
		if (sb_offset + sb_sz > lab_offset)
		{
			if (lab_offset < SEARCH_BUFFER_SIZE)
			{
				sb_sz = lab_offset;
			}
		}

		int lab_sz = LOOK_AHEAD_BUFFER_SIZE;
		if (plaintext + (lab_offset + lab_sz) > plaintext + plaintext_len)
		{
			lab_sz = (lab_offset + lab_sz) - plaintext_len;
			lab_sz = LOOK_AHEAD_BUFFER_SIZE - lab_sz;
		}
		// printf(">>> SB : [%.*s]\n", sb_sz, plaintext + sb_offset);
		// printf(">>> LAB: [%.*s]\n\n", lab_sz,  plaintext + lab_offset);

		// printf("> [%.*s]", sb_sz, plaintext + sb_offset);
		// printf("[%.*s]\n", lab_sz,  plaintext + lab_offset);

		// printf("> [");
		// fflush(stdout);
		// write(STDOUT_FILENO, plaintext + sb_offset, sb_sz);
		// printf("]");

		// printf("[");
		// fflush(stdout);
		// write(STDOUT_FILENO, plaintext + lab_offset, lab_sz);
		// printf("]\n");

		t_tuple t = get_tuple(plaintext + sb_offset, sb_sz, plaintext + lab_offset, lab_sz);
		t.next_value = *(plaintext + lab_offset + t.size);

		printf("(%d, %d, %c)\n", t.offset, t.size, t.next_value);
		lab_offset += t.size + 1;
	}
}
