/* queue.c: Concurrent Queue of Requests */

#include "smq/queue.h"
#include "smq/utils.h"

/**
 * Create queue structure.
 * @return  Newly allocated queue structure.
 **/
Queue * queue_create() {
    return NULL;
}

/**
 * Delete queue structure.
 * @param   q       Queue structure.
 **/
void queue_delete(Queue *q) {
}

/**
 * Shutdown queue.
 * @param   q       Queue structure.
 **/
void queue_shutdown(Queue *q) {
}

/**
 * Push message to the back of queue.
 * @param   q       Queue structure.
 * @param   r       Request structure.
 **/
void queue_push(Queue *q, Request *r) {
}

/**
 * Pop message to the front of queue (block until there is something to return).
 * @param   q       Queue structure.
 * @param   timeout How long to wait before re-checking condition (ms).
 * @return  Request structure.
 **/
Request * queue_pop(Queue *q, time_t timeout) {
    return NULL;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
