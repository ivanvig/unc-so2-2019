#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#include "server_utils.h"
#include "remote.h"

int main(int argc, char const *argv[])
{
	int pid;
	int sockfd, connect_sockfd;
	struct sockaddr_in connect_addr, serv_addr;
	socklen_t connect_addr_len = sizeof(connect_addr);

	sock_setup(&sockfd, &serv_addr, SOCK_STREAM, SV_PORT);

	if (listen(sockfd, N_MAX_CONN)) {
		perror("[!] ERROR: No se pudo poner el socket a la escucha\n");
		exit(EXIT_FAILURE);
	}

	printf("[*] Servidor listo a la escucha en puerto %d\n",
	       ntohs(serv_addr.sin_port));

	while (1) {
		connect_sockfd =
			accept(sockfd, (struct sockaddr *)&connect_addr,
			       &connect_addr_len);

		if (connect_sockfd < 0) {
			perror("[!] ERROR: La conexion con el satelite no se pudo establecer");
		}

		switch ((pid = fork())) {
		case 0:
			sv_cli(connect_sockfd, &connect_addr, connect_addr_len);
		case -1:
			perror("[!] ERROR: Un error ha ocurrido durante la creacion de la CLI");
			exit(EXIT_FAILURE);
		default:
			close(connect_sockfd);
			continue;
		}
	}

	return EXIT_SUCCESS;
}
