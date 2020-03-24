
#include <sys/epoll.h>
#include "epoll_wrap.h"
#include "error_wrap.h"


int epoll_create_wrap(int max_events)
{
	int epollfd = epoll_create(max_events);
	if(epollfd < 0)
		err_sys("epollfd create error");
	return epollfd;
}

/**
 * regist epoll event
 *
 * @param epoll_fd	epoll handle
 * @param fd		socket handle
 * @param state		listen state
 */
void epoll_register(int epoll_fd, int fd, int state) {
	struct epoll_event event;

	event.events = state;
	event.data.fd = fd;
    
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
    	err_sys("epollfd regist error");
}

/**
 * cancel epoll event
 *
 * @param epoll_fd	epoll handle
 * @param fd		socket handle
 * @param state		listen state
 */
void epoll_cancel(int epoll_fd, int fd, int state) {
	struct epoll_event event;

	event.events = state;
	event.data.fd = fd;
	
	if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event) < 0)
		err_sys("epollfd cancel error");
}
