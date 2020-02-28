#ifndef __CLIENT_UTILS_H_
#define __CLIENT_UTILS_H_
#include "remote.h"

#define min(i, j) (((i) < (j)) ? (i) : (j))
#define SOFT_VER "1.0"
#define FPATH                                                                  \
	"/home/ivan/Projects/Facultad/unc-so2-2019/TP1/misc/20190861730_GOES16-ABI-FD-GEOCOLOR-10848x10848.jpg"
#define CLIENT_FPORT 6666

#define APP_SENDBUF 4 * MSS
#define KRNL_SENDBUF (4 / 3) * BWD_PROD + 1024

int gettel(struct telemetria *tel);
int send_scan(int sockfd, int imgfd);

#endif // __CLIENT_UTILS_H_
