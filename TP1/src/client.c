#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "remote.h"
#include "client_utils.h"

int main(int argc, char **argv)
{
	int sockfd;
	uint8_t msg;
	struct sockaddr_in serv_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("[!] ERROR: No se pudo crear socket\n");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SV_PORT);

	if (!inet_aton(SV_IP, &serv_addr.sin_addr)) {
		perror("[!] Error al configurar la IP del servidor");
		exit(EXIT_FAILURE);
	}

	int bufsize = OPT_SOCK_BUF + 512;
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &bufsize,
		       sizeof(bufsize)) < 0) {
		perror("[!] Error al configurar SO_SNDBUF");
		exit(EXIT_FAILURE);
	}

	printf("[*] Conectando al servidor %s en puerto %d\n", SV_IP, SV_PORT);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
	    0) {
		perror("[!] ERROR: No se pudo establecer la conexion.\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Conexion establecida\n");

	while (1) {
		struct telemetria tel;
		read(sockfd, &msg, 1);
		int fd;

		//TODO: checkear error de los write
		switch (msg) {
		case SAT_GETTEL:
			printf("[*] Comando 'get' recibido\n");
			if (!gettel(&tel)) {
				printf("[*] Comenzando envio de telemetria\n");
				msg = SAT_OK;
				write(sockfd, &msg, 1);

				int fsockfd;
				struct sockaddr_in servaddr;

				printf("[*] Creando socket UDP\n");
				if ((fsockfd = socket(AF_INET, SOCK_DGRAM, 0)) <
				    0) {
					perror("Error al crear socket UDP");
					exit(EXIT_FAILURE);
				}

				memset(&servaddr, 0, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(SV_UDPPORT);

				if (!inet_aton(SV_IP, &servaddr.sin_addr)) {
					perror("[!] Error al configurar la IP del servidor");
					exit(EXIT_FAILURE);
				}
				printf("[*] Enviando telemetria\n");
				sendto(fsockfd, &tel, sizeof(tel), 0,
				       (const struct sockaddr *)&servaddr,
				       sizeof(servaddr));
				shutdown(fsockfd, SHUT_WR);
				/* recv(fsockfd, NULL, 0, 0); */
				close(fsockfd);
			} else {
				perror("[!] Error obteniendo telemetria");
				msg = SAT_ERR;
				write(sockfd, &msg, 1);
			}
			break;
		case SAT_SCAN:
			printf("[*] Comando 'scan' recibido\n");
			if ((fd = open(FPATH, O_RDONLY)) < 0) {
				perror("[!] Error abriendo archivo");
				msg = SAT_ERR;
				write(sockfd, &msg, 1);
			} else {
				printf("[*] Enviando imagen\n");
				msg = SAT_OK;
				write(sockfd, &msg, 1);
				send_scan(sockfd, fd);
				close(fd);
			}
			break;
		case SAT_UPDATE:
			printf("[*] Comando 'update' recibido\n");
			msg = SAT_OK;
			write(sockfd, &msg, 1);
			if (update(sockfd) < 0) {
				printf("[!] Error al actualizar");
				msg = SAT_ERR;
				write(sockfd, &msg, 1);
			}
			break;
		default:
			perror("[!] Comando invalido");
			msg = SAT_INV;
			write(sockfd, &msg, 1);
			break;
		}
	}
}
