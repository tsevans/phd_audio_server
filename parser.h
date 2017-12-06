#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "server.h"

typedef struct HTTP_request
{
    char* method;
    char* uri;
    char* version;
    char* callback;
    bool keep_alive;
}HTTP_request;

bool end_of_buffer(char* buf, int buflen);
bool parse_header(char* buf, int buflen, struct HTTP_request* request);
void parse_uri_callback(struct HTTP_request* request);

#endif
