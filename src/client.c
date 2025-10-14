/* client.c: Simple Request Queue Client */

#include "smq/client.h"
#include "smq/queue.h"
#include "smq/thread.h"
#include "smq/request.h"
#include <stdio.h>

/* Internal Prototypes */

void * smq_pusher(void *);
void * smq_puller(void *);

/* External Functions */

/**
 * Create Simple Request Queue with specified name, host, and port.
 *
 * - Initialize values.
 * - Create internal queues.
 * - Create pusher and puller threads.
 *
 * @param   name        Name of client's queue.
 * @param   host        Address of server.
 * @param   port        Port of server.
 * @return  Newly allocated Simple Request Queue structure.
 **/
SMQ * smq_create(const char *name, const char *host, const char *port) {
    SMQ *smq = calloc(1, sizeof(*smq));

    if (smq) {
        if (!name) name = "";
        strncpy(smq->name, name, sizeof smq->name - 1);
        smq->name[sizeof smq->name - 1] = '\0';

        if (!host) host = "localhost";
        if (!port) port = "9620";
        snprintf(smq->server_url, sizeof smq->server_url, "http://%s:%s", host, port);

        smq->timeout = 3000;
        smq->running = true;

        smq->outgoing = queue_create();
        smq->incoming = queue_create();
        if (!smq->outgoing || !smq->incoming) { 
            free(smq); return NULL; 
        }

        thread_create(&smq->pusher, NULL, smq_pusher, smq);
        thread_create(&smq->puller, NULL, smq_puller, smq);

        return smq;

    }

    return NULL;
}

/**
 * Delete Simple Request Queue structure (and internal resources).
 * @param   smq     Simple Request Queue structure.
 **/
void smq_delete(SMQ *smq) {
    if (!smq) return;
    if (smq->running) smq_shutdown(smq);
    if (smq->outgoing) queue_delete(smq->outgoing);
    if (smq->incoming) queue_delete(smq->incoming);
    free(smq);
}


/**
 * Publish one message to topic (by placing new Request in outgoing queue).
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic to publish to.
 * @param   body    Request body to publish.
 **/
void smq_publish(SMQ *smq, const char *topic, const char *body) {
    if (!smq || !topic || !smq->running) return;

    const char *method = "PUT";
    Queue *outgoing = smq->outgoing; 

    char url[1024];
    snprintf(url, sizeof(url), "%s/topic/%s", smq->server_url, topic);

    if (!body) body = "";

    Request *request = request_create(method, url, body);
    if (!request) return;

    queue_push(outgoing, request);

}

/**
 * Retrieve one message (by taking a Request from incoming queue).
 *
 * Note: if the SMQ is not longer running, this will return NULL.
 *
 * @param   smq     Simple Request Queue structure.
 * @return  Newly allocated message body (must be freed).
 **/
char * smq_retrieve(SMQ *smq) {
    if (!smq) return NULL;
    if (!smq->running) return NULL;


    Request *r = queue_pop(smq->incoming, smq->timeout);
    if (!r) return NULL;

    char *message = NULL;
    if (r->body) {
        message = r->body;      /* hand ownership to caller */
        r->body = NULL;
    }

    request_delete(r);
    return message;
}

/**
 * Subscribe to specified topic.
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic string to subscribe to.
 **/
void smq_subscribe(SMQ *smq, const char *topic) {
    if (!smq) return;
    if (!topic) return;
    if (!smq->running) return;

    const char *method = "PUT";
    Queue *outgoing = smq->outgoing;

    char url[1024];
    snprintf(url, sizeof url, "%s/subscription/%s/%s", smq->server_url, smq->name, topic);

    Request *request = request_create(method, url, "");
    if (!request) return;

    queue_push(outgoing, request);
}


/**
 * Unubscribe to specified topic.
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic string to unsubscribe from.
 **/
void smq_unsubscribe(SMQ *smq, const char *topic) {
    if (!smq) return;
    if (!topic) return;
    if (!smq->running) return;

    const char *method = "DELETE";
    Queue *outgoing = smq->outgoing;

    char url[1024];
    snprintf(url, sizeof url, "%s/subscription/%s/%s", smq->server_url, smq->name, topic);

    Request *request = request_create(method, url, "");
    if (!request) return;

    queue_push(outgoing, request);
}


/**
 * Shutdown the Simple Request Queue by:
 *
 * 1. Shutting down the internal queues.
 * 2. Setting the internal running attribute.
 * 3. Joining internal threads.
 *
 * @param   smq      Simple Request Queue structure.
 */
void smq_shutdown(SMQ *smq) {
    if (!smq) return;

    smq->running = false;

    if (smq->outgoing) queue_shutdown(smq->outgoing);
    if (smq->incoming) queue_shutdown(smq->incoming);

    thread_join(smq->pusher, NULL);
    thread_join(smq->puller, NULL);
}


/**
 * Returns whether or not the Simple Request Queue is running.
 * @param   smq     Simple Request Queue structure.
 **/
bool smq_running(SMQ *smq) {
    if (!smq) return false;

    if(smq->running) {
        return true;
    } else {
        return false;
    }
}

/* Internal Functions */

/**
 * Pusher thread takes messages from outgoing queue and sends them to server.
 **/
void * smq_pusher(void *arg) {
    SMQ *smq = (SMQ *)arg;

    if(smq_running(smq)) {
        Queue *outgoing = smq->outgoing;

        while (smq_running(smq)) {
            Request *request = queue_pop(outgoing, smq->timeout);
            if (!request) continue;

            char *response = request_perform(request, smq->timeout);
            if (response) free(response);

            request_delete(request);
        }

    } else {
        printf("Server not running");
    }


    return NULL;
}

/**
 * Puller thread requests new messages from server and then puts them in
 * incoming queue.
 **/
void * smq_puller(void *arg) {
   SMQ *smq = (SMQ *)arg;

    if (smq_running(smq)) {
        Queue *incoming = smq->incoming;
        const char *method = "GET";
        char url[1024];

        while (smq_running(smq)) {
            snprintf(url, sizeof url, "%s/queue/%s", smq->server_url, smq->name);

            Request *req = request_create(method, url, NULL);
            if (!req) continue;

            char *body = request_perform(req, smq->timeout);
            request_delete(req);
            if (!body) continue;

            Request *deliver = request_create("BODY", "", NULL);
            if (!deliver) { free(body); continue; }
            deliver->body = body;

            queue_push(incoming, deliver);
        }
    } else {
        printf("Server not running\n");
    }

    return NULL;
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
