/* Request.c: Request structure */

#include "smq/request.h"
#include "smq/utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    Response *response = userdata;
    size_t capacity = nmemb * size;
    char *data = realloc(response->data, response->size + capacity + 1);
    if (!data) {
        return 0;
    }

    memcpy(data + response->size, ptr, capacity);
    response->size += capacity;
    data[response->size]='\0';
    response->data = data;

    return capacity;
}

/**
 * Reader function: Copy data up to size*nmemb from userdata (Payload) to ptr.
 *
 * @param   ptr         Pointer to data to deliver.
 * @param   size        Always 1.
 * @param   nmemb       Size of the data buffer.
 * @param   userdata    Pointer to user-provided Payload structure.
 **/
size_t request_reader(char *ptr, size_t size, size_t nmemb, void *userdata) {
    Payload *payload = userdata;                 // userdata was &Payload
    size_t capacity = size * nmemb;

    if (!payload || !payload->data) return 0;

    size_t total = strlen(payload->data);
    size_t remaining;
    if (total > payload->offset) {
        remaining = total - payload->offset;
    } else {
        remaining = 0;
    }
    if (remaining == 0) return 0;               // EOF

    size_t num_bytes = remaining;
    if (num_bytes > capacity) {
        num_bytes = capacity;
    }

    memcpy(ptr, payload->data + payload->offset, num_bytes);
    payload->offset += num_bytes;

    return num_bytes;                            // bytes provided to libcurl
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
        if (method) r->method = strdup(method);
        if (url)    r->url    = strdup(url);
        if (body)   r->body   = strdup(body);
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

    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Initialization failed\n");
        return NULL;
    }

    Response response = (Response){0};

    curl_easy_setopt(curl, CURLOPT_URL, r->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, request_writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);


    if (strcmp(r->method, "GET") == 0) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    } else if (strcmp(r->method, "PUT") == 0) {
        Payload payload;
        payload.data = NULL;
        payload.offset = 0;

        size_t payloadDataLength;
        if (r->body) {
            payload.data = r->body;
            payloadDataLength = strlen(r->body);
        } else {
            payload.data = "";
            payloadDataLength = 0;
        }

        payload.offset = 0;

        size_t body_len = payloadDataLength;

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, request_reader);
        curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)body_len);

    } else if (strcmp(r->method, "DELETE") == 0) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else {
        curl_easy_cleanup(curl);
        free(response.data);
        return NULL;
    }

    CURLcode result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        free(response.data);
        return NULL;
    }

    return response.data;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
