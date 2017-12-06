#include "wrappers.h"

/*
 * Create a POSIX thread.
 *
 * @param tid - Thread ID of new thread.
 * @param attr - Attributes for new thread.
 * @param routine - Routine executed by new thread.
 * @param arg - Argument passed to routine.
 */
void create_thread(pthread_t* tid, pthread_attr_t* attr, void*(*routine)(void*), void* arg)
{
    int retcode;
    if ((retcode = pthread_create(tid, attr, routine, arg)) != 0)
    {
        fprintf(stderr, "Error creating thread, returned code: %s\n", retcode);
        exit(0);
    }
}

/*
 * Wait for thread tid to terminate, immediate if already terminated.
 *
 * @param tid - Thread ID of thread to wait for.
 * @param ret - If not NULL, location to copy exit status of tid.
 */
void join_thread(pthread_t tid, void** ret)
{
    int retcode;
    if ((retcode = pthread_join(tid, ret)) != 0)
    {
        fprintf(stderr, "Error joining thread, returned code: %s\n", retcode);
        exit(0);
    }
}

/*
 * Terminate calling thread.
 *
 * @param retval - Value returned from terminated thread.
 */
void abort_thread(void* retval)
{
    pthread_exit(retval);
}

/*
 * Initialize attributes structure for a thread.
 *
 * @param attrs - Thread attributes for initialized structure.
 */
void init_thread_attrs(pthread_attr_t* attrs)
{
    int retcode;
    if ((retcode = pthread_join(tid, ret)) != 0)
    {
        fprintf(stderr, "Error initializing attributes for thread, returned code: %s\n", retcode);
        exit(0);
    }
}

/*
 * Set the detach state attribute for a thread.
 *
 * @param attr - Thread attributes structure to modify.
 * @param ds - Detachstate for thread, toggles deatched/joinable
 */
void set_thread_detachstate(pthread_attr_t* attr, int ds)
{
    int retcode;
    if ((retcode = pthread_attr_setdetachstate(attr, ds)) != 0)
    {
        fprintf(stderr, "Error setting detach state attribute for thread, returned code: %s\n", retcode);
        exit(0);
    }
}

/*
 * Create a socket.
 *
 * @param domain - The communication domain.
 * @param type - The type of socket.
 * @param protocol - The protocol code to use for the socket.
 * @return - File descriptor for socket.
 */
int create_socket(int domain, int type, int protocol)
{
    int retcode;
    if ((retcode = socket(domain, type, protocol)) < 0)
    {
        fprintf(stderr, "Error creating socket!: %s\n", strerror(errno));
        exit(0);
    }
    return retcode;
}

/*
 * Bind a socket to an address.
 *
 * @param sockfd - File descriptor of socket to bind.
 * @param addr - Address to be bound.
 * @param addrlen - Size of address to be bound.
 */
void bind_socket(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
    int retcode;
    if ((retcode = bind(sockfd, addr, addrlen)) < 0)
    {
        fprintf(stderr, "Error binding socket!: %s\n", strerror(errno));
        exit(0);
    }
}

/*
 * Set socket to listen for connection requests.
 *
 * @param sockfd - File descriptor of socket to set to listening.
 * @param backlog - Max length to which connection queue can grow.
 */
void listen_for_connection(int sockfd, int backlog)
{
    int retcode;
    if ((retcode = listen(sockfd, backlog)) < 0)
    {
        fprintf(stderr, "Error setting socket to listening state!: %s\n", strerror(errno));
        exit(0);
    }
}

/*
 * Accept connection from listening socket.
 *
 * @param sockfd - File descriptor of listening socket.
 * @param addr - Address of peer socket.
 * @param addrlen - Length of the address.
 * @return - File descriptor for accepted socket.
 */
int accept_connection(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
    int retcode;
    if ((retcode = accept(sockfd, addr, addrlen)) < 0)
    {
        fprintf(stderr, "Error accepting socket!: %s\n", strerror(errno));
        exit(0);
    }
    return retcode;
}

/*
 * Create listening file descriptor ready to receive connections.
 * Compliant with IPv6 addresses.
 *
 * @param port - Port to receive connection requests on.
 * @return - Opened file descriptor, -1 if error occurs.
 */
int open_listenfd(int port)
{
    int optval = 1;
    struct sockaddr_in6 serveraddr;

    //Create socket descriptor
    int listenfd = create_socket(AF_INET6, SOCK_STREAM, 0);

    //Prevent binding address already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*) &optval, sizeof(int)) < 0)
        return -1;

    memset((char*) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin6_family = AF_INET6;
    serveraddr.sin6_addr = in6addr_any;
    serveraddr.sin6_port = htons((unsigned short) port);

    bind_socket(listenfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    listen_for_connection(listenfd, 25);

    return listenfd;
}
