#ifndef __SERVER_UTILS_H_
#define __SERVER_UTILS_H_

#include <netinet/in.h>
#include "remote.h"

#define SERV_PORT 6020
#define N_MAX_CONN 10

struct connect_descriptor {
	int sockfd;
	uint8_t valid;
	char name[MAX_NAME_SIZE];
};

void start_server(int sockfd_local);
int sv_list_connections(int sockfd_local, struct connect_descriptor *conn_table,
			int nentries);
int get_conn_fd(char *name, struct connect_descriptor *conn_table);
void listen_setup(int *sockfd, struct sockaddr_in *serv_addr);
int load_new_connection(char *name, int sockfd,
			struct connect_descriptor *connection_table,
			int nentries);

#endif // __SERVER_UTILS_H_
