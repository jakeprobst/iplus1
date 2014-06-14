#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "input.h"
//#include "redisdb.h"






int handle_input(command_t* cmd, void* param)
{
    
    
    
    
    
    
    return 0;
}




int main(int argc, char** argv)
{
    
    input_t input;
    input_init(&input);
    input_set_callback(&input, &handle_input, NULL);
    
    
    
    
    
    input_loop(&input);
    
    input_destroy(&input);
    return 0;
}
