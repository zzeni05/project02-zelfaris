/* queue.h: SMQ Concurrent Queue of Requests */

#ifndef SMQ_QUEUE_H
#define SMQ_QUEUE_H

#include "smq/request.h"
#include "smq/thread.h"

#include <stdbool.h>
#include <semaphore.h>
#include <time.h>

/* Structures */

typedef struct Queue Queue;
struct Queue {
    Request *head;
    Request *tail;
    size_t   size;
    bool     running;

    sem_t    lock;
    sem_t    consumed;
    sem_t    produced;
};

/* Functions */

Queue *     queue_create();
void        queue_delete(Queue *q);

void        queue_shutdown(Queue *q);

void        queue_push(Queue *q, Request *r);
Request *   queue_pop(Queue *q, time_t timeout);

#endif

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
