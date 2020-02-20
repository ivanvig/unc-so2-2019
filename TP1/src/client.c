#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "remote.h"

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
		perror("[!] ERROR: Problema con la IP del servidor.\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Conectando al servidor %s en puerto %d\n", SV_IP, SV_PORT);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
	    0) {
		perror("[!] ERROR: No se pudo establecer la conexion.\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Conexion establecida con %s en puerto %d\n",
	       inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

	while (1) {
		read(sockfd, &msg, 1);
		write(sockfd, &msg, 1);
	}
}
