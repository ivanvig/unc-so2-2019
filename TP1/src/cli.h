#ifndef __CLI_H_
#define __CLI_H_

#include <stddef.h>

#define DPROMPT ">> "
#define min(i, j) (((i) < (j)) ? (i) : (j))

void start_cli(int sockfd_local);
void cli_list_connections(int sockfd);
void cli_remote_connect(char *name, int sockfd_local, int *sockfd, char *prompt,
			size_t prompt_size);

#endif // __CLI_H_
