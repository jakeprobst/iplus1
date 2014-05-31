#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sentence.h"





int iplus1_sentence_init(iplus1_sentence_t* sen, iplus1_lang_t* lang, char* str)
{
    if (sen == NULL || lang == NULL || str == NULL) {
        fprintf(stderr, "iplus1_sentence_init: invalid argument(s)\n");
        return IPLUS1_FAIL;
    }
    
    strncpy(sen->lang, lang->lang, 4);
    sen->str = strdup(str);
    sen->words = iplus1_lang_parse(lang, str);
    sen->trans_count = 1; //NULL terminated
    sen->translations = NULL;
    
    return IPLUS1_SUCCESS;
}

int iplus1_sentence_destroy(iplus1_sentence_t* sen)
{
    free(sen->str);
    int i;
    for(i = 0; sen->words[i] != NULL; i++) {
        free(sen->words[i]);
    }
    free(sen->words);
    free(sen->translations);
    
    return IPLUS1_SUCCESS;
}

int iplus1_sentence_add_translation(iplus1_sentence_t* original, iplus1_sentence_t* translated)
{
    original->trans_count++;
    
    original->translations = realloc(original->translations, original->trans_count*sizeof(iplus1_sentence_t*));
    if (original->translations == NULL)
        return IPLUS1_FAIL;
    original->translations[original->trans_count-2] = translated;
    original->translations[original->trans_count-1] = NULL;
    
    return original->trans_count;
}
