#include "event.h"

#define BUFLEN 8192
#define REALLOC_SIZE 2048
#define ANON_SIZE 268435456

static char* storage_path = "/mnt/pidrive1/";

/*
 * Read from a socket.
 *
 * @param data - Data to be read from socket.
 * @return - Generic pointer to data reading routine.
 */
void* read_from_socket(void* data)
{
    struct HTTP_socket* sock = (struct HTTP_socket*) data;
    int count = 0;
    char temp[BUFLEN];

    do
    {
        if ((count = read(sock->fd, temp, BUFLEN)) > 0)
        {
            sock->read_buf = realloc(sock->read_buf, sock->read_buf_size + count);
            memcpy(sock->read_buf + sock->read_buf_size, temp, count);
            sock->read_buf_size += count;
        }

    } while(count > 0);

    //Save room on stack
    char method[BUFLEN], uri[BUFLEN], version[BUFLEN], callback[BUFLEN];
    memset(method, '\0', BUFLEN);
    memset(uri, '\0', BUFLEN);
    memset(version, '\0', BUFLEN);
    memset(callback, '\0', BUFLEN);
    struct HTTP_request request = {method, uri, version, callback};

    if (parse_header(sock->read_buf, sock->read_buf_size, &request))
    {
        sock->keep_alive = request.keep_alive;
        handle_request(sock, &request);
    }
    else
        check_read(sock);

    return NULL;
}

/*
 * Write to a socket. If write doesn't fill buffer, add write event to
 * the server. Otherwise, add read event to server if it should be kept
 * alive and close socket if not.
 *
 * @param data - Data to be written to socket.
 * @return - Generic pointer to data writing routine.
 */
void* write_to_socket(void* data)
{
    struct HTTP_socket* sock = (struct HTTP_socket*) data;
    int count = 0;

    do
    {
        //Check for free space at end of buffer
        if (sock->write_buf.pos < sock->write_buf.end_pos)
        {
            count = write(sock->fd, sock->write_buf.data + sock->write_buf.pos, sock->write_buf.end_pos - sock->write_buf.pos);
            sock->write_buf.pos += count;
        }

        //Check if end of buffer has been passed
        if (sock->write_buf.pos >= sock->write_buf.end_pos && sock->data.pos < sock->data.end_pos)
        {
            count = write(sock->fd, sock->data.data + sock->data.pos, sock->data.end_pos - sock->data.pos);
            sock->data.pos += count;
        }

    } while((count != 0) && (sock->write_buf.pos < sock->write_buf.end_pos) && (sock->data.pos < sock->data.end_pos));

    if ((sock->write_buf.pos >= sock->write_buf.end_pos) && (sock->data.pos >= sock->data.end_pos))
    {
        if (sock->keep_alive)
        {
            clear_buffers(sock);
            check_read(sock);
        }
        else
            close_socket(sock);
    }
    else
        check_write(sock);

    return NULL;
}

/*
 * Handle incoming HTTP requests.
 *
 * @param sock - Socket that request is read from.
 * @param request - Request object.
 */
void handle_request(struct HTTP_socket* sock, struct HTTP_request* request)
{
    sock->keep_alive = (strcasecmp(request->version, "HTTP/1.0") == 0);

    if (strcasecmp(request->method, "GET") == 0)
    {
        if (strstr(request->uri, "cgi-bin") == 0)
            serve_static_request(sock, request);
    }
    else
    {
        write_buffer(&sock->data, "HTTP Error %d\n", 501);
        serve_text(sock, 501);
        return;
    }
}

// /*
//  * Change to root path for the server.
//  *
//  * @param path - New path for the server.
//  */
// void change_root_path(char* path)
// {
//     storage_path = path;
// }

/*
 * Method to serve a request for static content.
 *
 * @param sock - Socket that request was read from.
 * @param request - Request object.
 */
