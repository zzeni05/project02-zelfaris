# Project 02: Simple Message Queue

This is [Project 02] of [CSE.30341.FA24].

## Students

- Zenidene El Farissi (zelfaris@nd.edu)

## Video

[Reflection Video](https://www.youtube.com/watch?v=zXPjAKeR4Ro)

## Brainstorming

The following are questions that should help you in thinking about how to
approach implementing [Project 02].  For this project, responses to these
brainstorming questions **are not required**.

### Request

1. What data must be allocated and deallocated for each `Request` structure?

2. How do you use [libcurl] to perform a simple [HTTP] request?

### Queue

1. What data must be allocated and deallocated for each `Queue` structure?

2. How will you implement **mutual exclusion**?

3. How will you implement **signaling**?

4. What are the **critical sections**?

5. What is the purpose of `queue_shutdown`?

6. How will the `timeout` in `queue_pop` be used?

### Client

1. What data must be allocated and deallocated for each `SMQ` structure?

2. What should happen when the user **publishes** a message?

3. What should happen when the user **retrieves** a message?

4. What should happen when the user **subscribes** to a topic?

5. What should happen when the user **unsubscribes** to a topic?

6. How many internal **threads** are required?

7. What is the purpose of each internal **thread**?

8. When does each **thread** get created and joined?

9. What `SMQ` attribute needs to be **protected** from **concurrent** access?

10. How is the client **shutdown**?

## Errata

> Occasional race condition when spamming inputs in as messages on separate terminals - breifly mentioned in video. 

## Acknowledgments

> List anyone you collaborated with or received help from (including TAs, other
students, and AI tools)

For this project, I utilized Google Gemini & ChatGPT for debugging and conceptual clarification.

Debugging: The AI was instrumental in helping diagnose and resolve several complex concurrency issues. This included identifying the root cause of deadlocks in the test_queue.c functional test under high load, as well as fixing subtle race conditions in the chat.c application related to thread-safe printing to the terminal. The process involved generating instrumented code with debug prints and analyzing the output to pinpoint the exact nature of the bugs.

Conceptual Understanding: I consulted the AI to gain a deeper understanding of core concepts relevant to the project, such as the producer-consumer problem, the role of bounded buffers in providing backpressure, and the specific mechanisms of semaphores and mutexes in preventing deadlocks and race conditions.

While the AI provided significant debugging assistance, the final implementation, testing, and overall understanding of the project's architecture and concurrency principles were my own.


[Project 02]: https://www3.nd.edu/~pbui/teaching/cse.30341.fa24/project02.html
[CSE.30341.FA24]: https://www3.nd.edu/~pbui/teaching/cse.30341.fa24/
[libcurl]: https://curl.se/libcurl/c/
[HTTP]: https://en.wikipedia.org/wiki/HTTP
