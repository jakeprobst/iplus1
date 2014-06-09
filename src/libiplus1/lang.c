#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unicode/ustring.h>
#include <unicode/ubrk.h>


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

char* iplus1_lang_lowercase(char* str)
{
    int len;
    UErrorCode error = U_ZERO_ERROR;

    // this always gives U_BUFFER_OVERFLOW_ERROR even though it specifically says
    // this usage is allowed
    u_strFromUTF8(NULL, 0, &len, str, strlen(str), &error); 
    error = U_ZERO_ERROR;
    UChar uc[len+1];
    u_strFromUTF8(uc, len+1, &len, str, strlen(str), &error); 
    if (U_FAILURE(error)) {
        fprintf(stderr, "iplus1_lang_lowercase:u_strFromUTF8 error: '%s' %d\n", str, error);
        return NULL;
    }
    
    u_strToLower(uc, len+1, uc, u_strlen(uc), "", &error);
    if (U_FAILURE(error)) {
        fprintf(stderr, "iplus1_lang_lowercase:u_strToUpper error: %d\n", error);
        return NULL;
    }

    u_strToUTF8(NULL, 0, &len, uc, u_strlen(uc), &error);
    error = U_ZERO_ERROR;
    char* out_str = malloc(len+1);

    u_strToUTF8(out_str, len+1, &len, uc, u_strlen(uc), &error);
    if (U_FAILURE(error)) {
        fprintf(stderr, "iplus1_lang_lowercase:u_strToUTF8 error: '%s' %d\n", str, error);
        return NULL;
    }
    
    return out_str;
}

char** iplus1_lang_split(char* str)
{
    int len;
    UErrorCode error = U_ZERO_ERROR;
    char** output;
    
    // this always gives U_BUFFER_OVERFLOW_ERROR even though it specifically says
    // this usage is allowed
    u_strFromUTF8(NULL, 0, &len, str, strlen(str), &error); 
    error = U_ZERO_ERROR;
    UChar uc[len+1];
    u_strFromUTF8(uc, len+1, &len, str, strlen(str), &error); 
    if (U_FAILURE(error)) {
        fprintf(stderr, "iplus1_lang_split:u_strFromUTF8 error: %d\n", error);
        return NULL;
    }

    UBreakIterator* bi = ubrk_open(UBRK_WORD, "", uc, len+1, &error);
    if (U_FAILURE(error)) {
        fprintf(stderr, "iplus1_lang_split:ubrk_open error: %d\n", error);
        return NULL;
    }

    int output_size = 2; // +1 for counting spaces, +1 for null
    int index;
    for(index = ubrk_first(bi); index != UBRK_DONE; index = ubrk_next(bi)) {
        output_size++;
    }
    output = calloc(sizeof(char*), output_size);
   
    int output_index = 0;
    int prev = ubrk_first(bi);
    for(index = ubrk_next(bi); index != UBRK_DONE; index = ubrk_next(bi)) {
        UChar s[index-prev+1];
        u_strncpy(s, uc+prev, index-prev);
        s[index-prev] = '\0';
        
        u_strToUTF8(NULL, 0, &len, s, u_strlen(s), &error);
        error = U_ZERO_ERROR;
        
        output[output_index] = malloc(len+1);
        u_strToUTF8(output[output_index], len+1, &len, s, u_strlen(s), &error);
        if (U_FAILURE(error)) {
            fprintf(stderr, "iplus1_lang_split:u_strToUTF8 error: %d\n", error);
            free(output[output_index]);
            continue;
        }
        
        // check if the word is already in the list
        int n;
        for(n = 0; n < output_index; n++) {
            if (strcmp(output[output_index], output[n]) == 0) {
                break;
            }
        }
        if (n != output_index) { // we hit the word so the loop broke early
            free(output[output_index]);
        }
        else {
            output_index++;
        }
        
        prev = index;
    }
    
    ubrk_close(bi);
    return output;
}







