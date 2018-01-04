#ifndef REQUESTS_H
#define REQUESTS_H

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include "event.h"
#include "server.h"

void serve_runloop(struct HTTP_socket* sock);
void* runloop(void* data);

#endif
