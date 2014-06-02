#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"


typedef struct iplus1_spanish_t {
    struct sb_stemmer* stemmer;
    
} iplus1_spanish_t;

int valid_word(char* s)
{
    char* punc = " -!?.";
    
    int i;
    for(i = 0; punc[i] != '\0'; i++) {
        if (s[0] == punc[i]) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* str, void* param)
{
    iplus1_spanish_t* spa = (iplus1_spanish_t*)param;
    
    if (iplus1_lang_lowercase(str) == IPLUS1_FAIL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        return NULL;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(split[i])) {
            continue;
        }
        
        const sb_symbol* stemmed = sb_stemmer_stem(spa->stemmer, (sb_symbol*)split[i], strlen(split[i]));
        output[output_index] = malloc(strlen((char*)stemmed)+1);
        if (output[output_index] == NULL) {
            int n;
            for (n = 0; n < output_index; n++) {
                free(output[n]);
            }
            free(output);
            return NULL;
        }
        strcpy(output[output_index], (char*)stemmed);
        output_index++;
    }
    
    for(i = 0; split[i] != NULL; i++) {
        free(split[i]);
    }
    free(split);
    
    return output;
}

int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "spa");
    lang->param = malloc(sizeof(iplus1_spanish_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_spanish_t* spa = (iplus1_spanish_t*)lang->param;
    if ((spa->stemmer = sb_stemmer_new("spa", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find spanish stemmer\n");
        return IPLUS1_FAIL;
    }
    
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_spanish_t* spa = (iplus1_spanish_t*)lang->param;
    
    sb_stemmer_delete(spa->stemmer);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
