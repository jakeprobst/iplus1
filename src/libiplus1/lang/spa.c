#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"


typedef struct iplus1_spanish_t {
    struct sb_stemmer* stemmer;
    
} iplus1_spanish_t;





char** parse(char* str, void* param)
{
    iplus1_english_t* spa = (iplus1_spanish_t*)param;
    
    char** output;
    char* position;
    char* s;
    
    int count = 1; // +1 because NULL terminated
    int i;
    for(i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            count++;
        }
    }
    
    output = calloc(sizeof(char*), count);
        
    char* tmp;
    for(tmp = str, i = 0; (s = strtok_r(tmp, " ", &position)) != NULL; tmp = NULL, i++) {
        const sb_symbol* stemmed = sb_stemmer_stem(spa->stemmer, (sb_symbol*)s, strlen(s));
                
        output[i] = malloc(strlen((char*)stemmed)+1);
        strcpy(output[i], (char*)stemmed);
    }
    
    return output;
}

int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "spa");
    lang->param = malloc(sizeof(iplus1_spanish_t));
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
