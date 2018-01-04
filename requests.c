#include "requests.h"

/*
 * Synthetic load request to run thread in parallel that loops for
 * 10 seconds, temporarily increasing load average of server threads.
 *
 * @param sock - Socket to serve runloop request to.
 */
void serve_runloop(struct HTTP_socket* sock)
{
    pthread_attr_t attrs;
    initialize_attrs(&attrs);
    set_attrs_detachstate(&attrs, PTHREAD_CREATE_DETACHED);
    pthread_t thread;
    create_thread(&thread, &attrs, &runloop, NULL);

    write_buffer(&sock->data, "Spinning new thread on server for 10 seconds.\n");
    serve_text(sock, 0);
}

/*
 * Thread routine to let thread 'spin' for 10 seconds.
 *
 * @param data - NULL, no arguments for routine.
 * @return - Exit status of thread.
 */
void* runloop(void* data)
{
    printf("Starting loop...\n");
    time_t start_time = time(NULL);
    time_t curr_time = time(NULL);

    while ((start_time + 10) >= curr_time)
        curr_time = time(NULL);

    printf("... Loop finished.\n");
    return NULL;
}
