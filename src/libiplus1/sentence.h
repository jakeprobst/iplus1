#ifndef _SENTENCE_H_
#define _SENTENCE_H_

#include "iplus1.h"
#include "lang.h"

typedef struct iplus1_sentence_t {
    int id;
    char lang[4];
    char* str;
    char** words;
    
    int trans_count;
    struct iplus1_sentence_t** translations; // NULL terminated
} iplus1_sentence_t;





int iplus1_sentence_init(iplus1_sentence_t*, iplus1_lang_t*, char*);
int iplus1_sentence_destroy(iplus1_sentence_t*);

int iplus1_sentence_add_translation(iplus1_sentence_t*, iplus1_sentence_t*);







#endif /* _SENTENCE_H_ */
