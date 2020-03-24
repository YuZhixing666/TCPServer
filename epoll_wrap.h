#ifndef EPOLL_WRAP_H
#define EPOLL_WRAP_H

int epoll_create_wrap(int max_events);
void epoll_register(int epoll_fd, int fd, int state);
void epoll_cancel(int epoll_fd, int fd, int state);

#endif