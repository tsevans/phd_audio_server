#include "parser.h"

#define BUFLEN 4096 //4 Kilobytes

/*
 * Check for end of buffer, denoted \r\n\r\n.
 *
 * @param buf - Buffer to read.
 * @param buflen - Length of buffer.
 * @return - True if end of buffer, false otherwise.
 */
bool end_of_buffer(char* buf, int buflen)
{
    int x;
    for (x = 0; x < buflen - 3; x++)
    {
        if (buf[x] == '\r' && buf[x+1] == '\n' && buf[x+2] == '\r' && buf[x+3] == '\n')
	    return true;
    }

    return false;
}

/*
 * Parses buffer into header of HTTP request.
 *
 * @param buf - Buffer to read.
 * @param buflen - Length of buffer.
 * @param request - HTTP request to be filled.
 * @return - True if buffer was parsed into HTTP request, false otherwise.
 */
bool parse_header(char* buf, int buflen, struct HTTP_request* request)
{
    if (!end_of_buffer(buf, buflen))
    {
        printf("Parser hit end of buffer.\n");
        return false;
    }

    char tmp[BUFLEN];
    char* str = buf;
    int str_len = -1, x;
    for (x = 0; x < buflen; x++)
    {
        if (str[x] == '\n')
        {
            str_len = x;
            break;
        }
    }

    if (str_len == -1)
    {
        return false;
    }

    if (str_len > BUFLEN)
        str_len = BUFLEN;

    strncpy(tmp, str, str_len);
    sscanf(tmp, "%s %s %s\n", request->method, request->uri, request->version);
    parse_uri_callback(request);

    return true;
}

/*
 * Parse the callback from the URI.
 *
 * @param request - HTTP request to be parsed.
 */
void parse_uri_callback(struct HTTP_request* request)
{
    request->callback[0] = '\0';
    int x;
    for (x = 0; request->uri[x] != '\0' && x < BUFLEN; x++)
    {
        if (request->uri[x] == '?')
            break;
    }

    if (x > BUFLEN || request->uri[x] != '?')
        return;

    request->uri[x] = '\0';
    x++;

    for (; x != '\0' && x < BUFLEN; x++)
    {
        if (strncmp("callback=", request->uri + x, 9) == 0)
        {
            x += 9;
            int y;
            for (y = 0; y < BUFLEN; y++)
            {
                if (!(isalnum(request->uri[x]) || request->uri[x] == '_' || request->uri[x] == '.'))
                {
                    request->callback[y] = '\0';
                    return;
                }
                request->callback[y] = request->uri[x];
                x++;
            }
        }

        while (x < BUFLEN && request->uri[x] != '&' && request->uri[x] != '\0')
            x++;
    }
}
