
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include "main.h"
#include "socket_wrap.h"
#include "error_wrap.h"
#include "thread_pool.h"
#include "logger.h"
#include "tasks.h"
#include "epoll_wrap.h"


// Accept client connection when a EPOLLIN event come
void accept_client(int listenfd, int epollfd)
{
	//log_info("accept_client\n");
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen;

	clilen = sizeof(cliaddr);
	connfd = Accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

	//register the event
	epoll_register(epollfd, connfd, EPOLLIN | EPOLLET);		//ET mode is important here!!!
}


// Deal with the client request
void deal_request(int connfd, threadpool_t *poolAddr)
{
	//log_info("deal_request\n");
	threadpool_add_task(poolAddr, str_echo, connfd);
	//threadpool_add_task(poolAddr, testTask, 23);
}


int main(int argc, char **argv)
{
	int listenfd, epollfd, tempfd;
	int event_num, i;
	struct epoll_event events[MAX_EVENTS];
	threadpool_t pool;

	threadpool_init(&pool, THREAD_NUM);

	listenfd = create_listenfd();
	Listen(listenfd, LISTENQ);

	epollfd = epoll_create_wrap(FD_SIZE);
	epoll_register(epollfd, listenfd, EPOLLIN | EPOLLET);		//ET mode


	//event_timer_init();

	while(1)
	{
		//event_time_update();
		//event_expire_timers();

		event_num = epoll_wait(epollfd, events, MAX_EVENTS, 0);
		for(i = 0; i < event_num; ++i)
		{
			tempfd = events[i].data.fd;

			if((tempfd == listenfd) && (events[i].events == EPOLLIN))
				accept_client(listenfd, epollfd);
			else if(events[i].events == EPOLLIN)
			{
				deal_request(tempfd, &pool);
				epoll_cancel(epollfd, tempfd, EPOLLIN);
			}
			else if(events[i].events == EPOLLOUT)
				// too much data
				// todo
			{
				log_warn("Event: EPOLLOUT comes up");
				continue;
			}
		}
		usleep(EPOLL_WAIT_US);
	}

	pause();
}