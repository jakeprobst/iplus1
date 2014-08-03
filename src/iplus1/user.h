#ifndef _USER_H_
#define _USER_H_

#include "tree.h"
#include "iplus1.h"

typedef struct user_t {
    iplus1_lang_t* nlang;
    iplus1_lang_t* tlang;
    iplus1_tree_t words;
} user_t;


int user_init(user_t*, char*, char*);
int user_destroy(user_t*);

int user_add_word(user_t*, char*);
int user_has_word(user_t*, char*);

int user_parse(user_t*, char**);

















#endif /* USER_H_ */
