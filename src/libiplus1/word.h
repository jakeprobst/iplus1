#ifndef _WORD_H_
#define _WORD_H_

#include "sentence.h"
#include "list.h"

typedef struct iplus1_word_t {
    char* str;
    iplus1_list_t* sentences; // iplus1_sentence_t*
} iplus1_word_t;


int iplus1_word_init(iplus1_word_t*, char*);
int iplus1_word_destroy(iplus1_word_t*);

int iplus1_word_add_sentence(iplus1_word_t*, iplus1_sentence_t*);













#endif /* _WORD_H_ */
