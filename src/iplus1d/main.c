#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iplus1.h"
#include "cache.h"
#include "collection.h"
#include "sentence.h"


#include "tree.h"

int cmp(void* a, void* b)
{
    int* at = a;
    int* bt = b;
    return *at - *bt;
}

char* readline(FILE* fd)
{
    int len = 0;
    int start = ftell(fd);
    
    while(1) {
        int c = fgetc(fd);
        if (c == '\n' || c == EOF) {
            break;
        }
        len++;
    }
    
    if (len == 0) {
        return NULL;
    }
    
    char* buf = malloc(len+1);
    fseek(fd, start, SEEK_SET);
    fread(buf, len, 1, fd);
    buf[len] = '\0';
    
    fgetc(fd); // get that linebreak out of there
        
    return buf;
}




int load_collection(iplus1_t* iplus1, iplus1_collection_t* col)
{
    FILE* tatoeba_db = fopen("sentences.csv", "r");
    if (tatoeba_db == NULL) {
        return -1;
    }
    char* line;
    while ((line = readline(tatoeba_db)) != NULL) {
        char* s;
        char* position;
        int id;
        char* language = NULL;
        char* sentence = NULL;
        s = strtok_r(line, "\t", &position);
        if (s != NULL) {
            id = atol(s);
        }
        else {
            goto error;
        }
        s = strtok_r(NULL, "\t", &position);
        if (s != NULL) {
            language = strdup(s);
        }
        else {
            goto error;
        }
        s = strtok_r(NULL, "\t", &position);
        if (s != NULL) {
            sentence = strdup(s);
        }
        else {
            goto error;
        }
        
        iplus1_lang_t* lang = iplus1_get_lang(iplus1, language);
        if (lang != NULL) {
            iplus1_sentence_t* sen = malloc(sizeof(iplus1_sentence_t));
            
            iplus1_sentence_init(sen, lang, sentence);
            sen->id = id;
            
            iplus1_collection_add_sentence(col, sen);
            
        }
error:
        free(language);
        free(sentence);
        free(line);
    }
    fclose(tatoeba_db);
    
    return 1;
}

int link_collection(iplus1_t* iplus1, iplus1_collection_t* col)
{
    FILE* links = fopen("links.csv", "r");
    if (links == NULL) {
        return -1;
    }
    
    char* line;
    while ((line = readline(links)) != NULL) {
        char* s;
        char* position;
        
        int left = -1, right = -1;
        s = strtok_r(line, "\t", &position);
        if (s) {
            left = atoi(s);
        }
        else {
            goto error;
        }
        s = strtok_r(NULL, "\t", &position);
        if (s) {
            right = atoi(s);
        }
        else {
            goto error;
        }
        
        
        iplus1_sentence_t* original = iplus1_collection_get_sentence_by_id(col, left);
        iplus1_sentence_t* translated = iplus1_collection_get_sentence_by_id(col, right);
        
        if (original != NULL && translated != NULL) {
            iplus1_sentence_add_translation(original, translated);
        }
        
error:
        free(line);
    }
    
    fclose(links);
    return 0;
}


int main()
{
    iplus1_t iplus1;
    iplus1_collection_t main_db;

    iplus1_init(&iplus1);
    iplus1_collection_init(&main_db);
    load_collection(&iplus1, &main_db);
    link_collection(&iplus1, &main_db);
    
    iplus1_sentence_t* sen = iplus1_collection_get_sentence_by_id(&main_db, 120033);
    
    printf("orig: %s\n", sen->str);
    int i;
    for(i = 0; sen->translations[i] != NULL; i++) {
        printf("trans: %s\n", sen->translations[i]->str);
    }
    
    iplus1_collection_destroy(&main_db);
    iplus1_destroy(&iplus1);
    
    return 0;
}
