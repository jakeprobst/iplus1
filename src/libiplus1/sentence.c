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
        fprintf(stderr, "iplus1_sentence_init: could not parse '%s'\n", str);
        free(sen->str);
        return IPLUS1_FAIL;
    }
    sen->translations = malloc(sizeof(iplus1_list_t));
    iplus1_list_init(sen->translations);
    
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
    iplus1_list_destroy(sen->translations);
    free(sen->translations);
    
    return IPLUS1_SUCCESS;
}

int iplus1_sentence_add_translation(iplus1_sentence_t* original, iplus1_sentence_t* translated)
{
    iplus1_list_append(original->translations, translated);
    
    return original->translations->length;
}
