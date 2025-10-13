/* queue.c: Concurrent Queue of Requests */

#include "smq/queue.h"
#include "smq/utils.h"

#include <stdlib.h>

/**
 * Create queue structure.
 * @return  Newly allocated queue structure.
 **/
Queue * queue_create() {
    Queue *q = calloc(1, sizeof(Queue));

    if (q) {
        q->head    = NULL;
        q->tail    = NULL;
        q->size    = 0;
        q->running = true;

        // Initialize our mutex
        sem_init(&q->lock, 0, 1);

        // Initialize the condition variables
        sem_init(&q->produced, 0, 0);
        sem_init(&q->consumed, 0, 0);
    }

    return q;
}


/**
 * Delete queue structure.
 * @param   q       Queue structure.
 **/
void queue_delete(Queue *q) {
    if (q) {
        // drain and free any remaining requests
        sem_wait(&q->lock);
        Request *cur = q->head;
        while (cur) {
            Request *next = cur->next;
            request_delete(cur);
            cur = next;
        }
        q->head = q->tail = NULL;
        q->size = 0;
        sem_post(&q->lock);

        sem_destroy(&q->produced);
        sem_destroy(&q->consumed);
        sem_destroy(&q->lock);

        free(q);
    }

}

/**
 * Shutdown queue.
 * @param   q       Queue structure.
 **/
void queue_shutdown(Queue *q) {
    sem_wait(&q->lock);
    q->running = false;
    sem_post(&q->lock);

    for (size_t i = 0; i < 1024; i++) {
        sem_post(&q->produced);
    }

    sem_wait(&q->lock);
    Request *cur = q->head;
    while (cur) {
        Request *next = cur->next;
        cur = next;
    }
    q->head = q->tail = NULL;
    q->size = 0;
    sem_post(&q->lock);
}

/**
 * Push message to the back of queue.
 * @param   q       Queue structure.
 * @param   r       Request structure.
 **/
void queue_push(Queue *q, Request *r) {
    sem_wait(&q->lock);

    if (!q->running) {
        sem_post(&q->lock);
        return;
    }

    r->next = NULL;
    if (q->tail) {
        q->tail->next = r;
        q->tail = r;
    } else {
        q->head = q->tail = r;
    }
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

    Request *value = NULL;

    if (q->head) {
        value = q->head;
        q->head = q->head->next;
        if (!q->head) {
            q->tail = NULL;
        }
        q->size--;
        sem_post(&q->lock);
        return value;
    }

    if (!q->running) {
        sem_post(&q->lock);
        return NULL;
    }

    sem_post(&q->lock);
    return NULL;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
