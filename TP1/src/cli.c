#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"
#include "local.h"

void start_cli(int sockfd_local)
{
	ssize_t nchr_read;
	char *line = NULL;
	char *op;
	char prompt[32];
	char name[MAX_NAME_SIZE];
	size_t lsize = 0;
	int sockfd;

	strncpy(prompt, DPROMPT, sizeof(prompt));

	while (1) {
		printf("%s", prompt);
		if ((nchr_read = getline(&line, &lsize, stdin)) < 0) {
			perror("[!] ERROR: CLI input error");
		}

		if (!nchr_read)
			continue;

		line[nchr_read - 1] = '\0';
		op = strtok(line, " ");

		if (!strcmp(op, "list")) {
			cli_list_connections(sockfd_local);
		} else if (!strcmp(op, "connect")) {
			strncpy(name, strtok(NULL, " "),
				min(MAX_NAME_SIZE, lsize));

			cli_remote_connect(name, sockfd_local, &sockfd, prompt,
					   sizeof(prompt));
		} else {
			write(sockfd, "kawabonga\n", 10);
		};
	}
}

void cli_list_connections(int sockfd)
{
	char msg = SV_LIST;

	if (write(sockfd, &msg, 1) != 1) {
		perror("[!] Error de comunicacion con el servidor\n");
	}

	int n_entries;
	char name[MAX_NAME_SIZE];
	read(sockfd, &n_entries, sizeof(n_entries));

	for (int i = 0; i < n_entries; i++) {
		read(sockfd, name, MAX_NAME_SIZE);
		printf("\t+%s\n", name);
	}
}

void cli_remote_connect(char *name, int sockfd_local, int *sockfd, char *prompt,
			size_t prompt_size)
{
	char msg = SV_CONNECT;

	if (write(sockfd_local, &msg, 1) != 1) {
		perror("[!] Error de comunicacion con el servidor\n");
	}

	if (write(sockfd_local, name, MAX_NAME_SIZE) != MAX_NAME_SIZE) {
		perror("[!] ERROR: No se pudo conectar con el cliente remoto\n");
	}

	if (read_sockfd(sockfd_local, sockfd) < 0) {
		perror("[!] ERROR: Un problema ha ocurrido durante la recepcion del socket externo\n");
		return;
	}

	strncpy(prompt, name, prompt_size);
	strncat(prompt, DPROMPT, prompt_size - strlen(prompt) - 1);
}
