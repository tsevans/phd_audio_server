#ifndef THREADPOOL_H
#define THREADPOOL_H

struct threadpool* new_threadpool(int nthreads);
void terminate_threadpool(struct threadpool* pool);
typedef void* (* fork_join_task) (void* data);
void threadpool_submit(struct threadpool* pool, fork_join_task task, void* data);

#endif
