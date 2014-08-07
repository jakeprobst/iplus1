#ifndef _USER_H_
#define _USER_H_

#include "tree.h"
#include "iplus1.h"

typedef struct user_t {
    iplus1_lang_t* nlang; // native lang
    iplus1_lang_t* tlang; // target lang
    iplus1_tree_t words; // collection of words a user knows
} user_t;

/* initializes a user
 * nl: native language 3 character code
 * tl: target language 3 character code
 * returns -1 on failure */
int user_init(user_t*, char* nl, char* tl);

/* destroys a user */
void user_destroy(user_t*);

/* adds a word to a user's collection
 * word: new word to add
 * returns 1 if the word is already in the user's collection
 * returns 0 on success */
int user_add_word(user_t*, char* word);

/* checks if a word is in a user's collection
 * word: word to check
 * returns true/false */
int user_has_word(user_t*, char* word);

/* parses a list of sentences and adds the words to the user's collection
 * sentences: NULL terminated array of sentences */
void user_parse(user_t*, char** sentences);

















#endif /* USER_H_ */
