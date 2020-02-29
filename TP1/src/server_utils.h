#ifndef __SERVER_UTILS_H_
#define __SERVER_UTILS_H_

#include <netinet/in.h>
#include "remote.h"

#define N_MAX_CONN 10
#define DPROMPT ">> "
#define RCV_FNAME "image.jpg"
#define MAX_PROMPT_SIZE 64
#define SV_PASS "$1$/IRKS8y8$XWvK0etp8iNmFlIUDOaRb." //admin

void sock_setup(int *sockfd, struct sockaddr_in *serv_addr, int type, int port);
void sv_cli(int sockfd, struct sockaddr_in *connect_addr,
	    socklen_t connect_addr_len);

int sv_update(int sockfd, int updfd);
int sv_gettel(int sockfd, struct telemetria *tel);
int sv_scan(int sockfd);
int sv_ask_pass(const char *passwd);

#endif // __SERVER_UTILS_H_
