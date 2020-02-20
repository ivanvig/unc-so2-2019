#ifndef __LOCAL_H_
#define __LOCAL_H_

#include <sys/types.h>

#define SV_LIST 0x1
#define SV_CONNECT 0x2

ssize_t write_sockfd(int sockfd_local, int sockfd_remote);
ssize_t read_sockfd(int sockfd_local, int *sockfd_remote);

#endif // __LOCAL_H_
