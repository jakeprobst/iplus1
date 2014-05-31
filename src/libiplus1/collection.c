#include <stdlib.h>
#include "collection.h"



int _compare_int(void* a, void* b)
{
    int* at = a;
    int* bt = b;
    return *at - *bt;
}

int iplus1_collection_init(iplus1_collection_t* col)
{
    iplus1_tree_init(&col->sentences, &_compare_int);
    
    return IPLUS1_SUCCESS;
}

int _free_sentences(void* p, void* param)
{
    iplus1_sentence_t* sen = (iplus1_sentence_t*)p;
    iplus1_sentence_destroy(sen);
    free(p);
    return IPLUS1_SUCCESS;
}

int iplus1_collection_destroy(iplus1_collection_t* col)
{
    iplus1_tree_foreach_postorder(&col->sentences, &_free_sentences, NULL);
    iplus1_tree_destroy(&col->sentences);
    return IPLUS1_SUCCESS;
}


int iplus1_collection_add_sentence(iplus1_collection_t* col, iplus1_sentence_t* sen)
{
    iplus1_tree_insert(&col->sentences, &sen->id, sen);
    return IPLUS1_SUCCESS;
}

iplus1_sentence_t* iplus1_collection_get_sentence_by_id(iplus1_collection_t* col, int id)
{
    return iplus1_tree_get(&col->sentences, &id);
}














