#ifndef __SERVER_UTILS_H_
#define __SERVER_UTILS_H_

#include <netinet/in.h>

#define N_MAX_CONN 10
#define DPROMPT ">> "
#define MAX_PROMPT_SIZE 64
#define min(i, j) (((i) < (j)) ? (i) : (j))

void listen_setup(int *sockfd, struct sockaddr_in *serv_addr);
void sv_cli(int sockfd, struct sockaddr_in *connect_addr,
	    socklen_t connect_addr_len);

#endif // __SERVER_UTILS_H_
