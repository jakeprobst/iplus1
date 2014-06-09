#ifndef _SENTENCE_H_
#define _SENTENCE_H_

#include "iplus1.h"
#include "lang.h"
#include "list.h"

typedef struct iplus1_sentence_t {
    int id;
    char lang[4];
    char* str;
    char** words;
    iplus1_list_t* translations; // iplus1_sentence_t*
} iplus1_sentence_t;





int iplus1_sentence_init(iplus1_sentence_t*, iplus1_lang_t*, char*);
int iplus1_sentence_destroy(iplus1_sentence_t*);

int iplus1_sentence_add_translation(iplus1_sentence_t*, iplus1_sentence_t*);







#endif /* _SENTENCE_H_ */
