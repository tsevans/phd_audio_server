#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

//Threading
void create_thread(pthread_t* tid, pthread_attr_t* attr, void*(*routine)(void*), void* arg);
void join_thread(pthread_t tid, void** ret);
void abort_thread(void* retval);
void init_thread_attrs(pthread_attr_t* attrs);
void set_thread_detachstate(pthread_attr_t* attr, int ds);

#endif
