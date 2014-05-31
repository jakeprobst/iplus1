#include <stdio.h>
#include <dlfcn.h>


#include "iplus1.h"
#include "lang.h"








int iplus1_lang_init(iplus1_lang_t* lang, char* path)
{
    void* handle;
    if ((handle = dlopen(path, RTLD_LAZY)) == NULL) {
        fprintf(stderr, "could not load %s\n", path);
        return IPLUS1_FAIL;
    }
    
    int (*init)(iplus1_lang_t*);
    
    if ((init = dlsym(handle, "init")) == NULL) {
        fprintf(stderr, "no init func in %s\n", path);
        return IPLUS1_FAIL;
    }

    if ((lang->parse = dlsym(handle, "parse")) == NULL) {
        fprintf(stderr, "no parse func in %s\n", path);
        return IPLUS1_FAIL;
    }
    
    if ((lang->destroy = dlsym(handle, "destroy")) == NULL) {
        fprintf(stderr, "no destroy func in %s\n", path);
        return IPLUS1_FAIL;
    }

    lang->dlhandle = handle;
    return init(lang);
}

int iplus1_lang_destroy(iplus1_lang_t* lang)
{
    lang->destroy(lang);
    dlclose(lang->dlhandle);
    return IPLUS1_SUCCESS;
}

char** iplus1_lang_parse(iplus1_lang_t* lang, char* str)
{
    return lang->parse(str, lang->param);
}

