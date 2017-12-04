#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "parser.h"
#include "wrappers.h"

void handle_request(struct HTTP_socket* sock, struct HTTP_request* request);
void* read_from_socket(void* data);
void* write_to_socket(void* data);
void change_root_path(char* path);
int load_file(struct HTTP_socket* sock, char* fname);
void write_buffer(struct buffer* buf, char* str, ...);
void serve_text(struct HTTP_socket* sock, int err);
void finish_read(struct HTTP_socket* sock);

#endif
