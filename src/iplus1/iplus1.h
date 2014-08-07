#ifndef _IPLUS1_H_
#define _IPLUS1_H_

#include "lang.h"

enum {
    IPLUS1_SUCCESS = 0,
    IPLUS1_FAIL = -1,
};

/* initializes libiplus1, call before using anything 
 * returns -1 on failure */
int iplus1_init();

/* cleans up libiplus1 */
void iplus1_destroy();

/* returns language handle to use for parsing 
 * lang: 3 letter character code ('eng', 'fra', etc.) 
 * returns NULL if no such language exists */
struct iplus1_lang_t* iplus1_get_lang(char* lang);

/* get a list of supported languages
 * returns NULL terminated array of supported languages*/
char** iplus1_get_supported_langs();


#endif /* _IPLUS1_H_ */
