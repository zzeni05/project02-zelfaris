/* queue.c: Concurrent Queue of Requests */

#include "smq/queue.h"
#include "smq/utils.h"

#include <stdlib.h>
#include <errno.h>

#ifndef QUEUE_CAPACITY
#define QUEUE_CAPACITY (4096)
#endif

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

        sem_init(&q->lock, 0, 1);
        sem_init(&q->produced, 0, 0);
        sem_init(&q->consumed, 0, QUEUE_CAPACITY);
    }

    return q;
}

/**
 * Delete queue structure.
 * @param   q       Queue structure.
 **/
void queue_delete(Queue *q) {
    if (q) {
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

    // This is a broadcast mechanism. By posting many times, we ensure that any
    // thread currently blocked on the semaphores will wake up.
    for (size_t i = 0; i < QUEUE_CAPACITY; i++) {
        sem_post(&q->produced);
        sem_post(&q->consumed);
    }
}

/**
 * Push message to the back of queue.
 * @param   q       Queue structure.
 * @param   r       Request structure.
 **/
void queue_push(Queue *q, Request *r) {
    if (!q || !r) return;

    sem_wait(&q->consumed);

    sem_wait(&q->lock);
    if (!q->running) {
        // If the queue was shut down while we were waiting, we must
        // release the lock and put the "consumed" ticket back so another
        // producer can wake up and see that the queue is down.
        sem_post(&q->consumed);
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
    struct timespec ts;
    compute_stoptime(ts, timeout);

    // This loop is only to handle spurious wakeups from signals (EINTR).
    while (sem_timedwait(&q->produced, &ts) == -1) {
        if (errno == EINTR) {
            continue; // Interrupted by a signal, so we try waiting again.
        }
        return NULL; // A real timeout or error occurred.
    }

    sem_wait(&q->lock);

    Request *value = NULL;
    if (!q->running && q->size == 0) {
        // This is a shutdown signal. Put the "produced" ticket back
        // so another consumer can wake up and see the queue is down.
        sem_post(&q->produced);
        sem_post(&q->lock);
        return NULL;
    }

    // This is the normal case: pop the message.
    value = q->head;
    if (value) {
        q->head = q->head->next;
        if (!q->head) {
            q->tail = NULL;
        }
        q->size--;
        sem_post(&q->consumed); // Signal that a slot is now free.
    } else {
        // This can happen if we are woken up by shutdown.
        // We must put the "produced" ticket back for the next consumer.
        sem_post(&q->produced);
    }

    sem_post(&q->lock);
    return value;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */

