#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unicode/uclean.h>

#include "iplus1.h"
#include "lang.h"
#include "list.h"
#include "tree.h"

#include "input.h"
#include "redis.h"
#include "user.h"

iplus1_tree_t anki_users;

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
        return -1; //not supported lang
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



int handle_anki_begin(command_t* cmd, redis_t* redis)
{
    user_t* user = malloc(sizeof(user_t));
    if (user_init(user, cmd->fd, cmd->ankibegin.nlang, cmd->ankibegin.tlang) < 0) {
        fprintf(stderr, "could not init user: %d %s %s\n", cmd->fd, cmd->ankibegin.nlang, cmd->ankibegin.tlang);
        free(user);
        return -1;
    }
    printf("user added '%s' '%s'\n", cmd->ankibegin.nlang, cmd->ankibegin.tlang);
    iplus1_tree_insert(&anki_users, &user->fd, user);
    return -1;
}

int handle_anki_card(command_t* cmd, redis_t* redis)
{
    user_t* user = iplus1_tree_get(&anki_users, &cmd->fd);
    if (user == NULL) {
        fprintf(stderr, "handle_anki_card: user not initialized\n");
        return -1;
    }
    
    char** words = iplus1_lang_parse(user->tlang, cmd->anki.text);
    int i;
    for(i = 0; words[i] != NULL; i++) {
        user_add_word(user, words[i]);
    }
    return 0;
}

int handle_anki_end(command_t* cmd, redis_t* redis)
{
    user_t* user = iplus1_tree_get(&anki_users, &cmd->fd);
    if (user == NULL) {
        fprintf(stderr, "handle_anki_end: user not initialized\n");
        return -1;
    }
    
    char langs[32];
    if(strncmp(user->nlang->lang, user->tlang->lang, 3) < 0) {
        snprintf(langs, 32, "%s-%s", user->nlang->lang, user->tlang->lang);
    }
    else {
        snprintf(langs, 32, "%s-%s", user->tlang->lang, user->nlang->lang);
    }
    
    redis_t sub_redis;
    redis_init(&sub_redis, NULL, -1);
    int index = 0;
    while(1) {
        printf("SSCAN %s %d\n", langs, index);
        redis_command(redis, "SSCAN %s %d", langs, index);
        
        index = atoi(redis->reply->element[0]->str);
        
        
        int i;
        for(i = 0; i < redis->reply->element[1]->elements; i++) {
            int64_t result = atoll(redis->reply->element[1]->element[i]->str);
            
            int64_t n, t;
            if(strncmp(user->nlang->lang, user->tlang->lang, 3) < 0) {
                n = result & 0xFFFFFFFF;
                t = result >> 32;
            }
            else {
                t = result & 0xFFFFFFFF;
                n = result >> 32;
            }
            
            int count = 0;
            char *s, *pos, *str, *word;
            redis_command(&sub_redis, "HGET %s %d", user->tlang->lang, t);
            printf("str: %ld -> %ld %s\n", result, t, sub_redis.reply->str);
            for(str = sub_redis.reply->str; (s = strtok_r(str, ",", &pos)) != NULL; str = NULL) {
                printf("s: %s\n", s);
                if (!user_has_word(user, s)) {
                    count++;
                    word = s;
                }
            }
            
            if (count == 1) { // awesome we got one new thing
                char buf[256];
                int l = snprintf(buf, 256, "%ld\t%ld\t%s", n, t, word) + 1;
                printf("%s\n", buf);
                send(user->fd, buf, l, 0);
            }
        }
        
        
    }
    
    printf("user removed\n");
    redis_destroy(&sub_redis);
    iplus1_tree_remove(&anki_users, &user->fd, &user_tree_free);
    return 0;
}


int handle_input(command_t* cmd, void* param)
{
    if (cmd->type == CMD_SENTENCE) {
        handle_sentence(cmd, param);
    }
    else if (cmd->type == CMD_LINK) {
        handle_link(cmd, param);
    }
    else if (cmd->type == CMD_ANKIBEGIN) {
        handle_anki_begin(cmd, param);
    }
    else if (cmd->type == CMD_ANKICARD) {
        handle_anki_card(cmd, param);
    }
    else if (cmd->type == CMD_ANKIEND) {
        handle_anki_end(cmd, param);
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
    iplus1_tree_init(&anki_users, iplus1_tree_compare_int);
    
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
    iplus1_tree_destroy(&anki_users);
    iplus1_destroy();
    u_cleanup();
    return 0;
}
