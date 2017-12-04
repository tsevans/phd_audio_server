#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

//POSIX Threads Interface
void create_thread(pthread_t* tid, pthread_attr_t* attr, void*(*routine)(void*), void* arg);
void join_thread(pthread_t tid, void** ret);
void abort_thread(void* retval);
void init_thread_attrs(pthread_attr_t* attrs);
void set_thread_detachstate(pthread_attr_t* attr, int ds);

//Sockets interface
int create_socket(int domain, int type, int protocol);
void bind_socket(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
void listen_for_connection(int sockfd, int backlog);
int accept_connection(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
void connect_socket(int sockfd, struct sockaddr* addr, socklen_t addrlen);

#endif
