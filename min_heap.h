#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <sys/time.h>
#include <pthread.h>

#define MAX_HEAP_SIZE		65535

typedef time_t        sec_t;		/* Seconds */
typedef unsigned long msec_t;		/* Milliseconds */


/*
 * Heap node delete state
 */
#define HEAP_NODE_DELETED      1
#define HEAP_NODE_NOT_DELETED  0


/*
 * use volatile keep it not to be omitted by compiler optimization
 * make it save to be shared by multithreads
 */
volatile msec_t current_msec;

/*
 * Heap node structure
 */
typedef struct heap_node 
{
	msec_t				key;
	pthread_t			tid;
	int					fd;
	unsigned short		deleted;
} heap_node_t;


typedef struct min_heap
{
    int capacity;
    int size;
    heap_node_t **nodes; 				//an array of int for fd
} heap_t;


void minheap_init(heap_t *heap);
void minheap_insert(heap_t *heap, heap_node_t *node);
heap_node_t *minheap_get_min(heap_t *heap);
void minheap_delete_min(heap_t *heap);
void minheap_delete_node(heap_t *heap, heap_node_t *node);
int minheap_isempty(heap_t *heap);

#endif