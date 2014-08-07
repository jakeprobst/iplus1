#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "iplus1.h"
#include "lang.h"

struct iplus1_lang_t** languages;


int _iplus1_load_languages()
{    
    int count = 1; // language list is null terminated, so start with one
    
    DIR* dir;
    if ((dir = opendir(PREFIX "/share/iplus1/lang")) == NULL) {
        fprintf(stderr, "language directory not found '%s'\n", PREFIX "/share/lang");
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
    
    languages = calloc(count, sizeof(iplus1_lang_t*));
    
    int lindex = 0;
    rewinddir(dir);
    while ((dp = readdir(dir)) != NULL) {
        if (strncmp(dp->d_name+strlen(dp->d_name)-3 , ".so", 3) == 0) {
            char path[FILENAME_MAX];
            snprintf(path, FILENAME_MAX, PREFIX "%s%s", "/share/iplus1/lang/", dp->d_name);
            
            languages[lindex] = malloc(sizeof(iplus1_lang_t));
            if (languages[lindex] == NULL) {
                return IPLUS1_FAIL;
            }
            if (iplus1_lang_init(languages[lindex], path) == IPLUS1_FAIL) {
                fprintf(stderr, "could not load: %s\n", path);
                count--;
                free(languages[lindex]);
            }
            else {
                lindex++;
            }
        }
    }
    
    closedir(dir);
    return count;
}


iplus1_lang_t* iplus1_get_lang(char* lang)
{
    int i;
    for(i = 0; languages[i] != NULL; i++) {
        if (strncmp(languages[i]->lang, lang, 3) == 0) {
            return languages[i];
        }
    }
    
    return NULL;
}

int iplus1_init()
{    
    if (_iplus1_load_languages() == IPLUS1_FAIL) {
        return IPLUS1_FAIL;
    }
    
    return IPLUS1_SUCCESS;
}

void iplus1_destroy()
{
    int i;
    for(i = 0; languages[i] != NULL; i++) {
        iplus1_lang_destroy(languages[i]);
        free(languages[i]);
    }
    
    free(languages);
}









