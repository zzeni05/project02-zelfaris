/* client.c: Simple Request Queue Client */

#include "smq/client.h"

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
    return NULL;
}

/**
 * Delete Simple Request Queue structure (and internal resources).
 * @param   smq     Simple Request Queue structure.
 **/
void smq_delete(SMQ *smq) {
}

/**
 * Publish one message to topic (by placing new Request in outgoing queue).
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic to publish to.
 * @param   body    Request body to publish.
 **/
void smq_publish(SMQ *smq, const char *topic, const char *body) {
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
    return NULL;
}

/**
 * Subscribe to specified topic.
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic string to subscribe to.
 **/
void smq_subscribe(SMQ *smq, const char *topic) {
}

/**
 * Unubscribe to specified topic.
 * @param   smq     Simple Request Queue structure.
 * @param   topic   Topic string to unsubscribe from.
 **/
void smq_unsubscribe(SMQ *smq, const char *topic) {
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
}

/**
 * Returns whether or not the Simple Request Queue is running.
 * @param   smq     Simple Request Queue structure.
 **/
bool smq_running(SMQ *smq) {
    return false;
}

/* Internal Functions */

/**
 * Pusher thread takes messages from outgoing queue and sends them to server.
 **/
void * smq_pusher(void *arg) {
    return NULL;
}

/**
 * Puller thread requests new messages from server and then puts them in
 * incoming queue.
 **/
void * smq_puller(void *arg) {
    return NULL;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
