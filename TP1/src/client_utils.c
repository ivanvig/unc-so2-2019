#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "client_utils.h"
#include "remote.h"

int gettel(struct telemetria *tel)
{
	struct sysinfo info;

	if (gethostname(tel->id, sizeof(tel->id))) {
		return -1;
	}

	if (sysinfo(&info)) {
		return -2;
	}

	tel->uptime = info.uptime;
	tel->load = info.loads[1];
	tel->ram_usage = (info.freeram * info.mem_unit) / (1 << 20);

	strncpy(tel->ver, SOFT_VER, sizeof(tel->ver));

	return 0;
}

int update(int sockfd)
{
	off_t updsize;
	void *updbuf;
	int updfd;
	ssize_t leftbytes;
	uint8_t *bufptr;

	if (read(sockfd, &updsize, sizeof(updsize)) != sizeof(updsize)) {
		perror("[!] Error al recibir tamaño de archivo");
		return -1;
	}

	if ((updbuf = malloc(updsize)) == NULL) {
		perror("[!] Error reservando memoria para update");
		return -2;
	}

	if ((updfd = open(RCV_UPD_FNAME, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRWXU)) < 0) {
		perror("[!] Error al crear archivo");
		free(updbuf);
		return -3;
	}

	uint8_t msg = SAT_OK;
	write(sockfd, &msg, 1);

	leftbytes = updsize;
	bufptr = updbuf;
	while (leftbytes > 0) {
		ssize_t recvbytes = recv(sockfd, bufptr, leftbytes, 0);
		if (recvbytes < 0) {
			perror("[!] Error recibiendo archivo");
			free(updbuf);
			return -4;
		}
		leftbytes -= recvbytes;
		bufptr += recvbytes;
	}
	write(updfd, updbuf, updsize - leftbytes);
	close(updfd);

	printf("[*] Update recibido\n");

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	if (execlp("./" UPD_SCRIPT, UPD_SCRIPT, (char *)NULL) < 0) {
		perror("Error ejecutando nueva version");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int send_scan(int sockfd, int imgfd)
{
	struct stat statbuf;
	void *imgbuf;
	int leftbytes;
	uint8_t *bufptr;

	if (fstat(imgfd, &statbuf)) {
		perror("[!] Error obteniendo tamaño del archivo");
		return -1;
	}

	if ((imgbuf = malloc(statbuf.st_size)) == NULL) {
		perror("[!] Error reservando memoria para archivo");
		return -2;
	}
	printf("[*] Leyendo archivo\n");
	if ((read(imgfd, imgbuf, statbuf.st_size)) != statbuf.st_size) {
		perror("[!] Error leyendo archivo");
		return -3;
	}

	if (write(sockfd, &statbuf.st_size, sizeof(statbuf.st_size)) <
	    sizeof(statbuf.st_size)) {
		perror("[!] Error enviando tamaño de archivo");
		free(imgbuf);
		return -4;
	}

	printf("[*] Enviando archivo\n");

	leftbytes = statbuf.st_size;
	bufptr = (uint8_t *)imgbuf;
	while (leftbytes > 0) {
		ssize_t sendbytes = send(sockfd, bufptr, leftbytes, 0);
		if (sendbytes < 0) {
			perror("[!] Error enviando archivo");
			free(imgbuf);
			return -5;
		}
		leftbytes -= sendbytes;
		bufptr += sendbytes;
	}

	if (leftbytes != 0) {
		printf("[!] ERROR: Error al enviar la imagen\n");
		free(imgbuf);
		return -6;
	} else {
		printf("[*] Archivo enviado\n");
		free(imgbuf);
		return 0;
	}
}
