#include "handlers.h"

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXBUFFSIZE 4096
#define MAX(x, y) (x > y ? x : y)

struct Options {
	char* host_address;
	int host_port;
	char* target_address;
	int target_port;
};

void connect_endpoints(int downstream, int upstream)
{
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(downstream, &fs);
	FD_SET(upstream, &fs);

	char* buffer = calloc(1, MAXBUFFSIZE);
	while (pselect(MAX(downstream, upstream) + 1, &fs, NULL, NULL, NULL, NULL)) {
		if (FD_ISSET(downstream, &fs)) {
			size_t buflen = recv(downstream, buffer, MAXBUFFSIZE, 0);
			if (buflen == 0)
				break;

			handle_upstream(upstream, buffer, buflen);
		}

		if (FD_ISSET(upstream, &fs)) {
			size_t buflen = recv(upstream, buffer, MAXBUFFSIZE, 0);
			if (buflen == 0)
				break;

			handle_downstream(downstream, buffer, buflen);
		}

		FD_ZERO(&fs);
		FD_SET(downstream, &fs);
		FD_SET(upstream, &fs);
	}
	free(buffer);
}

void usage()
{
	printf("Usage: proxy [<host_address>] <host_port> <target_address> "
	       "<target_port>\n");
}

bool parse_options(int argc, char* argv[], struct Options* options)
{
	memset(options, 0, sizeof(*options));

	if (argc == 4) {
		options->host_port = atoi(argv[1]);
		options->target_address = argv[2];
		options->target_port = atoi(argv[3]);
	}
	else if (argc == 5) {
		options->host_address = argv[1];
		options->host_port = atoi(argv[2]);
		options->target_address = argv[3];
		options->target_port = atoi(argv[4]);
	}
	else {
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	if (argc < 4) {
		usage();
		exit(0);
	}

	struct Options options;
	if (!parse_options(argc, argv, &options)) {
		printf("Error parsing arguments\n");
		usage();
		exit(0);
	}

	int server, downstream, upstream;

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == -1) {
		perror("socket()");
		exit(errno);
	}

	struct sockaddr_in server_hint;
	memset(&server_hint, 0, sizeof(server_hint));
	server_hint.sin_family = AF_INET;
	if (options.host_address)
		inet_pton(AF_INET, options.host_address, &server_hint.sin_addr.s_addr);
	else
		server_hint.sin_addr.s_addr = INADDR_ANY;
	server_hint.sin_port = htons(options.host_port);

	if (bind(server, (struct sockaddr*)(&server_hint), sizeof(server_hint)) ==
	    -1) {
		close(server);
		perror("bind()");
		exit(errno);
	}

	if (listen(server, 2) == -1) {
		close(server);
		perror("listen()");
		exit(errno);
	}

	while (1) {
		printf("Waiting new connection\n");
		downstream = accept(server, NULL, NULL);
		if (downstream == -1) {
			close(server);
			perror("accept()");
			exit(errno);
		}

		upstream = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (upstream == -1) {
			close(server);
			close(downstream);
			perror("socket()");
			exit(errno);
		}

		struct sockaddr_in target_hint;
		memset(&target_hint, 0, sizeof(target_hint));
		target_hint.sin_family = AF_INET;
		inet_pton(AF_INET, options.target_address, &target_hint.sin_addr.s_addr);
		target_hint.sin_port = htons(options.target_port);

		if (connect(upstream, (struct sockaddr*)(&target_hint),
		            sizeof(target_hint)) == -1) {
			close(server);
			close(downstream);
			close(upstream);
			perror("connect()");
		}

		printf("All sockets connected\n");
		connect_endpoints(downstream, upstream);

		close(downstream);
		close(upstream);
		printf("All sockets closed\n");
	}

	close(server);
}
