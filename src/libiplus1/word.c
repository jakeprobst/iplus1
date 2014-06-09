#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "iplus1.h"
#include "word.h"



int iplus1_word_init(iplus1_word_t* word, char* str)
{
    word->str = strdup(str);
    word->sentences = malloc(sizeof(iplus1_list_t));
    iplus1_list_init(word->sentences);
    
    return IPLUS1_SUCCESS;
}

int iplus1_word_destroy(iplus1_word_t* word)
{
    free(word->str);
    iplus1_list_destroy(word->sentences);
    free(word->sentences);
    
    return IPLUS1_SUCCESS;
}

int iplus1_word_add_sentence(iplus1_word_t* word, iplus1_sentence_t* sen)
{
    iplus1_list_append(word->sentences, sen);
    
    return word->sentences->length;
}




