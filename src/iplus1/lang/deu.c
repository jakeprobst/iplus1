#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"
#include "tree.h"

typedef struct iplus1_german_t {
    struct sb_stemmer* stemmer;
    iplus1_tree_t* stopwords;
} iplus1_german_t;

int valid_word(iplus1_german_t* deu, char* s)
{
    if (strlen(s) == 0) {
        return 0;
    }
    if (deu->stopwords) {
        if (iplus1_tree_get(deu->stopwords, s)) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* tstr, void* param)
{
    iplus1_german_t* deu = (iplus1_german_t*)param;
    
    char* str = iplus1_lang_lowercase(tstr);
    if (str == NULL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(deu, split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        goto error;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(deu, split[i])) {
            continue;
        }
        
        const sb_symbol* stemmed = sb_stemmer_stem(deu->stemmer, (sb_symbol*)split[i], strlen(split[i]));
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

void load_stopwords(iplus1_german_t* deu)
{
    deu->stopwords = NULL;
    FILE* stopfile = fopen(PREFIX "/share/iplus1/data/deu-stopwords.txt", "r");
    if (stopfile) {
        deu->stopwords = malloc(sizeof(iplus1_tree_t));
        iplus1_tree_init(deu->stopwords, &iplus1_tree_compare_str);
        
        int ch;
        int w = 0;
        char word[64]; // surely no words over 64 characters...right?
        while((ch = fgetc(stopfile)) != EOF) {
            if (ch == '\n') {
                word[w] = '\0';
                char* s = strdup(word);
                iplus1_tree_insert(deu->stopwords, s, s);
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
    strcpy(lang->lang, "deu");
    lang->full_lang = strdup("german");
    lang->param = malloc(sizeof(iplus1_german_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_german_t* deu = (iplus1_german_t*)lang->param;
    if ((deu->stemmer = sb_stemmer_new("deu", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find german stemmer\n");
        return IPLUS1_FAIL;
    }
    
    load_stopwords(deu);
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_german_t* deu = (iplus1_german_t*)lang->param;
    
    iplus1_tree_foreach_postorder(deu->stopwords, &iplus1_tree_free_key, NULL);
    iplus1_tree_destroy(deu->stopwords);
    free(deu->stopwords);
    
    sb_stemmer_delete(deu->stemmer);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
