#include "server.h"
#include "event.h"
#include "wrappers.h"

static int socket_port = 6969; //Default port
static int epoll_fd;
static struct threadpool* pool;

int main(int argc, char** argv)
{
    struct sockaddr_in clientaddr;
    int listen_socket;
    if ((listen_socket = open_listenfd(socket_port)) < 0)
    {
        fprintf(stderr, "Error opening listen file descriptor!: %s\n", strerror(errno));
        exit(0);
    }

    pool = new_threadpool(10);  //Create threadpool with 10 threads
    epoll_fd = epoll_create(1); //Create reading epoll set

    pthread_t poll_thread;
    create_thread(&poll_thread, NULL, poll_connections, NULL);

    socklen_t clientlen = sizeof(clientaddr);

    while (true)
    {
        int connection = accept_connection(listen_socket, (struct sockaddr*) &clientaddr, &clientlen);

        //Make socket non-blocking
        int flags = fcntl(connection, F_GETFL, 0);
        fcntl(connection, F_SETFL, flags | O_NONBLOCK);

        struct HTTP_socket* new_sock = calloc(1, sizeof(struct HTTP_socket));
        new_sock->fd = connection;
        new_sock->last_access = time(NULL);
        new_sock->event.events = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT;
        new_sock->event.data.ptr = new_sock;

        //Add connection to epoll set
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection, &new_sock->event);
    }

    terminate_threadpool(pool);
    return 0;
}

/*
 * Thread routine checks TCP connections on server.
 *
 * @param data - Data for thread executing routine.
 * @return -
 */
void* poll_connections(void* data)
{
    struct epoll_event ready_events[10];

    while (true)
    {
        int wait_flag = epoll_wait(epoll_fd, ready_events, 10, -1);
        int x;
        for (x = 0; x < wait_flag; x++)
        {
            if (ready_events[x].events & EPOLLRDHUP || ready_events[x].events & EPOLLERR || ready_events[x].events & EPOLLHUP)
                close_socket((struct HTTP_socket*)ready_events[x].data.ptr);
            else if (ready_events[x].events & EPOLLIN)
                threadpool_submit(pool, (fork_join_task)read_from_socket, (void*)ready_events[x].data.ptr);
            else if (ready_events[x].events & EPOLLOUT)
                threadpool_submit(pool, (fork_join_task)write_to_socket, (void*)ready_events[x].data.ptr);
        }
    }
}

/*
 * Checks epoll events for a read.
 *
 * @param sock - Socket to check.
 * @return - 0 on successful call, -1 on error.
 */
int check_read(struct HTTP_socket* sock)
{
    sock->event.events = EPOLLIN | EPOLLRDHUP | EPOLLONESHOT;
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock->fd, &sock->event);
}

/*
 * Clear data buffers in a socket.
 *
 * @param sock - Socket to clear.
 */
void clear_buffers(struct HTTP_socket* sock)
{
    //Clear read buffer
    if (sock->read_buf != NULL)
    {
        free(sock->read_buf);
        sock->read_buf = NULL;
        sock->read_buf_size = 0;
    }

    //Clear write buffer
    if (sock->write_buf.data != NULL)
    {
        free(sock->write_buf.data);
        sock->write_buf.data = NULL;
        sock->write_buf.size = 0;
        sock->write_buf.pos = 0;
        sock->write_buf.end_pos = 0;
    }

    //Clear data buffers
    if (sock->data.data != NULL)
    {
        free(sock->data.data);
        sock->data.data = NULL;
        sock->data.size = 0;
        sock->data.pos = 0;
        sock->data.end_pos = 0;
    }
}

/*
 * Checks epoll events for a write.
 *
 * @param sock - Socket to check.
 * @return - 0 on successful call, -1 on error.
 */
int check_write(struct HTTP_socket* sock)
{
    sock->event.events = EPOLLOUT | EPOLLRDHUP | EPOLLONESHOT;
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sock->fd, &sock->event);
}

/*
 * Close a socket and free associated memory.
 *
 * @param sock - Socket to close.
 * @return - 0 on success.
 */
int close_socket(struct HTTP_socket* sock)
{
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock->fd, &sock->event))
        perror("Error deleting socket from epoll set!\n");

    if (close(sock->fd) < 0)
    {
        fprintf(stderr, "Error closing file descriptor! Code: %s\n", strerror(errno));
        exit(0);
    }

    if (sock->read_buf != NULL)
        free(sock->read_buf);

    if (sock->write_buf.data != NULL)
        free(sock->write_buf.data);

    if (sock->data.data != NULL)
    {
        if (sock->is_mmaped)
        {
            if (munmap(sock->data.data, sock->data.size) < 0)
            {
                fprintf(stderr, "Error munmapping file!: %s\n", strerror(errno));
                exit(0);
            }
        }
        else
            free(sock->data.data);
    }

    free(sock);
    return 0;
}
