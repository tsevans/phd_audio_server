#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>

//POSIX Threads Interface
void create_thread(pthread_t* tid, pthread_attr_t* attr, void* (*routine)(void*), void* arg);
void join_thread(pthread_t tid, void** ret);
void abort_thread(void* retval);
void initialize_attrs(pthread_attr_t* attrs);
void set_attrs_detachstate(pthread_attr_t* attrs, int detachstate);

//Sockets interface
int create_socket(int domain, int type, int protocol);
void bind_socket(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
void listen_for_connection(int sockfd, int backlog);
int accept_connection(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int open_listenfd(int port);

#endif
