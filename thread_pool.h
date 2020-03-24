#ifndef THREAD_POOL_H
#define THREAD_POOL_H


#include "condition.h"

#define WAIT_TIME 30
#define MAX_THREADS 8

//task queue
typedef struct task
{
    void *(*run)(void *args);			//function pointer
    void *arg;							//args
    struct task *next;					//next task
}task_t;


typedef struct threadpool
{
    condition_t ready;					//condition for pool state
    task_t *first;						//the first task
    task_t *last;						//the last task
    int counter;						//number of threads in pool
    int idle;							//number of idle threads
    int max_threads;					//maximum number of threads
    int quit;							//quit flag
}threadpool_t;



void threadpool_init(threadpool_t *pool, int threads);
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg);
void threadpool_destroy(threadpool_t *pool);



#endif