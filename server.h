#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <error.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include "dlist.h"
#include "parser.h"
#include "threadpool.h"

typedef struct write_buffer
{
    char* data;  //Data in buffer
    int size;    //Size of buffer
    int pos;     //Current index in buffer
    int end_pos; //Last written index in buffer
}buffer;

typedef struct HTTP_socket
{
    int fd;                    //File descriptor for socket
    int read_buf_size;         //Size of read buffer
    char* read_buf;            //Read buffer
    bool keep_alive;           //Keep alive flag
    struct buffer write_buf;   //Write buffer
    struct buffer data;        //Data buffer
    struct epoll_event event;  //Epoll event
    struct list_elem elem;     //List element for socket
    time_t last_access;        //Time of last access
}HTTP_socket;

void* poll_connections(void* data);
int check_read(struct HTTP_socket* sock);
int check_write(struct HTTP_socket* sock);
int close_socket(struct HTTP_socket* sock);
void clear_buffers(struct HTTP_socket* sock);

#endif
