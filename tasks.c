
#include "main.h"
#include "tasks.h"
#include "socket_wrap.h"
#include "error_wrap.h"

void str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[BUFFSIZE];

again:
	while((n = read(sockfd, buf, BUFFSIZE)) > 0)
		Writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");


	Close(sockfd);
}

int testTask(int i)
{
	pthread_t tid = pthread_self();
	printf("In thread: %d and i = %d.\n", tid, i);
}