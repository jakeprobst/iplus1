#ifndef _PARSE_H_
#define _PARSE_H_

#include "user.h"

#define MAX_RESULTS 100

typedef struct result_t {
    int64_t nid; // id in the database for the full native sentence
    int64_t tid; // id in the database for the full target sentence
    //char* word;
} result_t;

/* compare the user against the database to find sentences with one new word
 * returns NULL on failure
 * returns a NULL terminated array of results (free with iplus1_parse_full_free) */
result_t* iplus1_parse_user(user_t*);

/* compare a list of strings against the database to find sentences with one new word
 * nl: native language 3 character code
 * tl: target language 3 character code
 * sentences: NULL terminated array of sentences
 * returns a NULL terminated array of results (free with iplus1_parse_full_free) */
result_t* iplus1_parse_full(char* nl, char* tl, char** sentences);

/* frees NULL terminated arrays of results */
void iplus1_parse_full_free(result_t*);

























#endif /* _PARSE_H_ */
