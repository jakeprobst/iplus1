#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "iplus1.h"
#include "word.h"







int iplus1_word_init(iplus1_word_t* word, char* str)
{
    word->str = strdup(str);
    word->sen_count = 0;
    //word->sentences = calloc(sizeof(iplus1_sentence_t*), 1);
    word->sentences = malloc(sizeof(iplus1_sentence_t*));
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
    word->sen_count++;
    
    iplus1_list_append(word->sentences, sen);
    
    /*word->sentences = realloc(word->sentences, (word->sen_count + 1)*sizeof(iplus1_sentence_t*));
    if (word->sentences == NULL)
        return IPLUS1_FAIL;
    word->sentences[word->sen_count-1] = sen;
    word->sentences[word->sen_count] = NULL;*/
    
    return word->sen_count;
}




