/* Request.c: Request structure */

#include "smq/request.h"
#include "smq/utils.h"

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

/* Internal Structures */

typedef struct {
    char *       data;      // Response data string
    size_t       size;      // Response data length
} Response;

typedef struct {
    const char * data;      // Payload data string
    size_t       offset;    // Payload data offset
} Payload;

/* Internal Functions */

/**
 * Writer function: Copy data up to size*nmemb from ptr to userdata (Response).
 *
 * @param   ptr         Pointer to delivered data.
 * @param   size        Always 1.
 * @param   nmemb       Size of the delivered data.
 * @param   userdata    Pointer to user-provided Response structure.
 **/
size_t  request_writer(char *ptr, size_t size, size_t nmemb, void *userdata) {
    char buffer[BUFSIZ];

    if (fgets(ptr, buffer, userdata) != NULL) {
        printf("Successfully copied ptr to userdata");
    } else {
        printf("Error reading input/buffer empty");
    }

    return 0;
}

/**
 * Reader function: Copy data up to size*nmemb from userdata (Payload) to ptr.
 *
 * @param   ptr         Pointer to data to deliver.
 * @param   size        Always 1.
 * @param   nmemb       Size of the data buffer.
 * @param   userdata    Pointer to user-provided Payload structure.
 **/
size_t  request_reader(char *ptr, size_t size, size_t nmemb, void *userdata) {
    char buffer[BUFSIZ];

    if (fgets(userdata, buffer, ptr) != NULL) {
        printf("Successfully copied ptr to userdata");
    } else {
        printf("Error reading input/buffer empty");
    }

    return 0;
}

/* Functions */

/**
 * Create Request structure.
 * @param   method      Request method string.
 * @param   uri         Request uri string.
 * @param   body        Request body string.
 * @return  Newly allocated Request structure.
 **/
Request * request_create(const char *method, const char *url, const char *body) {
    Request *r = calloc(1, sizeof(*r));

    if (r) {
        r->method = strdup(method);
        r->url = strdup(url);
        r->body = strdup(body);
    }

    return r;
}

/**
 * Delete Request structure.
 * @param   r           Request structure.
 **/
void request_delete(Request *r) {
    if (r) {
        free(r->method);
        free(r->url);
        free(r->body);
        free(r);
    }
}

/**
 * Perform HTTP request using libcurl.
 *
 *  1. Initialize curl.
 *  2. Set curl options.
 *  3. Perform curl.
 *  4. Cleanup curl.
 *
 * Note: this must support GET, PUT, and DELETE methods, and adjust options as
 * necessary to support these methods.
 *
 * @param   r           Request structure.
 * @param   timeout     Maximum total HTTP transaction time (in milliseconds).
 * @return  Body of HTTP response (NULL if error or timeout).
 **/
char * request_perform(Request *r, long timeout) {
    int WORKER_THREADS = 1;

    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Initialization failed\n");
        return 0;
    }

    curl_easy_cleanup(curl);

    curl_easy_setopt(curl, CURLOPT_URL);
    



    return NULL;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
