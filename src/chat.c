/* chat.c: Chat application that utilizes the SMQ client library */

#include "smq/client.h"
#include "smq/thread.h"
#include "smq/utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

/* Constants */

#define BACKSPACE   127
#define TOPIC       "chat"

/* Globals */

char INPUT_BUFFER[BUFSIZ] = "";
Mutex STDOUT_LOCK; // Mutex to protect stdout

/* Functions */

/**
 * Toggles raw mode for the terminal.
 * This allows for character-by-character input processing.
 * https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 **/
void toggle_raw_mode() {
    static struct termios OriginalTermios = {0};
    static bool enabled = false;

    if (enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &OriginalTermios);
    } else {
        tcgetattr(STDIN_FILENO, &OriginalTermios);
        atexit(toggle_raw_mode);

        struct termios raw = OriginalTermios;
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;

        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        enabled = true;
    }
}

/* Threads */

/**
 * Receiver thread that continuously retrieves messages from the SMQ server
 * and prints them to the console.
 **/
void *receiver_thread(void *arg) {
    SMQ *smq = (SMQ *)arg;

    while (smq_running(smq)) {
        char *message = smq_retrieve(smq);
        if (message) {
            mutex_lock(&STDOUT_LOCK);
            // Erase the current line and print the received message
            printf("\r\033[K%s\n", message);
            // Redraw the input prompt with the current buffer
            printf("> %s", INPUT_BUFFER);
            fflush(stdout);
            mutex_unlock(&STDOUT_LOCK);
            free(message);
        }
    }
    return NULL;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    char *name = getenv("USER");
    char *host = "localhost";
    char *port = "9620";

    if (argc > 1) { name = argv[1]; }
    if (argc > 2) { host = argv[2]; }
    if (argc > 3) { port = argv[3]; }

    // Initialize stdout mutex
    mutex_init(&STDOUT_LOCK, NULL);

    // Create and start message queue client
    SMQ *smq = smq_create(name, host, port);
    if (!smq) {
        fprintf(stderr, "Unable to create SMQ client\n");
        return EXIT_FAILURE;
    }
    smq_subscribe(smq, TOPIC);

    // Set terminal to raw mode and print initial prompt
    toggle_raw_mode();
    mutex_lock(&STDOUT_LOCK);
    printf("> ");
    fflush(stdout);
    mutex_unlock(&STDOUT_LOCK);


    // Start receiver thread
    Thread receiver;
    thread_create(&receiver, NULL, receiver_thread, smq);

    // Main input loop (foreground thread)
    size_t input_index = 0;
    INPUT_BUFFER[0] = 0;
    
    size_t name_len = strlen(name);
    size_t prefix_len = name_len + 2; // for ": "
    size_t max_input_len = (BUFSIZ > prefix_len) ? (BUFSIZ - prefix_len - 1) : 0; // -1 for null terminator

    while (smq_running(smq)) {
        char input_char = 0;
        read(STDIN_FILENO, &input_char, 1);

        mutex_lock(&STDOUT_LOCK);

        if (input_char == '\n') { // Process commands and messages
            mutex_unlock(&STDOUT_LOCK); // Unlock before potentially blocking operations

            if (strcmp(INPUT_BUFFER, "/quit") == 0 || strcmp(INPUT_BUFFER, "/exit") == 0) {
                smq_shutdown(smq);
                break;
            }

            char message_buffer[BUFSIZ] = {0};
            // Construct the message piece-by-piece to satisfy the compiler
            strncpy(message_buffer, name, BUFSIZ - 1);
            strncat(message_buffer, ": ", BUFSIZ - strlen(message_buffer) - 1);
            strncat(message_buffer, INPUT_BUFFER, BUFSIZ - strlen(message_buffer) - 1);

            smq_publish(smq, TOPIC, message_buffer);

            input_index = 0;
            INPUT_BUFFER[0] = 0;
            
            mutex_lock(&STDOUT_LOCK); // Re-lock to print prompt
            printf("\r\033[K> %s", INPUT_BUFFER);

        } else if (input_char == BACKSPACE && input_index) { // Handle backspace
            INPUT_BUFFER[--input_index] = 0;
            printf("\r\033[K> %s", INPUT_BUFFER);
        } else if (!iscntrl(input_char) && input_index < max_input_len) { // Append character
            INPUT_BUFFER[input_index++] = input_char;
            INPUT_BUFFER[input_index]   = 0;
            printf("\r\033[K> %s", INPUT_BUFFER);
        }

        fflush(stdout);
        mutex_unlock(&STDOUT_LOCK);
    }

    // Cleanup
    thread_join(receiver, NULL);
    
    // Toggle raw mode off before final prints
    // Note: atexit() handles the final toggle, but we do it here for clean exit printing
    toggle_raw_mode(); 
    printf("\r\nDisconnected.\n");

    smq_delete(smq);
    mutex_destroy(&STDOUT_LOCK);

    return EXIT_SUCCESS;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */


