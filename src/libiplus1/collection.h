#ifndef _COLLECTION_H_
#define _COLLECTION_H_

#include "sentence.h"
#include "tree.h"

typedef struct iplus1_collection_t {
    iplus1_tree_t sentences;
    
    
} iplus1_collection_t;



int iplus1_collection_init(iplus1_collection_t*);
int iplus1_collection_destroy(iplus1_collection_t*);

int iplus1_collection_add_sentence(iplus1_collection_t*, iplus1_sentence_t*);

iplus1_sentence_t* iplus1_collection_get_sentence_by_id(iplus1_collection_t*, int);








#endif /* _COLLECTION_H_ */
