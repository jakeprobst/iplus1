#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

#include "iplus1.h"


#define PORT "51945"


char* readline(FILE* fd)
{
    int len = 0;
    int start = ftell(fd);
    
    while(1) {
        int c = fgetc(fd);
        if (c == '\n' || c == EOF) {
            break;
        }
        len++;
    }
    
    if (len == 0) {
        return NULL;
    }
    
    char* buf = malloc(len+1);
    fseek(fd, start, SEEK_SET);
    fread(buf, len, 1, fd);
    buf[len] = '\0';
    
    fgetc(fd); // get that linebreak out of there
        
    return buf;
}



int connect_to_server()
{
    struct addrinfo hint, *res;
    
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    
    getaddrinfo(NULL, PORT, &hint, &res);
    
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == -1) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }
    
    if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "connect: %s\n", strerror(errno));
        return -1;
    }
    
    freeaddrinfo(res);
    return fd;
}


int main(int argc, char** argv)
{
    int fd = connect_to_server();
    if (fd == -1) {
        fprintf(stderr, "couldn`t connect to server\n");
        return -1;
    }
    
    FILE* sentences = fopen("sentences.csv", "r");
    char buf[2048];
    char* line;
    while((line = readline(sentences)) != NULL) {
        int l = snprintf(buf, 2048, "SEN\t%s", line) + 1;
        send(fd, buf, l, 0);
        free(line);
    }
    fclose(sentences);
    
    FILE* links = fopen("data/links.csv", "r");
    while((line = readline(links)) != NULL) {
        int l = snprintf(buf, 2048, "LINK\t%s", line) + 1;
        send(fd, buf, l, 0);
        free(line);
    }

    int l = snprintf(buf, 2048, "QUIT") + 1;
    send(fd, buf, l, 0);

    
    close(fd);
    return 0;
}







