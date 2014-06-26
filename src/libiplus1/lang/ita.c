#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"
#include "tree.h"

typedef struct iplus1_italian_t {
    struct sb_stemmer* stemmer;
    iplus1_tree_t* stopwords;
} iplus1_italian_t;

int valid_word(iplus1_italian_t* ita, char* s)
{
    if (strlen(s) == 0) {
        return 0;
    }
    if (ita->stopwords) {
        if (iplus1_tree_get(ita->stopwords, s)) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* tstr, void* param)
{
    iplus1_italian_t* ita = (iplus1_italian_t*)param;
    
    char* str = iplus1_lang_lowercase(tstr);
    if (str == NULL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(ita, split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        goto error;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(ita, split[i])) {
            continue;
        }
        
        const sb_symbol* stemmed = sb_stemmer_stem(ita->stemmer, (sb_symbol*)split[i], strlen(split[i]));
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

void load_stopwords(iplus1_italian_t* ita)
{
    ita->stopwords = NULL;
    FILE* stopfile = fopen("data/ita-stopwords.txt", "r");
    if (stopfile) {
        ita->stopwords = malloc(sizeof(iplus1_tree_t));
        iplus1_tree_init(ita->stopwords, &iplus1_tree_compare_str);
        
        int ch;
        int w = 0;
        char word[64]; // surely no words over 64 characters...right?
        while((ch = fgetc(stopfile)) != EOF) {
            if (ch == '\n') {
                word[w] = '\0';
                char* s = strdup(word);
                iplus1_tree_insert(ita->stopwords, s, s);
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
    strcpy(lang->lang, "ita");
    lang->full_lang = strdup("italian");
    lang->param = malloc(sizeof(iplus1_italian_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_italian_t* ita = (iplus1_italian_t*)lang->param;
    if ((ita->stemmer = sb_stemmer_new("ita", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find italian stemmer\n");
        return IPLUS1_FAIL;
    }
    
    load_stopwords(ita);
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_italian_t* ita = (iplus1_italian_t*)lang->param;
    
    iplus1_tree_foreach_postorder(ita->stopwords, &iplus1_tree_free_key, NULL);
    iplus1_tree_destroy(ita->stopwords);
    free(ita->stopwords);
    
    sb_stemmer_delete(ita->stemmer);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
