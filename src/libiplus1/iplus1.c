#include "iplus1.h"















int iplus1_set_cache(iplus1_t* iplus1, iplus1_cache_t* cache)
{
    iplus1->cache = cache;
    return 1;
}






int iplus1_init(iplus1_t* iplus1)
{
    iplus1->cache = 0;
    return 1;
}

int iplus1_destroy(iplus1_t* iplus1)
{
    return 1;
}
