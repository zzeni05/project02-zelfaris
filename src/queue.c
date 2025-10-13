/* queue.c: Concurrent Queue of Requests */

#include "smq/queue.h"
#include "smq/utils.h"

/**
 * Create queue structure.
 * @return  Newly allocated queue structure.
 **/
Queue * queue_create(int sentinel, size_t capacity) {
    Queue *q = calloc(1, sizeof(Queue));

    if (q) {
        q->data = calloc(capacity, sizeof(int));
        q->sentinel = sentinel;
        q->capacity = capacity;

        // Initialize our mutex
        sem_init(&q->lock, 0, 1);

        // Initialize the condition variables
        sem_init(&q->produced, 0, 0);
        sem_init(&q->consumed, 0, q->capacity);
    }

    return NULL;
}


/**
 * Delete queue structure.
 * @param   q       Queue structure.
 **/
void queue_delete(Queue *q) {
    if (q) {
        free(q->data);
    	free(q);
    }

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
    sem_wait(&q->consumed);
    sem_wait(&q->lock);


    q->data[q->writer] = value;
    q->writer = (q->writer + 1) % q->capacity;
    q->size++;

    sem_post(&q->lock);
    sem_post(&q->produced);
}

/**
 * Pop message from the front of queue (block until there is something to return).
 * @param   q       Queue structure.
 * @param   timeout How long to wait before re-checking condition (ms).
 * @return  Request structure.
 **/
Request * queue_pop(Queue *q, time_t timeout) {

    sem_wait(&q->produced);
    sem_wait(&q->lock);
    sleep(timeout);

    int value = q->data[q->reader];
    if (value != q->sentinel) {
        q->reader = (q->reader + 1) % q->capacity;
        q->size--;
    }

    sem_post(&q->lock);
    if (value != q->sentinel) {
        sem_post(&q->consumed);
    } else {
        sem_post(&q->produced);
    }
    return value;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
