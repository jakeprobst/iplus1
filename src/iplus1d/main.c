#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unicode/uclean.h>

#include "iplus1.h"
#include "lang.h"
#include "list.h"
#include "tree.h"

#include "input.h"
#include "redis.h"



int handle_sentence(command_t* cmd, redis_t* redis)
{
    iplus1_lang_t* lang = iplus1_get_lang(cmd->sen.lang);
    if (lang == NULL) {
        //not supported lang
        return -1;
    }
    
    char** token = iplus1_lang_parse(lang, cmd->sen.sen);
    if (token == NULL) {
        // could not parse
        return -1;
    }
    if (token[0] == NULL) { // there are sentences that actually have no tokens!
        free(token);
        return 0;
    }
        
    int tokenlen = 1;
    int i;
    for(i = 0; token[i]; i++) {
        tokenlen += strlen(token[i]) + 1; // append a comma
    }
        
    char tokenbuf[tokenlen];
    memset(tokenbuf, 0, tokenlen);
    strncat(tokenbuf, token[0], tokenlen);
    for(i = 1; token[i]; i++) {
        strncat(tokenbuf, ",", tokenlen);
        strncat(tokenbuf, token[i], tokenlen);
    }
    
    redis_command(redis, "HSET %s %d %s", cmd->sen.lang, cmd->sen.id, tokenbuf);
    printf("HSET %s %d %s\n", cmd->sen.lang, cmd->sen.id, tokenbuf);
    if (redis->reply->type == REDIS_REPLY_ERROR) {
        printf("error: %s\n", redis->reply->str);
    }
    
    
    if (token) {
        for(i = 0; token[i] != NULL; i++) {
            free(token[i]);
        }
        free(token);
    }
    
    return 0;
}


int handle_link(command_t* cmd, redis_t* redis)
{
    
    if (iplus1_get_lang(cmd->link.lang) == NULL || iplus1_get_lang(cmd->link.tlang) == NULL) {
        //not supported lang
        return -1;
    }
    
    // redis can optimize sets if the value is a 64 bit integer
    // so here I merge two 32 bit ints into a single 64 bit int
    // cause really, tatoeba isnt gonna have 4294967295 sentences anytime soon
    // the & 0xFFFFFFFF is the one specified on the left, >> 32 is on right
    // ie. "eng-jpn" will be `eng = id & 0xFFFFFFFF; jpn = id >> 32;`
    int64_t merged = 0;
    if (strcmp(cmd->link.lang, cmd->link.tlang) < 0) { // lang sorts before tlang
        merged = (cmd->link.id) | (cmd->link.tid << 32);
        redis_command(redis, "SADD %s-%s %ld", cmd->link.lang, cmd->link.tlang, merged);
        if (redis->reply->type == REDIS_REPLY_ERROR) {
            printf("error: %s\n", redis->reply->str);
        }
        printf("SSET %s-%s %ld\n", cmd->link.lang, cmd->link.tlang, merged);
    }
    else {
        merged = (cmd->link.tid) | (cmd->link.id << 32);
        redis_command(redis, "SADD %s-%s %ld", cmd->link.tlang, cmd->link.lang, merged);
        if (redis->reply->type == REDIS_REPLY_ERROR) {
            printf("error: %s\n", redis->reply->str);
        }
        printf("SSET %s-%s %ld\n", cmd->link.tlang, cmd->link.lang, merged);
    }
    
    return 0;
}

int handle_input(command_t* cmd, void* param)
{
    if (cmd->type == CMD_SENTENCE) {
        handle_sentence(cmd, param);
    }
    if (cmd->type == CMD_LINK) {
        handle_link(cmd, param);
    }
    
    
    
    return 0;
}

int free_tokens(void* k, void* v, void* param)
{
    free(k);
    iplus1_list_t* list = v;
    iplus1_list_node_t* l;
    for(l = list->node; l; l = l->next) {
        free(l->data);
    }
    iplus1_list_destroy(list);
    
    free(list);
    return 0;
}


int main(int argc, char** argv)
{
    iplus1_init();
    
    redis_t redis;
    if (redis_init(&redis, NULL, -1) == -1) {
        fprintf(stderr, "could not init redis\n");
        return -1;
    }
        
    input_t input;
    input_init(&input);
    input_set_callback(&input, &handle_input, &redis);
    
    
    

    
    printf("ready\n");
    input_loop(&input);
    
    input_destroy(&input);
    redis_destroy(&redis);
    iplus1_destroy();
    u_cleanup();
    return 0;
}
