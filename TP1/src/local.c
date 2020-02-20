#include <sys/socket.h>
#include <string.h>
#include "local.h"

ssize_t write_sockfd(int sockfd_local, int sockfd_remote)
{
	struct msghdr msg = { 0 };
	struct cmsghdr *cmsg;

	char iobuf[1];
	struct iovec io = { .iov_base = iobuf, .iov_len = sizeof(iobuf) };
	union {
		char buf[CMSG_SPACE(sizeof(sockfd_remote))];
		struct cmsghdr align;
	} u;

	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(sockfd_remote));
	memcpy(CMSG_DATA(cmsg), &sockfd_remote, sizeof(sockfd_remote));

	return sendmsg(sockfd_local, &msg, 0);
}

ssize_t read_sockfd(int sockfd_local, int *sockfd_remote)
{
	ssize_t recv_size;

	struct msghdr msg = { 0 };
	struct cmsghdr *cmsg;

	char iobuf[1];
	struct iovec io = { .iov_base = iobuf, .iov_len = sizeof(iobuf) };
	union {
		char buf[CMSG_SPACE(sizeof(*sockfd_remote))];
		struct cmsghdr align;
	} u;

	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	msg.msg_control = u.buf;
	msg.msg_controllen = sizeof(u.buf);

	if ((recv_size = recvmsg(sockfd_local, &msg, 0) < 0)) {
		goto finish;
	}

	cmsg = CMSG_FIRSTHDR(&msg);

	if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(*sockfd_remote)) &&
	    cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
		memcpy(sockfd_remote, CMSG_DATA(cmsg), sizeof(*sockfd_remote));

	} else {
		*sockfd_remote = -1;
	}

finish:
	return recv_size;
}
