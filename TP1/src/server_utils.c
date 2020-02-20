#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "server_utils.h"
#include "remote.h"

void listen_setup(int *sockfd, struct sockaddr_in *serv_addr)
{
	if (!(*sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("[!] ERROR: El socket no pudo ser creado.\n");
		exit(EXIT_FAILURE);
	}

	memset(serv_addr, 0, sizeof(*serv_addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = INADDR_ANY;
	serv_addr->sin_port = htons(SV_PORT);

	if (bind(*sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr))) {
		perror("[!] ERROR: No se pudo hacer el bind del socket\n");
		exit(EXIT_FAILURE);
	}

	if (listen(*sockfd, N_MAX_CONN)) {
		perror("[!] ERROR: No se pudo poner el socket a la escucha\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Servidor listo a la escucha en puerto %d\n",
	       ntohs(serv_addr->sin_port));
}

void sv_cli(int sockfd, struct sockaddr_in *connect_addr,
	    socklen_t connect_addr_len)
{
	struct telemetria tel;
	uint8_t msg = SAT_GETTEL;
	char prompt[MAX_PROMPT_SIZE];

	char *line = NULL;
	size_t lsize = 0;
	ssize_t nchr_read;
	char *op;

	if (write(sockfd, &msg, 1) != 1) {
		perror("[!] ERROR: No se pudo pedir el ID del satelite\n");
		strncpy(tel.id, "UNKNOWN", sizeof(tel.id));
	} else {
		if (read(sockfd, &tel, sizeof(tel)) != sizeof(tel)) {
			perror("[!] ERROR: No se pudo recibir el ID del satelite\n");
			strncpy(tel.id, "UNKNOWN", sizeof(tel.id));
		}
	}

	strncpy(prompt, tel.id, sizeof(prompt));
	strncat(prompt, DPROMPT, MAX_PROMPT_SIZE - strlen(prompt) - 1);

	while (1) {
		printf("%s ", prompt);

		if ((nchr_read = getline(&line, &lsize, stdin)) < 0) {
			perror("[!] ERROR: CLI input error\n");
		} else if (!nchr_read) {
			continue;
		}

		line[nchr_read - 1] = '\0';
		op = strtok(line, " ");

		if (!strcmp(op, "update")) {
			msg = SAT_UPDATE;
			if (write(sockfd, &msg, 1) != 1) {
				perror("[!] ERROR: Intente nuevamente\n");
				continue;
			}

		} else if (!strcmp(op, "get")) {
			msg = SAT_GETTEL;
			if (write(sockfd, &msg, 1) != 1) {
				perror("[!] ERROR: Intente nuevamente\n");
				continue;
			}

		} else if (!strcmp(op, "scan")) {
			msg = SAT_SCAN;
			if (write(sockfd, &msg, 1) != 1) {
				perror("[!] ERROR: Intente nuevamente\n");
				continue;
			}
		} else {
			perror("[!] ERROR: Comando invalido\n");
			continue;
		};
		read(sockfd, &msg, 1);
		printf("%d\n\n", msg);
	}
}
