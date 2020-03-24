

#include <stdlib.h>
#include "error_wrap.h"
#include "min_heap.h"


/*static fun means they can only be used in this file*/

//choose the smaller one for min heap
static int minheap_cmp(heap_t *heap, int i, int j)
{
	if(i > heap->size || j > heap->size)
		err_sys("heap nodes compare error");
	heap_node_t *a = heap->nodes[i], *b = heap->nodes[j];
	return a->key < b->key;
}

static void minheap_exch(heap_t *heap, int i, int j)
{
	if(i > heap->size || j > heap->size)
		err_sys("heap nodes exchange error");
	heap_node_t *t = heap->nodes[i];
	heap->nodes[i] = heap->nodes[j];
	heap->nodes[j] = t;
}

//swim the node to the right place.
static void minheap_swim(heap_t *heap, int k)
{
	while(k > 1 && minheap_cmp(heap, k, k >> 1))
	{
		minheap_exch(heap, k, k >> 1);
		k >>= 1;
	}
}

//sink the node to the right place.
static void minheap_sink(heap_t *heap, int k)
{
	int i;
	while(k << 1 <= heap->size)
	{
		i = k << 1;
		if(i < heap->size && minheap_cmp(heap, i+1, i))
			++i;
		if(minheap_cmp(heap, k, i))	break;
		minheap_exch(heap, k, i);
		k = i;
	}
}


void minheap_init(heap_t *heap)
{
	if ((heap->nodes = (heap_node_t **) 
				malloc(MAX_HEAP_SIZE * sizeof(heap_node_t*))) == NULL) {
        err_sys("malloc heap nodes error");
    }
    heap->size = 0;
}


void minheap_insert(heap_t *heap, heap_node_t *node)
{
	if (!heap || !heap->nodes) {
        err_sys("the heap has not been initialized");
    }
    heap->nodes[++heap->size] = node;
    minheap_swim(heap, heap->size);
}

heap_node_t *minheap_get_min(heap_t *heap)
{
	if (!heap || !heap->nodes)
        err_sys("the heap has not been initialized");

    if(minheap_isempty(heap))
    	err_sys("the heap is empty");

	heap_node_t *min = heap->nodes[1];
    return min;

}

void minheap_delete_min(heap_t *heap)
{
	minheap_exch(heap, 1, heap->size--);
    free(heap->nodes[heap->size + 1]);		/* Don't forget */
    minheap_sink(heap, 1);
}


void minheap_delete_node(heap_t *heap, heap_node_t *node)
{
	/*
	 * Actually we don't need to delete
	 * Only need to check the "deleted" state
	 */
	if(NULL == node) {
		return;
    }

    if (node->deleted == HEAP_NODE_DELETED) {
        log_warn("The min heap node has already been marked as [DELETED].");
        return;
    }

    // mark the delete node
    node->deleted = HEAP_NODE_DELETED;
}

int minheap_isempty(heap_t *heap) 
{
    return !heap->size;
}

unsigned int minheap_size(heap_t *heap) 
{
    return heap->size;
}