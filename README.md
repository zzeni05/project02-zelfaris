# Project 02: Simple Message Queue

This is [Project 02] of [CSE.30341.FA25].

## Students

- Domer McDomerson (dmcdomer@nd.edu)
- Belle Fleur (bfleur@nd.edu)

## Video

[Reflection Video](...)

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

> Describe any known errors, bugs, or deviations from the requirements.

## Acknowledgments

> List anyone you collaborated with or received help from (including TAs, other
students, and AI tools)

[Project 02]: https://pnutz.h4x0r.space/courses/cse.30341.fa25/project02.html
[CSE.30341.FA25]: https://pnutz.h4x0r.space/courses/cse.30341.fa25/
[libcurl]: https://curl.se/libcurl/c/
[HTTP]: https://en.wikipedia.org/wiki/HTTP
