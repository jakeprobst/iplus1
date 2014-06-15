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
    
    char* str = "SEN\t123\teng\thuehuehuehu\0SEN\t124\teng\tblahblahblah\0";
    printf("%s (%ld, %ld)\n", str, strlen(str), (strlen(str)*2) +3);
    send(fd, str, (strlen(str)*2) + 3, 0);
    
    send(fd, str, (strlen(str)*2) + 3, 0);

    /*char* pos;
    char* s = strdup("a s d f");
    char* z = strtok_r(s, " ", &pos);
    printf("z: %s\n", z);
    z = strtok_r(NULL, " ", &pos);
    printf("z: %s\n", z);
    z = strtok_r(NULL, " ", &pos);
    printf("z: %s\n", z);
    z = strtok_r(NULL, " ", &pos);
    printf("z: %s\n", z);
    printf("s: %s\n", s);*/



    close(fd);
    return 0;
}