void serve_static_request(struct HTTP_socket* sock, struct HTTP_request* request)
{
    char filename[BUFLEN];
    char filetype[BUFLEN];

    strcpy(filename, storage_path);
    strcat(filename, request->uri + 6);

    if (strstr(filename, "index.html"))
    {
        strcpy(filetype, "text/html");
        load_file(sock, filename);
        write_buffer(&sock->write_buf, "HTTP/1.1 200 OK\r\n");
        write_buffer(&sock->write_buf, "Server: PHD Audio Server\r\n");
        write_buffer(&sock->write_buf, "Content-length: %d\r\n", sock->data.size);
        write_buffer(&sock->write_buf, "Content-type: %s\r\n", filetype);
        write_buffer(&sock->write_buf, "\r\n");
        finish_read(sock);
        return;
    }

    struct stat sbuf;
    if (!(stat(filename, &sbuf) == 0 && S_ISREG(sbuf.st_mode)))
    {
        write_buffer(&sock->data, "HTTP Error %d\n", 404);
        serve_text(sock, 404);
        return;
    }

    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".mp3"))
        strcpy(filetype, "audio/mpeg");
    else
        strcpy(filetype, "text/plain");

    load_file(sock, filename);
    write_buffer(&sock->write_buf, "HTTP/1.1 200 OK\r\n");
    write_buffer(&sock->write_buf, "Server: PHD Audio Server\r\n");
    write_buffer(&sock->write_buf, "Content-length %d\r\n", sock->data.size);
    write_buffer(&sock->write_buf, "Content-type: %s\r\n", filetype);
    write_buffer(&sock->write_buf, "\r\n");
    finish_read(sock);
}

/*
 * Serve a plain text file.
 *
 * @param sock - Socket for request.
 * @param err - HTTP error code if response is not OK.
 */
void serve_text(struct HTTP_socket* sock, int err)
{
    if (err)
        write_buffer(&sock->write_buf, "HTTP/1.1 %d\r\n", err);
    else
        write_buffer(&sock->write_buf, "HTTP/1.1 200 OK\r\n");

    write_buffer(&sock->write_buf, "Server: PHD Audio Server\r\n");
    write_buffer(&sock->write_buf, "Content-length %d\r\n", sock->data.pos);
    write_buffer(&sock->write_buf, "Content-type: text/plain\r\n");
    write_buffer(&sock->write_buf, "\r\n");
    finish_read(sock);
}

/*
 * Load file from socket and map to memory.
 *
 * @param sock - Socket to load file from.
 * @param fname - Name of file to load.
 * @return - -1 on error, 0 on success, 2 on unavilable/unpermitted.
 */
int load_file(struct HTTP_socket* sock, char* fname)
{
    struct stat stat_block;
    int fd = open(fname, O_RDONLY);

    if (fd < 0)
    {
        if (fd == EEXIST || fd == EFAULT || fd == ENAMETOOLONG || fd == ETXTBSY)
            return -1;

        if (fd == EACCES || fd == EPERM)
            return 2;

        perror("Error opening file for mmaping!\n");
        return -1;
    }

    if (fstat(fd, &stat_block) != 0)
    {
        perror("Error statting file for mmaping!\n");
        close(fd);
        return -1;
    }

    void* mmaped_file;
    if ((mmaped_file = mmap(NULL, stat_block.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == ((void*) -1))
    {
        fprintf(stderr, "Error mmaping file!: %s\n", strerror(errno));
        exit(0);
    }

    if (mmaped_file == MAP_FAILED)
    {
        perror("Error mapping file!\n");
        close(fd);
        return -1;
    }

    sock->is_mmaped = true;
    sock->data.data = mmaped_file;
    sock->data.size = stat_block.st_size;
    sock->data.pos = sock->data.size;
    return 0;
}

/*
 * Method to write to a buffer.
 *
 * @param buf - Buffer to be written to.
 * @param str - Message to write in buffer.
 */
void write_buffer(struct buffer* buf, char* str, ...)
{
    va_list arg_ptr;
    int x, free_space;
    do
    {
        free_space = buf->size - buf->pos;
        va_start(arg_ptr, str);
        x = vsnprintf(buf->data + buf->pos, free_space, str, arg_ptr);
        va_end(arg_ptr);

        if (x >= free_space)
        {
            buf->data = realloc(buf->data, buf->size + REALLOC_SIZE);
            buf->size += REALLOC_SIZE;
        }
        else
            buf->pos += x;

    } while(x >= free_space);
}

/*
 * Called when finished reading a socket.
 *
 * @param sock - Socket that was read/
 */
void finish_read(struct HTTP_socket* sock)
{
    sock->write_buf.end_pos = sock->write_buf.pos;
    sock->data.end_pos = sock->data.pos;
    sock->write_buf.pos = 0;
    sock->data.pos = 0;
    check_write(sock);
}
