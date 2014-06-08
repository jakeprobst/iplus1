#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "anki.h"

#include "iplus1.h"











int anki_init(anki_t* anki, char* path)
{
    if (sqlite3_open(path, &anki->db)) {
        fprintf(stderr, "can't open database: %s\n", path);
        sqlite3_close(anki->db);
        return IPLUS1_FAIL;
    }

    
    
    return IPLUS1_SUCCESS;
    
}

int anki_destroy(anki_t* anki)
{
    sqlite3_close(anki->db);
    return IPLUS1_SUCCESS;
}

int _foreach_callback(void* param, int argc, char** argv, char** colname)
{
     int (*func)(char*) = param;
    
    // strip clozes and html
    char* str = argv[0];
    char* buf = malloc(strlen(argv[0])+1);
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

    func(buf);
    free(buf);
    return 0;
}


int anki_foreach(anki_t* anki, int (*func)(char*, void*), void* param)
{
    
    int _subfunc(char* s)
    {
        return func(s, param);
    }
    
    char* errmsg;
    if (sqlite3_exec(anki->db, "SELECT flds FROM notes;", &_foreach_callback, 
                     &_subfunc, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "sql error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return IPLUS1_FAIL;
    }
    
    
    
    return IPLUS1_SUCCESS;
}









