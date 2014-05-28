#ifndef _PLUGIN_H_
#define _PLUGIN_H_



#include "iplus1.h"



typedef struct iplus1_lang_t {
    char lang[4]; // 3 character language code
    
    void* param;
    int (*process)(char*,void*);
    
    
} iplus1_lang_t;



int iplus1_lang_load(char* path, iplus1_lang_t*);














#endif /* _PLUGIN_H_ */
