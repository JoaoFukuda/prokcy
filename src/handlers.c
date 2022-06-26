#include "handlers.h"

#include "binio.h"

#include <stdio.h>
#include <sys/socket.h>

void handle_upstream(int upstream, char* buffer, unsigned long int buflen)
{
	printf("downstream -> upstream [%ld]\n", buflen);
	hexdump(buffer, buflen);
	printf("\n");

	send(upstream, buffer, buflen, 0);
}

void handle_downstream(int downstream, char* buffer, unsigned long int buflen)
{
	printf("upstream -> downstream [%ld]\n", buflen);
	hexdump(buffer, buflen);
	printf("\n");

	send(downstream, buffer, buflen, 0);
}
