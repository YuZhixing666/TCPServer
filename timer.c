/*
 * Copyright (C) Chenyang Li
 * Copyright (C) Vino
 */
#include <stdlib.h>
#include <sys/time.h>
#include "timer.h"
#include "min_heap.h"
#include "logger.h"
#include "error_wrap.h"
#include "socket_wrap.h"

heap_t heap;

void event_timer_init(void)
{
    minheap_init(&heap);
    event_time_update();
}

void event_time_update(void)
{
    struct timeval tv;
    sec_t sec;
    msec_t msec;

    if (gettimeofday(&tv, NULL) < 0) {
        err_sys("gettimeofday error");
    }
    sec = tv.tv_sec;
    msec = tv.tv_usec / 1000;

    current_msec = sec * 1000 + msec;
}

msec_t event_find_timer(void)
{
    long timer;
    heap_node_t *node;

    if (minheap_isempty(&heap)) {
        return 0;
    }

    node = minheap_get_min(&heap);
    timer = node->key - current_msec;
    return timer > 0 ? timer : 0;
}

void event_expire_timers(void)
{
    heap_node_t *node;

    while (!minheap_isempty(&heap)) {
        node = minheap_get_min(&heap);

        if (node->deleted == HEAP_NODE_DELETED) {
            minheap_get_min(&heap);
            continue;
        }

        /* 
         * If node.key < current_msec, timeout.
         * Be careful, the gap should be cast to Long type.
         */
        if ((long) (node->key - current_msec) > 0) {
            return;
        }


        // destory the thread
        kill_thread_by_node(node);

        minheap_delete_min(&heap);
    }
}

void event_add_timer(pthread_t tid, int fd, msec_t timer)
{
    heap_node_t *node;
    msec_t key;

    if ((node = (heap_node_t *) malloc(sizeof(heap_node_t))) == NULL) {
        err_sys("malloc error");
    }

    event_time_update();
    key = current_msec + timer;
    node->key = key;
    node->tid = tid;
    node->fd = fd;
    node->deleted = HEAP_NODE_NOT_DELETED;
    minheap_insert(&heap, node);
}




/*
 *  kill the thread and the sockfd because of time out
 *  this func shouldn't be here, need to be optimized
 */
void kill_thread_by_node(heap_node_t *node)
{
    //int kill_rc;
    pthread_t tid = node->tid;
    int fd = node->fd;
    int ret;

    log_warn("Before cancel thread: %d \n", tid);
    ret = pthread_cancel(tid);

    if(ret != 0)
        log_warn("cancel thread: %d waring, cancel fail.\n", tid);
    else
        log_info("cancel thread: %d might success", tid);


    //close the fd
    //Close(fd);


/*    kill_rc = pthread_kill(tid, SIGQUIT);
    if(kill_rc == ESRCH)
        log_warn("kill thread: %d waring, the thread did not exists or already quit\n", tid);
    else if(kill_rc == EINVAL)
        log_warn("kill thread: %d waring, signal is invalid\n", tid);
*/
}