#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collection.h"



int _compare_int(void* a, void* b)
{
    int* at = a;
    int* bt = b;
    return *at - *bt;
}

int _compare_str(void* a, void* b)
{
    char* at = a;
    char* bt = b;
    return strcmp(at, bt);
}

int iplus1_collection_init(iplus1_collection_t* col)
{
    iplus1_tree_init(&col->sentences, &_compare_int);
    iplus1_tree_init(&col->words, &_compare_str);
    
    return IPLUS1_SUCCESS;
}

int _free_sentences(void* p, void* param)
{
    iplus1_sentence_t* sen = (iplus1_sentence_t*)p;
    iplus1_sentence_destroy(sen);
    free(p);
    return IPLUS1_SUCCESS;
}

int _free_words(void* p, void* param)
{
    iplus1_word_t* word = (iplus1_word_t*)p;
    iplus1_word_destroy(word);
    free(p);
    return IPLUS1_SUCCESS;
}

int iplus1_collection_destroy(iplus1_collection_t* col)
{
    iplus1_tree_foreach_postorder(&col->sentences, &_free_sentences, NULL);
    iplus1_tree_destroy(&col->sentences);
    
    iplus1_tree_foreach_postorder(&col->words, &_free_words, NULL);
    iplus1_tree_destroy(&col->words);
    return IPLUS1_SUCCESS;
}


int iplus1_collection_add_sentence(iplus1_collection_t* col, iplus1_sentence_t* sen)
{
    iplus1_tree_insert(&col->sentences, &sen->id, sen);
    
    int i;
    for(i = 0; sen->words[i] != NULL; i++) {
        iplus1_word_t* word = iplus1_tree_get(&col->words, sen->words[i]);
        if (word == NULL) {
            word = malloc(sizeof(iplus1_word_t));
            iplus1_word_init(word, sen->words[i]);
            iplus1_tree_insert(&col->words, word->str, word);
        }
        iplus1_word_add_sentence(word, sen);
    }
    
    return IPLUS1_SUCCESS;
}

iplus1_sentence_t* iplus1_collection_get_sentence_by_id(iplus1_collection_t* col, int id)
{
    return iplus1_tree_get(&col->sentences, &id);
}

iplus1_sentence_t** iplus1_collection_get_sentences_by_word(iplus1_collection_t* col, char* str)
{
    iplus1_word_t* word = iplus1_tree_get(&col->words, str);
    return word->sentences;
}













