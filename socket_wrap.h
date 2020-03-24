#ifndef SOCKET_WRAP_H
#define SOCKET_WRAP_H

#define SERV_PORT	9877
#define LISTENQ		1024


#include <sys/types.h>
#include <sys/socket.h>

int Socket(int family,int type,int protocol);
int Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
int Bind(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
int Close(int sockfd);

int create_listenfd();

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t Readn(int fd, void *ptr, size_t nbytes);
ssize_t	writen(int fd, const void *vptr, size_t n);
void Writen(int fd, void *ptr, size_t nbytes);
ssize_t readline(int fd, void *vptr, size_t maxlen);
ssize_t Readline(int fd, void *ptr, size_t maxlen);

#endif