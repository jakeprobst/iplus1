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

void usage(char* name)
{
    printf("usage:\n");
    printf("initialize:\n");
    printf("\t%s init\n", name);
    printf("send deck to iplus1d:\n");
    printf("\t%s native-lang target-lang anki-deck\n", name);
}

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


int initialize_redis(int fd)
{
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
    
    return 0;
}

int send_anki_deck(int fd, char* native_lang, char* target_lang, char* anki_path)
{
    
    return 0;
}

int get_new_sentences(int fd)
{
    return 0;
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        usage(argv[0]);
        return 0;
    }
    
    int fd = connect_to_server();
    if (fd == -1) {
        fprintf(stderr, "couldn`t connect to server\n");
        return -1;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "init") == 0) {
            initialize_redis(fd);
        }
        else {
            usage(argv[0]);
        }
    }
    
    if (argc == 4) {
        char* native_lang = argv[1];
        char* target_lang = argv[2];
        char* anki_path = argv[3];
        send_anki_deck(fd, native_lang, target_lang, anki_path);
        
        get_new_sentences(fd);
    }
    


    
    close(fd);
    return 0;
}







