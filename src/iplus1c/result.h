#ifndef _RESULT_H_
#define _RESULT_H_

#include "list.h"


typedef struct search_result_t {
    char* original;
    char* translated;
    iplus1_list_t* new_words; // iplus1_word_t*
} search_result_t;

int search_result_init(search_result_t*);
int search_result_destroy(search_result_t*);




























#endif /* _RESULT_H_ */
