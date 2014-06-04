#ifndef _IPLUS1_H_
#define _IPLUS1_H_

enum {
    IPLUS1_SUCCESS = 0,
    IPLUS1_FAIL = -1,
};


struct iplus1_lang_t;


typedef struct iplus1_t {
    struct iplus1_lang_t** language;
    
} iplus1_t;



//int iplus1_set_cache(iplus1_t*, iplus1_cache_t*);

int iplus1_init(iplus1_t*);
int iplus1_destroy(iplus1_t*);


struct iplus1_lang_t* iplus1_get_lang(iplus1_t*, char*);





#endif /* _IPLUS1_H_ */
