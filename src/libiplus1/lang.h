#ifndef _PLUGIN_H_
#define _PLUGIN_H_



#include "iplus1.h"



typedef struct iplus1_lang_t {
    char lang[4]; // 3 character language code
    char* full_lang;
    
    void* param;
    char** (*parse)(char*, void*);
    int (*destroy)(struct iplus1_lang_t*);
    
    void* dlhandle;
    
} iplus1_lang_t;



int iplus1_lang_init(iplus1_lang_t*, char* path);
int iplus1_lang_destroy(iplus1_lang_t*);

char** iplus1_lang_parse(iplus1_lang_t*, char*);

// convenience functions for language plugins
// most will probably want to use these
int iplus1_lang_lowercase(char*);
char** iplus1_lang_split(char*);








#endif /* _PLUGIN_H_ */
