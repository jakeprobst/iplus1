#ifndef _LANG_H_
#define _LANG_H_

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
void iplus1_lang_parse_free(char**);

// convenience functions for language plugins
// most will probably want to use these
char* iplus1_lang_lowercase(char*);
char** iplus1_lang_split(char*);




#endif /* _LANG_H_ */
