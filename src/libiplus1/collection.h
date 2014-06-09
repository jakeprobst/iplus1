#ifndef _COLLECTION_H_
#define _COLLECTION_H_

#include "sentence.h"
#include "word.h"
#include "tree.h"
#include "list.h"

typedef struct iplus1_collection_t {
    iplus1_tree_t sentences; // iplus1_sentence_t
    iplus1_tree_t words; // iplus1_word_t
} iplus1_collection_t;



int iplus1_collection_init(iplus1_collection_t*);
int iplus1_collection_destroy(iplus1_collection_t*);

int iplus1_collection_add_sentence(iplus1_collection_t*, iplus1_sentence_t*);

iplus1_sentence_t* iplus1_collection_get_sentence_by_id(iplus1_collection_t*, int);
iplus1_list_t* iplus1_collection_get_sentences_by_word(iplus1_collection_t*, char*);


#endif /* _COLLECTION_H_ */
