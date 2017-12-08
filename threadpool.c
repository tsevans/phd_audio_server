#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"
#include "dlist.h"
#include "wrappers.h"

static void* runner_thread_routine(void* tpool);

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
    struct list work_queue;
    struct list threads;
    bool running;
    pthread_cond_t condition;
    pthread_mutex_t lock;
}threadpool;

/*
 * Create a new threadpool with no more than nthreads.
 *
 * @param nthreads - Max number of threads for the pool.
 * @return - Newly created threadpool.
 */
struct threadpool* new_threadpool(int nthreads)
{
    struct threadpool* pool = malloc(sizeof(struct threadpool));
    pthread_cond_init(&pool->condition, NULL);
    pthread_mutex_init(&pool->lock, NULL);
    list_init(&pool->work_queue);
    list_init(&pool->threads);
    pool->running = true;
    pthread_mutex_lock(&pool->lock);

    int x;
    for (x = 0; x < nthreads; x++)
    {
        struct thread_data* tdata = malloc(sizeof(struct thread_data));
        create_thread(&tdata->thread, NULL, runner_thread_routine, (void *) pool);
        list_push_back(&pool->threads, &tdata->elem);
    }

    pthread_mutex_unlock(&pool->lock);
    return pool;
}

/*
 * Thread routine for runner threads.
 *
 * @param tpool - Pointer to threadpool.
 * @return - NULL for end of routine.
 */
static void* runner_thread_routine(void* tpool)
{
    struct threadpool* pool = (struct threadpool*) tpool;
    struct future* fut = NULL;
    pthread_mutex_lock(&pool->lock);

    while (pool->running)
    {
        if (!list_empty(&pool->work_queue))
        {
            fut = list_entry(list_pop_front(&pool->work_queue), struct future, elem);
            pthread_mutex_unlock(&pool->lock);
            fut->result = fut->execution(fut->arg);
            sem_post(&(fut->sem));
            free(fut);
            pthread_mutex_lock(&pool->lock);
        }
        else
            pthread_cond_wait(&pool->condition, &pool->lock);
    }

    pthread_mutex_unlock(&pool->lock);
    return NULL;
}

/*
 * Shut down and destroy threadpool in orderly fashion.
 * Tasks in pool may or may not be executed.
 *
 * @param pool - Threadpool to terminate.
 */
void terminate_threadpool(struct threadpool* pool)
{
    pthread_mutex_lock(&pool->lock);
    pool->running = false;
    pthread_cond_broadcast(&pool->condition);

    while (!list_empty(&pool->threads))
    {
        struct thread_data* tdata = list_entry(list_pop_front(&pool->threads), struct thread_data, elem);
        void* retval;
        pthread_mutex_unlock(&pool->lock);
        join_thread(tdata->thread, &retval);
        free(tdata);
        pthread_mutex_lock(&pool->lock);
    }

    while (!list_empty(&pool->work_queue))
        list_pop_front(&pool->work_queue);

    pthread_mutex_unlock(&pool->lock);
    pthread_cond_destroy(&pool->condition);
    pthread_mutex_destroy(&pool->lock);
    free(pool);
}

/*
 * Submit fork-join task to threadpool to get a future.
 *
 * @param pool - Thread pool to submit task to.
 * @param task - Task to submit.
 * @param data - Data to be used in the task.
 */
void threadpool_submit(struct threadpool* pool, fork_join_task task, void* data)
{
    struct future* fut = malloc(sizeof(struct future));
    fut->execution = task;
    fut->arg = data;
    sem_init(&fut->sem, 0, 0);
    pthread_mutex_lock(&pool->lock);
    list_push_back(&pool->work_queue, &fut->elem);
    pthread_cond_signal(&pool->condition);
    pthread_mutex_unlock(&pool->lock);
}
