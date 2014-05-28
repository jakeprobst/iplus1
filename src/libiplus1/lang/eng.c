#include "iplus1.h"
#include "lang.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef struct iplus1_english_t {
    
    
} iplus1_english_t;





int process(char* str, void* param)
{
    //iplus1_english_t* eng = (iplus1_english_t*)param;
    
    printf("process: %s\n", str);
    
    
    
    return IPLUS1_SUCCESS;
}


















int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "eng");
    lang->param = malloc(sizeof(iplus1_english_t));
    lang->process = process;
    
    return IPLUS1_SUCCESS;
}
