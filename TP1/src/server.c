#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char const *argv[])
{
	int sv[2];
	int pid;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) < 0) {
		perror("[!] ERROR: No se pudo crear socket local para CLI");
		exit(EXIT_FAILURE);
	}

	switch ((pid = fork())) {
	case 0:
		close(sv[1]);
		start_remote_conection(sv[0]);
		break;
	case -1:
		perror("[!] ERROR: Un error ha ocurrido durante la creacion de la CLI");
		exit(EXIT_FAILURE);
	default:
		close(sv[0]);
		start_cli(sv[1]);
		break;
	}

	return EXIT_SUCCESS;
}
