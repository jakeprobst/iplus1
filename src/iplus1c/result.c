#include <stdlib.h>

#include "result.h"



int _free_str(void* str, void* param)
{
    free(str);
    return 0;
}


int search_result_init(search_result_t* result)
{
    result->original = NULL;
    result->translated = NULL;
    result->new_words = malloc(sizeof(iplus1_list_t));
    iplus1_list_init(result->new_words);
    
    return 0;
}

int search_result_destroy(search_result_t* result)
{
    free(result->original);
    free(result->translated);
    iplus1_list_foreach(result->new_words, &_free_str, NULL);
    iplus1_list_destroy(result->new_words);
    free(result->new_words);
    
    return 0;
}
