#ifndef _IPLUS1_H_
#define _IPLUS1_H_

#include "lang.h"

enum {
    IPLUS1_SUCCESS = 0,
    IPLUS1_FAIL = -1,
};

struct iplus1_lang_t;

int iplus1_init();
int iplus1_destroy();

struct iplus1_lang_t* iplus1_get_lang(char*);


#endif /* _IPLUS1_H_ */
