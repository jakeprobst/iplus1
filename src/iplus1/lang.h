#ifndef _LANG_H_
#define _LANG_H_

/* language handle */
typedef struct iplus1_lang_t {
    char lang[4]; // 3 character language code
    char* full_lang; // full language name
    
    void* param; // used to hold persistent language plugin data
    char** (*parse)(char*, void*); // function to parse string (use iplus1_lang_parse)
    int (*destroy)(struct iplus1_lang_t*); // clean up plugin data
    
    void* dlhandle; // shared library handle
} iplus1_lang_t;


/* initializes a lang
 * path: full path to .so to parse 
 * returns -1 on failure */
int iplus1_lang_init(iplus1_lang_t*, char* path);

/* destroys a lang */
void iplus1_lang_destroy(iplus1_lang_t*);

/* stems all the words in a string
 * str: string to parse
 * returns NULL terminated array of strings (free with iplus1_lang_parse_free) */
char** iplus1_lang_parse(iplus1_lang_t*, char* str);

/* free array of strings
 * tokens: return value of iplus1_lang_parse
 * note: array must be NULL terminated
 * */
void iplus1_lang_parse_free(char** tokens);

// TODO: iplus1_lang_parse_free should just be a generic iplus1_strarray_free()

/* lowers the case of all characters in a string
 * convenience function for language plugins
 * str: string to lowercase
 * side effects: modifies str
 * returns: all lowercase string */
char* iplus1_lang_lowercase(char* str);

/* splits string up by natual word boundaries
 * convenience function for language plugins
 * str: string to split
 * side effect: modifies str
 * returns: NULL terminated array of strings */
char** iplus1_lang_split(char* str);




#endif /* _LANG_H_ */
