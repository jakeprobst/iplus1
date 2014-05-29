#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "iplus1.h"
#include "cache.h"
#include "lang.h"










int _iplus1_load_languages(iplus1_t* iplus1)
{
    // TODO: later on this should use the PREFIX variable
    // but for now I`ll just hardcode it where they are locally
    
    int count = 0;
    
    DIR* dir;
    if ((dir = opendir("./lang")) == NULL) {
        fprintf(stderr, "language directory not found\n");
        return IPLUS1_FAIL;
    }
    
    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL) {
        if (strncmp(dp->d_name+strlen(dp->d_name)-3 , ".so", 3) == 0) {
            count++;
        }
    }
    if (count == 0) {
        fprintf(stderr, "no languages found\n");
        return IPLUS1_FAIL;
    }
    
    iplus1->language = calloc(count, sizeof(iplus1_lang_t*));
    
    int lindex = 0;
    rewinddir(dir);
    while ((dp = readdir(dir)) != NULL) {
        if (strncmp(dp->d_name+strlen(dp->d_name)-3 , ".so", 3) == 0) {
            char path[FILENAME_MAX];
            snprintf(path, FILENAME_MAX, "%s%s", "lang/", dp->d_name);
            
            iplus1->language[lindex] = malloc(sizeof(iplus1_lang_t));
            if (iplus1_lang_load(path, iplus1->language[lindex]) == IPLUS1_FAIL) {
                count--;
                free(iplus1->language[lindex]);
            }
            else {
                lindex++;
            }
        }
    }
    
    closedir(dir);
    return count;
}




int iplus1_set_cache(iplus1_t* iplus1, iplus1_cache_t* cache)
{
    iplus1->cache = cache;
    return IPLUS1_SUCCESS;
}




int iplus1_init(iplus1_t* iplus1)
{
    iplus1->cache = NULL;
    
    if (_iplus1_load_languages(iplus1) == IPLUS1_FAIL) {
        return IPLUS1_FAIL;
    }
    
    return IPLUS1_SUCCESS;
}

int iplus1_destroy(iplus1_t* iplus1)
{
    return IPLUS1_SUCCESS;
}









