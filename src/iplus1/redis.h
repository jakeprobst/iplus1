#ifndef _REDIS_H_
#define _REDIS_H_

#include <hiredis/hiredis.h>

/* This is meant for internal use only */

/* notes on usage:
 * when querying, the result from hiredis gets stored in reply.
 * this value will be freed on the next redis query.
 * so if you need to do nested redis calls you will need a second redis handle.
 * see parse.c for example */


typedef struct redis_t {
    redisContext* redis;
    redisReply* reply;
} redis_t;


// TODO: different error codes depending on the error
/* initializes a redis handle
 * host: host of server to connect to, defaults to "127.0.0.1" if NULL
 * port: port of server to connect to, defaults to 6379 if -1
 * returns -1 on failure */
int redis_init(redis_t*, char* host, int post);

/* destroys a redis handle */
int redis_destroy(redis_t*);

/* executes a redis command
 * use ->reply to get queried data (see hiredis documentation)
 * format: printf-style string
 * ...: arguments for format */
int redis_command(redis_t*, char* format, ...);

















#endif /* _REDIS_H_ */
