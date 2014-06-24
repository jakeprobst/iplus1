#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "input.h"

// mashed my keyboard till a legit port came up
#define PORT "51945"
#define MAX_EVENTS 16

// this can probably be smaller, I think the sentence spec says 
// a single sentence can be at most 512 chars...
// there is this one russian sentence that is like 1000 characters so...
#define BUFFER_SIZE 1024*2


void printbuf(char* buf, int size)
{
    int l, i;
    for(l = 0; l < (size/16)+1; l++) {
        printf("|");
        for(i = 0; i < 16; i++) {
            if ((l*16 + i) > size) {
                printf("   ");
            }
            else {
                printf("%.2X ", buf[(l*16 + i)] & 0xFF);
            }
        }
        printf("|");
        for(i = 0; i < 16; i++) {
            if ((l*16 + i) > size) {
                printf(" ");
            }
            else {
                if (0x20 < buf[(l*16 + i)] && buf[(l*16 + i)] < 0x7F) {
                    printf("%c", buf[(l*16 + i)]);
                }
                else {
                    printf(".");
                }
            }
        }
        printf("|\n");
    }
    
}

int set_socket_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        fprintf(stderr, "fnctl: %s\n", strerror(errno));
        return -1;
    }
    
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        fprintf(stderr, "fnctl: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

int make_bind_socket()
{
    struct addrinfo hints;
    struct addrinfo *result;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(NULL, PORT, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errno));
        return -1;
    }
    
    int fd;
    struct addrinfo *r;
    for(r = result; r != NULL; r = r->ai_next) {
        if ((fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1) {
            continue;
        }
        
        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 
        
        if (bind(fd, r->ai_addr, r->ai_addrlen) == 0) {
            break;
        }
        close(fd);
    }
    if (r == NULL) {
        fprintf(stderr, "could not bind\n");
        return -1;
    }
    
    freeaddrinfo(result);
    if (set_socket_nonblocking(fd) == -1) {
        close(fd);
        return -1;
    }
    return fd;
}

int input_init(input_t* input)
{
    input->running = 1;
    input->listen_fd = make_bind_socket();
    if (input->listen_fd == -1) {
        fprintf(stderr, "input_init: couldn't init socket\n");
        return -1;
    }
    
    if (listen(input->listen_fd, SOMAXCONN) == -1) {
        fprintf(stderr, "input_init: listen: %s\n", strerror(errno));
        return -1;
    }
    
    if ((input->epoll_fd = epoll_create1(0)) == -1) {
        fprintf(stderr, "input_init: epoll_create1: %s\n", strerror(errno));
        return -1;
    }
    
    struct epoll_event event;
    event.data.fd = input->listen_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(input->epoll_fd, EPOLL_CTL_ADD, input->listen_fd, &event) == -1) {
        fprintf(stderr, "input_init: epoll_ctl: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}


int input_destroy(input_t* input)
{
    close(input->listen_fd);
    close(input->epoll_fd);
    
    return 0;
}

int command_destroy(command_t* cmd)
{
    if (cmd->type == CMD_SENTENCE) {
        free(cmd->sen.sen);
    }
    if (cmd->type == CMD_ANKICARD) {
        free(cmd->anki.text);
    }
    
    return 0;
}


int input_accept_connection(input_t* input)
{
    for(;;) {
        struct sockaddr addr;
        socklen_t len;
        
        int new_fd = accept(input->listen_fd, &addr, &len);
        if (new_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            else {
                fprintf(stderr, "accept: %s\n", strerror(errno));
                break;
            }
        }
        
        if (set_socket_nonblocking(new_fd) == -1) {
            close(new_fd);
            continue;
        }
        
        struct epoll_event event;
        event.data.fd = new_fd;
        event.events = EPOLLIN;
        
        if (epoll_ctl(input->epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1) {
            fprintf(stderr, "epoll_ctl: %s\n", strerror(errno));
            continue;
        }
    }
    return 0;
}

int parse_line(char* in_line, command_t* cmd)
{
    char* line = strdup(in_line);
    char* pos;
    char* s = strtok_r(line, "\t", &pos);

    if (s == NULL) {
        goto error;
    }
    if (strncmp(s, "SEN", 3) == 0) {
        cmd->type = CMD_SENTENCE;
        cmd->sen.sen = NULL;
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        cmd->sen.id = atoi(s);
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        strncpy(cmd->sen.lang, s, 4);
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        cmd->sen.sen = strdup(s);
    }
    else if (strncmp(s, "LINK", 4) == 0) {
        cmd->type = CMD_LINK;
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        strncpy(cmd->link.lang, s, 4);
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        cmd->link.id = atoi(s);
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        strncpy(cmd->link.tlang, s, 4);
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        cmd->link.tid = atoi(s);
    }
    else if (strncmp(s, "ANKIBEGIN", 9) == 0) {
        cmd->type = CMD_ANKIBEGIN;
        
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        strncpy(cmd->ankibegin.nlang, s, 4);

        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        strncpy(cmd->ankibegin.tlang, s, 4);
    }
    else if (strncmp(s, "ANKICARD", 8) == 0) {
        if ((s = strtok_r(NULL, "\t", &pos)) == NULL) {
            goto error;
        }
        cmd->anki.text = strdup(s);
    }
    else if (strncmp(s, "ANKIEND", 7) == 0) {
        cmd->type = CMD_ANKIEND;
    }
    else if (strncmp(s, "QUIT", 4) == 0) {
        cmd->type = CMD_QUIT;
    }
    
    free(line);
    return strlen(in_line)+1;
    
error:
    free(line);
    return -1;
}


int input_parse_data(input_t* input, int fd)
{
    char buf[BUFFER_SIZE];
    command_t cmd;
    
    int size = 0;
    for(;;) {
        memset(buf, 0, BUFFER_SIZE);
        size = recv(fd, buf, BUFFER_SIZE, MSG_PEEK);
        if (size <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                errno = 0;
                continue;
            }
            close(fd);
            return 0;
        }
        
        int z = parse_line(buf, &cmd);
        if (z == -1) {
            fprintf(stderr, "invalid data: '%s'\n", buf);
            z = strlen(buf) + 1;
        }
        else {
            if (cmd.type == CMD_QUIT) {
                input->running = 0;
                return 1;
            }
            cmd.fd = fd;
            input->callback(&cmd, input->callback_param);
        }
        
        command_destroy(&cmd);
        recv(fd, buf, z, 0);
    }
    
    return 0;
}






int input_loop(input_t* input)
{
    struct epoll_event events[MAX_EVENTS];
    
    int i, n;
    while(input->running) {
        n = epoll_wait(input->epoll_fd, events, MAX_EVENTS, -1);
        for(i = 0; i < n; i++) {
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                fprintf(stderr, "epoll error\n");
                epoll_ctl(input->epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                close(events[i].data.fd);
            }
            else if (events[i].data.fd == input->listen_fd) {
                input_accept_connection(input);
            }
            else {
                input_parse_data(input, events[i].data.fd);
            }
        }
    }
    
    return 0;
}





int input_set_callback(input_t* input, int (*func)(command_t*, void*), void* param)
{
    input->callback = func;
    input->callback_param = param;
    
    return 0;
}




















