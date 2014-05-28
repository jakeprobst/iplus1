#include <stdio.h>
#include <dlfcn.h>


#include "iplus1.h"
#include "lang.h"














int iplus1_lang_load(char* path, iplus1_lang_t* lang)
{
    void* handle;
    if ((handle = dlopen(path, RTLD_LAZY)) == NULL) {
        printf("could not load %s\n", path);
        return IPLUS1_FAIL;
    }
    
    int (*init)(iplus1_lang_t*);
    
    if ((init = dlsym(handle, "init")) == NULL) {
        printf("no init func in %s\n", path);
        return IPLUS1_FAIL;
        
    }

    if ((lang->process = dlsym(handle, "process")) == NULL) {
        printf("no process func in %s\n", path);
        return IPLUS1_FAIL;
    }

    return init(lang);
}
