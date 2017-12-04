#include "server.h"
#include "event.h"
#include "wrappers.h"

static int socket_port = 6969;
static int epoll_fd;
static struct threadpool* pool;

int main(int argc, char** argv)
{
    //Peniz
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
                threadpool_submit(); //TODO: memory leak from allocated future
            else if (ready_events[x].events & EPOLLOUT)
                threadpool_submit();
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
        free(sock->data.data);

    free(sock);
    return 0;
}
