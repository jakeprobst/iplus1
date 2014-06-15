#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unicode/uclean.h>


#include "input.h"
//#include "redisdb.h"






int handle_input(command_t* cmd, void* param)
{
    printf("cmd: %d\n", cmd->type);
    if (cmd->type == CMD_SENTENCE) {
        printf("sen: %s\n", cmd->sen.sen);
    }
    
    
    
    
    
    return 0;
}




int main(int argc, char** argv)
{
    
    input_t input;
    input_init(&input);
    input_set_callback(&input, &handle_input, NULL);
    
    
    
    
    
    input_loop(&input);
    
    input_destroy(&input);
    u_cleanup();
    return 0;
}
