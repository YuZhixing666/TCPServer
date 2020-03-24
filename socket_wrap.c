/*
 * This file contains the wrapper functions for the socket
 */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "error_wrap.h"
#include "socket_wrap.h"

int Socket(int family,int type,int protocol)
{
	int n;
	if((n = socket(family, type, protocol)) < 0)
		err_sys("socket error");
	return (n);
}


int Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
	int n;
	if((n = connect(sockfd, servaddr, addrlen)) == -1)
		err_sys("connect error");
	return (n);
}

int Bind(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
	int n=1;
	if((n = bind(sockfd, servaddr, addrlen)) == -1)
		err_sys("bind error");
	return (n);
}

void Listen(int sockfd, int backlog)
{
	char *ptr;

	//support override the backlog with a environment variable
	if((ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if(listen(sockfd, backlog) < 0)
		err_sys("listen error");
}

int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
	int n;
	if((n = accept(sockfd, cliaddr, addrlen)) == -1)
		err_sys("accept error");
	return (n);
}

int Close(int sockfd)
{
	int n;
	if((n = close(sockfd)) == -1)
		err_sys("close error");
	return (n);
}


/* create a listen fd and bind to server */
int create_listenfd()
{
	int listenfd;
	struct sockaddr_in servaddr;
	int flags;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);


	//make it Non-Blocking
	flags = fcntl(listenfd, F_GETFL, 0);
	fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);

	//bind to the port
	Bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	return listenfd;
}



/* Read "n" bytes from a descriptor. */
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return -1;
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return (n - nleft);		/* return >= 0 */
}

ssize_t Readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
		err_sys("readn error");
	return n;
}

/* Write "n" bytes to a descriptor. */
ssize_t	writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}

/* PAINFULLY SLOW VERSION -- example only */
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
again:
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else {
			if (errno == EINTR)
				goto again;
			return(-1);		/* error, errno set by read() */
		}
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}
/* end readline */

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}