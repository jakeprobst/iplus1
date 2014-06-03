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
    if (sen->words == NULL) {
        free(sen->str);
        return IPLUS1_FAIL;
    }
    sen->trans_count = 0;
    sen->translations = calloc(sizeof(iplus1_sentence_t*), 1);
    
    return IPLUS1_SUCCESS;
}

int iplus1_sentence_destroy(iplus1_sentence_t* sen)
{
    if (sen == NULL)
        return IPLUS1_FAIL;
    free(sen->str);
    int i;
    if (sen->words) {
        for(i = 0; sen->words[i] != NULL; i++) {
            free(sen->words[i]);
        }
        free(sen->words);
    }
    free(sen->translations);
    
    return IPLUS1_SUCCESS;
}

int iplus1_sentence_add_translation(iplus1_sentence_t* original, iplus1_sentence_t* translated)
{
    original->trans_count++;
    
    original->translations = realloc(original->translations, (original->trans_count + 1) *sizeof(iplus1_sentence_t*));
    if (original->translations == NULL)
        return IPLUS1_FAIL;
    original->translations[original->trans_count-1] = translated;
    original->translations[original->trans_count] = NULL;
    
    return original->trans_count;
}
