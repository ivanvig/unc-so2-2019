#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <time.h>
#include "server_utils.h"
#include "remote.h"

void sock_setup(int *sockfd, struct sockaddr_in *serv_addr, int type, int port)
{
	if (!(*sockfd = socket(AF_INET, type, 0))) {
		perror("[!] ERROR: El socket no pudo ser creado.\n");
		exit(EXIT_FAILURE);
	}

	memset(serv_addr, 0, sizeof(*serv_addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = INADDR_ANY;
	serv_addr->sin_port = htons(port);

	if (bind(*sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr))) {
		perror("[!] ERROR: No se pudo hacer el bind del socket\n");
		exit(EXIT_FAILURE);
	}
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
		if (read(sockfd, &msg, 1) != 1 || msg != SAT_OK ||
		    read(sockfd, &tel, sizeof(tel)) != sizeof(tel)) {
			perror("[!] ERROR: No se pudo recibir el ID del satelite\n");
			strncpy(tel.id, "UNKNOWN", sizeof(tel.id));
		}
	}

	strncpy(prompt, tel.id, sizeof(prompt));
	strncat(prompt, DPROMPT, MAX_PROMPT_SIZE - strlen(prompt) - 1);

	while (1) {
		printf("%s ", prompt);

		//TODO: enter no funcionando
		if ((nchr_read = getline(&line, &lsize, stdin)) < 0) {
			perror("[!] ERROR: CLI input error\n");
		} else if (!nchr_read) {
			continue;
		}

		line[nchr_read - 1] = '\0';
		op = strtok(line, " ");

		if (!strcmp(op, "update")) {
			sv_update(sockfd);
		} else if (!strcmp(op, "get")) {
			sv_gettel(sockfd);
		} else if (!strcmp(op, "scan")) {
			sv_scan(sockfd);
		} else {
			perror("[!] ERROR: Comando invalido\n");
			continue;
		};
	}
}

int sv_update(int sockfd)
{
	uint8_t msg;
	msg = SAT_UPDATE;
	if (write(sockfd, &msg, 1) != 1) {
		perror("[!] ERROR: Intente nuevamente\n");
		return -1;
	}
	return 0;
}

int sv_gettel(int sockfd)
{
	uint8_t msg;
	struct telemetria tel;
	msg = SAT_GETTEL;
	if (write(sockfd, &msg, 1) != 1) {
		perror("[!] ERROR: Intente nuevamente\n");
		return -1;
	}
	if (read(sockfd, &msg, 1) != 1 || msg != SAT_OK ||
	    read(sockfd, &tel, sizeof(tel)) != sizeof(tel)) {
		perror("[!] ERROR: No se pudo recibir la telemetria\n");
		return -2;
	} else {
		printf("\t ID: %s\n "
		       "\t UPTIME [s]: %ld\n"
		       "\t VERSION: %s\n"
		       "\t CARGA DEL SISTEMA: %f\n"
		       "\t RAM LIBRE [MiB]: %lu\n",
		       tel.id, tel.uptime, tel.ver,
		       tel.load / (float)(1 << SI_LOAD_SHIFT), tel.ram_usage);
	}
	return 0;
}

int sv_scan(int sockfd)
{
	uint8_t msg;

	off_t imgsize;
	void *imgbuf;
	int imgfd;
	ssize_t leftbytes;
	uint8_t *bufptr;
	struct timespec begin, end;

	msg = SAT_SCAN;
	if (write(sockfd, &msg, 1) != 1) {
		perror("[!] ERROR: Intente nuevamente");
		return -1;
	}

	if (read(sockfd, &msg, 1) != 1 || msg != SAT_OK ||
	    read(sockfd, &imgsize, sizeof(imgsize)) != sizeof(imgsize)) {
		perror("[!] Error al recibir tamaño de archivo");
		return -2;
	}

	if ((imgbuf = malloc(imgsize)) == NULL) {
		perror("[!] Error reservando memoria para imagen");
		return -3;
	}

	if ((imgfd = open(RCV_FNAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) <
	    0) {
		perror("[!] Error al crear archivo");
		free(imgbuf);
		return -4;
	}

	int bufsize = OPT_SOCK_BUF + 2048;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &bufsize,
		       sizeof(bufsize)) < 0) {
		perror("[!] Error al configurar SO_RCVBUF");
		return -7;
	}

	leftbytes = imgsize;
	bufptr = imgbuf;
	clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
	while (leftbytes > 0) {
		ssize_t recvbytes = recv(sockfd, bufptr, leftbytes, 0);
		if (recvbytes < 0) {
			perror("[!] Error recibiendo archivo");
			free(imgbuf);
			return -5;
		}
		leftbytes -= recvbytes;
		bufptr += recvbytes;
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	write(imgfd, imgbuf, imgsize - leftbytes);
	close(imgfd);

	double time = (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
		      (end.tv_sec - begin.tv_sec);
	double recvsize = (imgsize - leftbytes) / 1048576.0;
	printf("[*] Recibidos %.2f MBytes en %.2f segundos [%.2f MBytes/s]\n",
	       recvsize, time, recvsize / time);

	if (leftbytes != 0) {
		perror("[!] Error al leer la imagen");
		free(imgbuf);
		return -6;
	}

	free(imgbuf);
	return 0;
}
