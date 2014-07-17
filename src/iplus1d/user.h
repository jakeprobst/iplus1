#ifndef _USER_H_
#define _USER_H_

#include "tree.h"
#include "iplus1.h"

typedef struct user_t {
    int fd;
    iplus1_lang_t* nlang;
    iplus1_lang_t* tlang;
    iplus1_tree_t words;
} user_t;


int user_init(user_t*, int, char*, char*);
int user_destroy(user_t*);

int user_add_word(user_t*, char*);
int user_has_word(user_t*, char*);

int user_tree_free(void*, void*, void*);























#endif /* USER_H_ */
