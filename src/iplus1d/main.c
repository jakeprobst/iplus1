#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iplus1.h"
#include "cache.h"
#include "sentence.h"





int main()
{
    iplus1_t iplus1;
    iplus1_sentence_t sen;
    //iplus1_cache_t cache;
    
    //iplus1_cache_init(&cache, NULL);

    
    iplus1_init(&iplus1);
    
    
    char* str = malloc(256);
    strcpy(str, "I am running very far away");
    iplus1_sentence_init(&sen, iplus1_get_lang(&iplus1, "eng"), str);
    
    int i;
    printf("str: %s\n", sen.str);
    for(i = 0; sen.words[i] != NULL; i++) {
        printf("w: %s\n", sen.words[i]);
    }
    
    
    
    
    free(str);
    iplus1_sentence_destroy(&sen);
    iplus1_destroy(&iplus1);
    
    return 0;
}
