#include <stdio.h>
#include <string.h>
#include <stdint.h>

int LOOK_AHEAD_BUFFER_SIZE = 3;
int SEARCH_BUFFER_SIZE = 8;

int OFFSET = 0;
int SIZE = 1;
void get_offset_and_size(char *plaintext, int sb_offset, int sb_sz, int lab_offset, int lab_sz, int offset_and_size[2]) {
	int cmp_sz = lab_sz < sb_sz ? lab_sz : sb_sz;
	printf("cmp_sz = %d\n", cmp_sz);

	// offset_and_size[1] = memcmp(plaintext + sb_offset, plaintext + lab_offset, cmp_sz);
	// printf("SIZE -> %d\n", offset_and_size[1]);
}

int main() {
	printf("[?] LAB -> Look Ahead Buffer\n");
	printf("[?] SB  -> SEARCH BUFFER\n\n");

	char *plaintext = "BASILE BAVE DANS SON BAVOIR";
	printf("[+] Original text:\n%s\n\n", plaintext);

	size_t plaintext_len = strlen(plaintext);
	for (int lab_offset = 0; lab_offset < plaintext_len; lab_offset++) {
		int sb_offset = lab_offset - SEARCH_BUFFER_SIZE < 0 ? 0 : lab_offset - SEARCH_BUFFER_SIZE;
		// printf("[+] sb_offset  = %d\n", sb_offset);

		// printf("sb_offset + SEARCH_BUFFER_SIZE = %d\n", sb_offset + SEARCH_BUFFER_SIZE);
		// printf("[+] lab_offset = %d\n", lab_offset);

		int sb_sz = SEARCH_BUFFER_SIZE;
		if (sb_offset + sb_sz > lab_offset) {
			if (lab_offset < SEARCH_BUFFER_SIZE) {
				sb_sz = lab_offset;
			}
		}

		
		int lab_sz = LOOK_AHEAD_BUFFER_SIZE;
		if (plaintext + (lab_offset + lab_sz) > plaintext + plaintext_len) {
			lab_sz = (lab_offset + lab_sz) - plaintext_len;
			lab_sz = LOOK_AHEAD_BUFFER_SIZE - lab_sz;
		}
		// printf(">>> SB : [%.*s]\n", sb_sz, plaintext + sb_offset);
		// printf(">>> LAB: [%.*s]\n\n", lab_sz,  plaintext + lab_offset);

		printf("> [%.*s]", sb_sz, plaintext + sb_offset);
		printf("[%.*s]\n", lab_sz,  plaintext + lab_offset);

		int offset_size[2];
		get_offset_and_size(plaintext, sb_offset, sb_sz, lab_offset, lab_sz, offset_size);

		printf("\n");
	}
}