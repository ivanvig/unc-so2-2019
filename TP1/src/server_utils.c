#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "server_utils.h"
#include "local.h"
#include "remote.h"

int sv_list_connections(int sockfd_local, struct connect_descriptor *conn_table,
			int nentries)
{
	int n_valid_entries = 0;
	for (int i = 0; i < nentries; i++) {
		if (conn_table[i].valid) {
			n_valid_entries++;
		}
	}

	if (write(sockfd_local, &n_valid_entries, sizeof(n_valid_entries)) <
	    0) {
		return -1;
	}

	for (int i = 0; i < N_MAX_CONN; i++) {
		if (conn_table[i].valid) {
			if (write(sockfd_local, &conn_table[i].name,
				  MAX_NAME_SIZE) < 0) {
				return -1;
			}
		}
	}

	return 0;
}

int get_conn_fd(char *name, struct connect_descriptor *conn_table)
{
	for (int i = 0; i < N_MAX_CONN; i++) {
		if (conn_table[i].valid && !strcmp(name, conn_table[i].name)) {
			return conn_table[i].sockfd;
		}
	}
	return -1;
}

void listen_setup(int *sockfd, struct sockaddr_in *serv_addr)
{
	if (!(*sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("[!] ERROR: El socket no pudo ser creado.\n");
		exit(EXIT_FAILURE);
	}

	memset(serv_addr, 0, sizeof(*serv_addr));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = INADDR_ANY;
	serv_addr->sin_port = htons(SERV_PORT);

	if (bind(*sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr))) {
		perror("[!] ERROR: No se pudo hacer el bind del socket");
		exit(EXIT_FAILURE);
	}

	if (listen(*sockfd, N_MAX_CONN)) {
		perror("[!] ERROR: No se pudo poner el socket a la escucha");
		exit(EXIT_FAILURE);
	}

	printf("[*] Servidor listo a la escucha en puerto %d\n",
	       ntohs(serv_addr->sin_port));
}

int load_new_connection(char *name, int sockfd,
			struct connect_descriptor *connection_table,
			int nentries)
{
	for (int i = 0; i < nentries; i++) {
		if (!connection_table[i].valid) {
			strncpy(connection_table[i].name, name,
				sizeof(connection_table[i].name));
			connection_table[i].sockfd = sockfd;
			connection_table[i].valid = 1;
			return 0;
		}
	}
	return -1;
}

void start_server(int sockfd_local)
{
	int sockfd, connect_sockfd;
	struct sockaddr_in connect_addr, serv_addr;
	socklen_t connect_addr_len = sizeof(connect_addr);

	struct connect_descriptor *connection_table;

	struct pollfd fds[N_MAX_CONN + 2];
	int ret, nfd;

	char msg;
	struct telemetria tel;

	if ((connection_table = (struct connect_descriptor *)calloc(
		     N_MAX_CONN, sizeof(struct connect_descriptor)))) {
		perror("[!] ERROR: No se pudo allocar memoria para tabla de conecciones");
		exit(EXIT_FAILURE);
	}

	listen_setup(&sockfd, &serv_addr);

	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	fds[1].fd = sockfd_local;
	fds[1].events = POLLIN;

	nfd = 2;

	while (1) {
		if ((ret = poll(fds, nfd, -1)) <= 0) {
			perror("[!] ERROR: Un error ocurrio haciendo polling");
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < nfd; i++) {
			if (fds[i].revents & POLLIN) {
				if (i == 0) {
					connection_handler()

						printf("[*] Nueva conexion del satelite %s registrada",
						       tel.id);

				} else if (i == 1) {
					command_handler(fds[i].fd);
				}
			} else if (fds[i].revents & POLLHUP) {
				printf("Algo se cerro che");
			}
		}
	}
	write_sockfd(sockfd_local, connect_sockfd);
	close(connect_sockfd);
}

int connection_handler(int sockfd, int *connect_sockfd,
		       struct sockaddr_in *connect_addr,
		       socklen_t *connect_addr_len,
		       struct connect_descriptor *connection_table)
{
	char msg;
	struct telemetria tel;

	if ((*connect_sockfd = accept(sockfd, (struct sockaddr *)connect_addr,
				      connect_addr_len)) < 0) {
		return -1;
		/* perror("[!] ERROR: La conexion con el satelite no se pudo establecer"); */
	}

	msg = SAT_GETTEL;
	if (write(*connect_sockfd, &msg, sizeof(msg)) < 0) {
		/* perror("[!] ERROR: Error al obtener datos del satelite"); */
		return -2;
	}

	if (read(*connect_sockfd, &tel, sizeof(tel)) < 0) {
		/* perror("[!] ERROR: Error al leer los datos del satelite"); */
		return -3;
	}

	//TODO: ojo con tiempo de vida en la tabla
	if (load_new_connection(tel.id, connect_sockfd, connection_table,
				N_MAX_CONN) < 0) {
		perror("[!] ERROR: No se pudo registrar la conexion");
		continue;
	}
}
