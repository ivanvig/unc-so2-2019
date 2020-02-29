#ifndef __CLIENT_UTILS_H_
#define __CLIENT_UTILS_H_
#include "remote.h"

#define SOFT_VER "1.0"
#define FPATH                                                                  \
	"/home/ivan/Projects/Facultad/unc-so2-2019/TP1/misc/20190861730_GOES16-ABI-FD-GEOCOLOR-10848x10848.jpg"
#define RCV_UPD_FNAME "firmware_update"
#define UPD_SCRIPT "update_binary.sh"
#define CLIENT_FPORT 6666

#define APP_SENDBUF 4 * MSS
#define KRNL_SENDBUF (4 / 3) * BWD_PROD + 1024

int gettel(struct telemetria *tel);
int send_scan(int sockfd, int imgfd);
int update(int sockfd);

#endif // __CLIENT_UTILS_H_
