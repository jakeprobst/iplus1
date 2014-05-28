#ifndef _IPLUS1_H_
#define _IPLUS1_H_

#include "cache.h"


typedef struct _iplus1_t {
    iplus1_cache_t* cache;
    
} iplus1_t;



int iplus1_set_cache(iplus1_t*, iplus1_cache_t*);

int iplus1_init(iplus1_t*);
int iplus1_destroy(iplus1_t*);










#endif /* _IPLUS1_H_ */
