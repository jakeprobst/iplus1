#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libstemmer.h>

#include "iplus1.h"
#include "lang.h"
#include "tree.h"

// there seems to be multiple 3char codes for norwegian
// tatoeba has non and nob, with nob having most of them
// libstemmer uses nor as the character code to get the stemmer

typedef struct iplus1_norwegian_t {
    struct sb_stemmer* stemmer;
    iplus1_tree_t* stopwords;
} iplus1_norwegian_t;

int valid_word(iplus1_norwegian_t* nob, char* s)
{
    if (strlen(s) == 0) {
        return 0;
    }
    if (nob->stopwords) {
        if (iplus1_tree_get(nob->stopwords, s)) {
            return 0;
        }
    }
    return 1;
}

char** parse(char* tstr, void* param)
{
    iplus1_norwegian_t* nob = (iplus1_norwegian_t*)param;
    
    char* str = iplus1_lang_lowercase(tstr);
    if (str == NULL) {
        return NULL;
    }
    
    char** split = iplus1_lang_split(str);
    int output_size = 1; // +1 cause null terminated
    int i;
    for(i = 0; split[i] != NULL; i++) {
        if (valid_word(nob, split[i])) {
            output_size++;
        }
    }
    char** output = calloc(sizeof(char*), output_size);
    if (output == NULL) {
        goto error;
    }
    
    int output_index = 0;
    for(i = 0; split[i] != NULL; i++) {
        if (!valid_word(nob, split[i])) {
            continue;
        }
        
        const sb_symbol* stemmed = sb_stemmer_stem(nob->stemmer, (sb_symbol*)split[i], strlen(split[i]));
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

void load_stopwords(iplus1_norwegian_t* nob)
{
    nob->stopwords = NULL;
    FILE* stopfile = fopen("data/nob-stopwords.txt", "r");
    if (stopfile) {
        nob->stopwords = malloc(sizeof(iplus1_tree_t));
        iplus1_tree_init(nob->stopwords, &iplus1_tree_compare_str);
        
        int ch;
        int w = 0;
        char word[64]; // surely no words over 64 characters...right?
        while((ch = fgetc(stopfile)) != EOF) {
            if (ch == '\n') {
                word[w] = '\0';
                char* s = strdup(word);
                iplus1_tree_insert(nob->stopwords, s, s);
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
    strcpy(lang->lang, "nob");
    lang->full_lang = strdup("norwegian");
    lang->param = malloc(sizeof(iplus1_norwegian_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_norwegian_t* nob = (iplus1_norwegian_t*)lang->param;
    if ((nob->stemmer = sb_stemmer_new("nor", "UTF_8")) == NULL) {
        fprintf(stderr, "could not find norwegian stemmer\n");
        return IPLUS1_FAIL;
    }
    
    load_stopwords(nob);
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_norwegian_t* nob = (iplus1_norwegian_t*)lang->param;
    
    iplus1_tree_foreach_postorder(nob->stopwords, &iplus1_tree_free_key, NULL);
    iplus1_tree_destroy(nob->stopwords);
    free(nob->stopwords);
    
    sb_stemmer_delete(nob->stemmer);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
