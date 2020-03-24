#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"
#include "timer.h"

//routine of a single thread
void *thread_routine(void *arg)
{
    struct timespec abstime;
    int timeout;
    int status;
    int tid;

    log_info("thread %d is starting\n", (int)pthread_self());
    threadpool_t *pool = (threadpool_t *)arg;

/*    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);    //set type as PTHREAD_CANCEL_ASYCHRONOUS, cancel in time ,no need cancel point
*/
    //pthread_testcancel();


    while(1)
    {

        timeout = 0;
        //lock to access the state of the pool
        condition_lock(&pool->ready);

        //increase idle threads
        pool->idle++;
        
        //wait for task or wait for destroy, within WAIT_TIME seconds
        while(pool->first == NULL && !pool->quit)
        {
            log_info("thread %d is waiting\n", (int)pthread_self());
            
            //set wait time
            clock_gettime(CLOCK_REALTIME, &abstime);  
            abstime.tv_sec += WAIT_TIME;

            //sleep for wakeup signal
            //this func will release the lock and will lock when awake.
            //if awake by destroy, the quit flag will be set and exit the circle.
            status = condition_timedwait(&pool->ready, &abstime);  
            
            //timeout
            if(status == ETIMEDOUT)
            {
                log_info("thread %d wait timed out\n", (int)pthread_self());
                timeout = 1;
                break;
            }
        }
        
        pool->idle--;
        if(pool->first != NULL)
        {
            //take the first task from the list
            task_t *t = pool->first;
            pool->first = t->next;

            //tid = (int)pthread_self();
            //add an timer
            //event_add_timer(tid, t->arg, DEFAULT_TIMEOUT);

            //unlock when running the task
            //because it might cause a long time
            condition_unlock(&pool->ready);

            //run the task and free
            t->run(t->arg);
            free(t);

            //relock
            condition_lock(&pool->ready);
        }
        //after this thread finish the task , it becomes a idle thread and wait for the task.
        //if there is no task within WAIT_TIME, the thread will be destroy.


        //timeout
        //let the thread break from the circle
        if(timeout == 1)
        {
            pool->counter--;
            condition_unlock(&pool->ready);
            break;
        }

        //Awake by destroy
        //exit from the pool
        if(pool->quit && pool->first == NULL)
        {
            pool->counter--;

            //if this is the last thread, notify the main thread that destory finish.
            if(pool->counter == 0)
            {
                condition_signal(&pool->ready);
            }
            condition_unlock(&pool->ready);
            break;
        }

        condition_unlock(&pool->ready);
    }
    
    log_info("thread %d is exiting\n", (int)pthread_self());
    return NULL;
    
}


//initialize the pool
void threadpool_init(threadpool_t *pool, int threads)
{
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last =NULL;
    pool->counter =0;
    pool->idle =0;
    pool->max_threads = (threads < MAX_THREADS)? threads : MAX_THREADS;
    pool->quit =0;
}


//add task
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg)
{
    //The new task is placed at the end of the list
    task_t *newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next=NULL;
    
    //the state of the pool is shared by all thread, need to lock
    condition_lock(&pool->ready);
    
    if(pool->first == NULL)
        pool->first = newtask;
    else   
        pool->last->next = newtask;

    pool->last = newtask;
    
    //wake a thread if there is idle thread
    if(pool->idle > 0)
    {
        condition_signal(&pool->ready);
    }
    //if all threads are busy and there is space, build a new thread
    else if(pool->counter < pool->max_threads)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, pool);
        pool->counter++;
    }

    //unlock
    condition_unlock(&pool->ready);
}

//destroy the pool
void threadpool_destroy(threadpool_t *pool)
{
    //already destroy
    if(pool->quit)
    {
    return;
    }
    //lock
    condition_lock(&pool->ready);
    //set the flag
    pool->quit = 1;

    //still have thread in the pool
    if(pool->counter > 0)
    {
        //wake the sleeping thread
        if(pool->idle > 0)
        {
            condition_broadcast(&pool->ready);
        }

        //wait for the runing threads
        while(pool->counter)
        {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}

