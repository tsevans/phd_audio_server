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
