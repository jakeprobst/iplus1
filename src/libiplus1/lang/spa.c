#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"
#include "tree.h"

typedef struct iplus1_spanish_t {
    struct sb_stemmer* stemmer;
    iplus1_tree_t* stopwords;
} iplus1_spanish_t;

int valid_word(iplus1_spanish_t* spa, char* s)
{
    if (strlen(s) == 0) {
        return 0;
    }
    if (spa->stopwords) {
        if (iplus1_tree_get(spa->stopwords, s)) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* tstr, void* param)
{
    iplus1_spanish_t* spa = (iplus1_spanish_t*)param;
    
    char* str = iplus1_lang_lowercase(tstr);
    if (str == NULL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(spa, split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        goto error;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(spa, split[i])) {
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

void load_stopwords(iplus1_spanish_t* spa)
{
    spa->stopwords = NULL;
    FILE* stopfile = fopen(PREFIX "/share/iplus1/data/spa-stopwords.txt", "r");
    if (stopfile) {
        spa->stopwords = malloc(sizeof(iplus1_tree_t));
        iplus1_tree_init(spa->stopwords, &iplus1_tree_compare_str);
        
        int ch;
        int w = 0;
        char word[64]; // surely no words over 64 characters...right?
        while((ch = fgetc(stopfile)) != EOF) {
            if (ch == '\n') {
                word[w] = '\0';
                char* s = strdup(word);
                iplus1_tree_insert(spa->stopwords, s, s);
                w = 0;
            }
            else {
                word[w] = ch;
                w++;
            }
        }
    }
    fclose(stopfile);
}

int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "spa");
    lang->full_lang = strdup("spanish");
    lang->param = malloc(sizeof(iplus1_spanish_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_spanish_t* spa = (iplus1_spanish_t*)lang->param;
    if ((spa->stemmer = sb_stemmer_new("spa", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find spanish stemmer\n");
        return IPLUS1_FAIL;
    }
    
    load_stopwords(spa);
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_spanish_t* spa = (iplus1_spanish_t*)lang->param;
    
    iplus1_tree_foreach_postorder(spa->stopwords, &iplus1_tree_free_key, NULL);
    iplus1_tree_destroy(spa->stopwords);
    free(spa->stopwords);
    
    sb_stemmer_delete(spa->stemmer);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
