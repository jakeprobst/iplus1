#ifndef _REDIS_H_
#define _REDIS_H_

#include <hiredis/hiredis.h>



typedef struct redis_t {
    redisContext* redis;
    redisReply* reply;
    
} redis_t;




int redis_init(redis_t*, char*, int);
int redis_destroy(redis_t*);

int redis_command(redis_t*, char*, ...);

















#endif /* _REDIS_H_ */
