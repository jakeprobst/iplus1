#include <stdio.h>
#include <stdarg.h>

#include "redis.h"

#define BUFFER_SIZE 1024*2
#define PORT 6379


int redis_init(redis_t* redis, char* thost, int tport)
{
    char* host = "127.0.0.1";
    if (thost != NULL) {
        host = thost;
    }
    
    int port = PORT;
    if (tport != -1) {
        port = tport;
    }
    
    struct timeval timeout = {1, 500000}; // 1.5s
    redis->redis = redisConnectWithTimeout(host, port, timeout);
    if (redis->redis == NULL) {
        fprintf(stderr, "could not alloc redis\n");
        return -1;
    }
    if (redis->redis->err) {
        fprintf(stderr, "redis error: %s\n", redis->redis->errstr);
        redisFree(redis->redis);
        return -1;
    }
    
    redis->reply = NULL;
    
    return 0;
}

int redis_destroy(redis_t* redis)
{
    if (redis->reply) {
        freeReplyObject(redis->reply);
    }
    redisFree(redis->redis);
    return 0;
}

int redis_command(redis_t* redis, char* format, ...)
{
    if (redis->reply) {
        freeReplyObject(redis->reply);
        redis->reply = NULL;
    }
    
    va_list arg;
    va_start(arg, format);
    char buf[BUFFER_SIZE];
    vsnprintf(buf, BUFFER_SIZE, format, arg);
    
    redis->reply = redisCommand(redis->redis, buf);
    
    va_end(arg);
    return 0;
}






