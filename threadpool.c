#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"
#include "dlist.h"
#include "wrappers.h"

typedef struct future
{
    struct list_elem elem;
    void* result;
    void* arg;
    fork_join_task execution;
    sem_t sem;
}future;

typedef struct thread_data
{
    struct list_elem elem;
    pthread_t thread;
}thread_data;

typedef struct threadpool
{
    struct dlist work_queue;
    struct dlist threads;
    bool running;
    pthread_cond_t condition;
    pthread_mutex_t lock;
}threadpool;
