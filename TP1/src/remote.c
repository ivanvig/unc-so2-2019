#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include "remote.h"

int safe_send(int sockfd, const void *bufptr, size_t bufsize, int flags)
{
	size_t leftbytes = bufsize;
	uint8_t *current_bufptr = (uint8_t *)bufptr;
	while (leftbytes > 0) {
		/* ssize_t sendbytes = */
		/* 	send(sockfd, current_bufptr, leftbytes, flags); */
		/* if (sendbytes < 0) { */
		/* 	perror("[!] Error al enviar"); */
		/* 	return -1; */
		/* } */
		/* leftbytes -= sendbytes; */
		/* current_bufptr += sendbytes; */
		ssize_t sendbytes =
			send(sockfd, current_bufptr, MSS, flags);
		if (sendbytes < 0) {
			perror("[!] Error al enviar");
			return -1;
		}
		leftbytes -= sendbytes;
		current_bufptr += sendbytes;
	}
	return 0;
}

int safe_recv(int sockfd, const void *bufptr, size_t bufsize, int flags)
{
	size_t leftbytes = bufsize;
	uint8_t *current_bufptr = (uint8_t *)bufptr;
	while (leftbytes > 0) {
		ssize_t recvbytes =
			recv(sockfd, current_bufptr, leftbytes, flags);
		if (recvbytes < 0) {
			perror("[!] Error al leer");
			return -1;
		}
		leftbytes -= recvbytes;
		current_bufptr += recvbytes;
	}
	return 0;
}
