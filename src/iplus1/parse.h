#ifndef _PARSE_H_
#define _PARSE_H_

#include "user.h"

#define MAX_RESULTS 100

typedef struct result_t {
    int64_t nid;
    int64_t tid;
    //char* word;
} result_t;

result_t* iplus1_parse_user(user_t*);
result_t* iplus1_parse_full(char*, char*, char**);
void iplus1_parse_full_free(result_t*);

























#endif /* _PARSE_H_ */
