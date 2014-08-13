#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "iplus1/iplus1.h"
#include "iplus1/parse.h"
#include "iplus1/redis.h"

/* this requires a running redis server and running the scripts:
 * preparefiles.py: fetches data from tatoeba and formats to be easily parsed
 * fillredis.py: iplus1`s all sentences of a languages and inserts them into redis; also links sentences together
 * insertlang.py: inserts full copies of sentences into redis
 * this may take a while (a few hours even) */


#define REDIS_SERVER 'localhost'
#define REDIS_PORT 6379

/* remove any html, clozes, and escape codes from the card string */
char* anki_card_clean(char* str)
{
    char* buf = malloc(strlen(str)+1);
    int str_index = 0, buf_index = 0;
    int in_html = 0, in_cloze = 0, in_escape = 0;
    for(; str[str_index] != '\0'; str_index++) {
        if (str[str_index] == '<') {
            in_html = 1;
            continue;
        }
        if (str[str_index] == '>') {
            in_html = 0;
            continue;
        }

        if (strncmp(str+str_index, "{{" ,2) == 0) {
            in_cloze = 1;
            str_index++;
            continue;
        }
        if (strncmp(str+str_index, "::" ,2) == 0 && in_cloze) {
            in_cloze = 0;
            str_index++;
            continue;
        }
        if (strncmp(str+str_index, "}}" ,2) == 0) {
            str_index++;
            continue;
        }
        
        if (str[str_index] == '&') {
            in_escape = 1;
            continue;
        }
        if (str[str_index] == ';') {
            in_escape = 0;
            continue;
        }
        
        if (!in_html && !in_cloze && !in_escape) {
            buf[buf_index] = str[str_index];
            buf_index++;
        }
    }
    buf[buf_index] = '\0';
    return buf;
}

/* get the front side of all cards in a deck */
char** load_anki_deck(char* ankideck)
{
    sqlite3 *anki;
    if (sqlite3_open(ankideck, &anki)) {
        sqlite3_close(anki);
        return NULL;
    }
    
    char** result;
    int nrow, ncol;
    char* errmsg;
    if (sqlite3_get_table(anki, "SELECT flds FROM notes;", &result, &nrow, &ncol, &errmsg) != SQLITE_OK) {
        sqlite3_close(anki);
        return NULL;
    }
    sqlite3_close(anki);

    char** out = calloc(sizeof(char*), nrow+1);
    int i;
    for(i = 0; i < nrow; i++) {
        out[i] = anki_card_clean(result[i]);
    }
    
    sqlite3_free_table(result);
    
    return out;
}



int main(int argc, char** argv)
{
    /* initialize iplus1, needs to be done before calling any iplus1 functions */
    iplus1_init();
    
    if (argc != 4) {
        printf("usage: %s <native lang> <target lang> <path to collection.anki2>\n", argv[0]);
        
        char** langs = iplus1_get_supported_langs();
        printf("langs:");
        int l;
        for(l = 0; langs[l] != NULL; l++) {
            printf(" %s", langs[l]);
        }
        printf("\n");
        return 0;
    }
    char* nlang = argv[1];
    char* tlang = argv[2];
    char* ankideck = argv[3];
    
    /* fetch the sentences from the anki deck */
    char** sentences = load_anki_deck(ankideck);
    if (sentences == NULL) {
        printf("invalid deck: %s\n", ankideck);
        return 0;
    }
    
    /* the core iplus1 function,
     * takes character codes for native and target languages
     * see iplus1_get_supported_langs() for supported languages 
     * and a NULL terminated array of sentence strings
     * returns an array of results (see results.h) */
    result_t* results = iplus1_parse_full(nlang, tlang, sentences);
    
    
    /* normally, redis should not be used, but in the interest of having a full
     * locally running setup, I use it here. Tatoeba uses django and so only
     * required that IDs of sentences would be returned.
     * In the future sqlite should be possible. */
    redis_t redis;
    redis_init(&redis, NULL, -1);
    int r;
    for(r = 0; results[r].nid != 0; r++) {
        redis_command(&redis, "HGET %s-full %d", nlang, results[r].nid);
        printf("%s -> ", redis.reply->str);
        redis_command(&redis, "HGET %s-full %d", tlang, results[r].tid);
        printf("%s\n", redis.reply->str);
    }
    
    /* misc cleanup */
    redis_destroy(&redis);
    iplus1_parse_full_free(results);
    iplus1_lang_parse_free(sentences); // this is just a generic strarray free-er
    iplus1_destroy();
    return 1;
}







