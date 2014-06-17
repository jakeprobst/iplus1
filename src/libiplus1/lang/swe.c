#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"
#include "tree.h"

typedef struct iplus1_swedish_t {
    struct sb_stemmer* stemmer;
    iplus1_tree_t* stopwords;
} iplus1_swedish_t;

int valid_word(iplus1_swedish_t* swe, char* s)
{
    if (strlen(s) == 0) {
        return 0;
    }
    if (swe->stopwords) {
        if (iplus1_tree_get(swe->stopwords, s)) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* tstr, void* param)
{
    iplus1_swedish_t* swe = (iplus1_swedish_t*)param;
    
    char* str = iplus1_lang_lowercase(tstr);
    if (str == NULL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(swe, split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        goto error;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(swe, split[i])) {
            continue;
        }
        
        const sb_symbol* stemmed = sb_stemmer_stem(swe->stemmer, (sb_symbol*)split[i], strlen(split[i]));
        output[output_index] = malloc(strlen((char*)stemmed)+1);
        if (output[output_index] == NULL) {
            int n;
            for (n = 0; n < output_index; n++) {
                free(output[n]);
            }
            free(output);
            output = NULL;
            goto error;
        }
        strcpy(output[output_index], (char*)stemmed);
        output_index++;
    }
    
error:
    for(i = 0; split[i] != NULL; i++) {
        free(split[i]);
    }
    free(split);
    free(str);
    
    return output;
}

void load_stopwords(iplus1_swedish_t* swe)
{
    swe->stopwords = NULL;
    FILE* stopfile = fopen("data/swe-stopwords.txt", "r");
    if (stopfile) {
        swe->stopwords = malloc(sizeof(iplus1_tree_t));
        iplus1_tree_init(swe->stopwords, &iplus1_tree_compare_str);
        
        int ch;
        int w = 0;
        char word[64]; // surely no words over 64 characters...right?
        while((ch = fgetc(stopfile)) != EOF) {
            if (ch == '\n') {
                word[w] = '\0';
                char* s = strdup(word);
                iplus1_tree_insert(swe->stopwords, s, s);
                w = 0;
            }
            else {
                word[w] = ch;
                w++;
            }
        }
    }
}

int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "swe");
    lang->full_lang = strdup("swedish");
    lang->param = malloc(sizeof(iplus1_swedish_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_swedish_t* swe = (iplus1_swedish_t*)lang->param;
    if ((swe->stemmer = sb_stemmer_new("swe", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find swedish stemmer\n");
        return IPLUS1_FAIL;
    }
    
    load_stopwords(swe);
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_swedish_t* swe = (iplus1_swedish_t*)lang->param;
    
    iplus1_tree_foreach_postorder(swe->stopwords, &iplus1_tree_free_key, NULL);
    iplus1_tree_destroy(swe->stopwords);
    free(swe->stopwords);
    
    sb_stemmer_delete(swe->stemmer);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
