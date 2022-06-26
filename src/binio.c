#include "binio.h"

#include <ctype.h>
#include <stdio.h>

const int bytes_per_line = 8;

void putline(char* buffer, int n_of_bytes)
{
	for (int i = 0; i < n_of_bytes; ++i) {
		printf("%02hhx ", buffer[i]);
	}

	for (int i = n_of_bytes; i != bytes_per_line; ++i)
		printf("   ");

	printf(" |");

	for (int i = 0; i < n_of_bytes; ++i) {
		char c = isprint(buffer[i]) ? buffer[i] : '.';
		if (c == '\t' || c == '\n' || c == '\r')
			c = ' ';

		printf("%c", c);
	}

	for (int i = n_of_bytes; i != bytes_per_line; ++i)
		printf(" ");

	printf("|\n");
}

void hexdump(char* buffer, unsigned long buffer_len)
{
	int left = buffer_len;
	char* cursor = buffer;
	while (left != 0) {
		int n_of_bytes = left < bytes_per_line ? left : bytes_per_line;

		putline(cursor, n_of_bytes);

		left -= n_of_bytes;
		cursor += n_of_bytes;
	}
}
