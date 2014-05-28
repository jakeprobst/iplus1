#include <stdio.h>
#include "iplus1.h"
#include "cache.h"






int main()
{
    iplus1_t iplus1;
    //iplus1_cache_t cache;
    
    //iplus1_cache_init(&cache, NULL);

    
    iplus1_init(&iplus1);
    
    //iplus1_set_cache(&iplus1, &cache);
    
    
    
    
    
    
    iplus1_destroy(&iplus1);
    
    return 0;
}
