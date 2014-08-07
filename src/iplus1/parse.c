#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "redis.h"

result_t* iplus1_parse_user(user_t* user)
{
    char langs[32];
    if(strncmp(user->nlang->lang, user->tlang->lang, 3) < 0) {
        snprintf(langs, 32, "%s-%s", user->nlang->lang, user->tlang->lang);
    }
    else {
        snprintf(langs, 32, "%s-%s", user->tlang->lang, user->nlang->lang);
    }
    
    result_t* results = calloc(MAX_RESULTS+1, sizeof(result_t));
    int sent_words = 0;
    //int hget_count = 0;
    redis_t redis, sub_redis;
    if (redis_init(&redis, NULL, -1) == -1) {
        return NULL;
    }
    if (redis_init(&sub_redis, NULL, -1) == -1) {
        return NULL;
    }
    int index = 0;
    while(1) {
        redis_command(&redis, "SSCAN %s %d", langs, index);
        
        index = atoi(redis.reply->element[0]->str);
        
        int i;
        for(i = 0; i < redis.reply->element[1]->elements; i++) {
            int64_t result = atoll(redis.reply->element[1]->element[i]->str);
            
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
            //char *s, *pos, *str, *word;
            char *s, *pos, *str;
            redis_command(&sub_redis, "HGET %s %d", user->tlang->lang, t);
            for(str = sub_redis.reply->str; (s = strtok_r(str, ",", &pos)) != NULL; str = NULL) {
                if (!user_has_word(user, s)) {
                    count++;
                    //word = s;
                }
            }
            
            if (count == 1) { // awesome we got one new thing
                results[sent_words].nid = n;
                results[sent_words].tid = t;
                sent_words++;
                if (sent_words >= MAX_RESULTS) {
                    break;
                }
            }
        }
        
        if (index == 0) {
            break;
        }
    }
        
    redis_destroy(&sub_redis);
    redis_destroy(&redis);
    return results;
}

result_t* iplus1_parse_full(char* nlang, char* tlang, char** str)
{
    user_t user;
    user_init(&user, nlang, tlang);
    user_parse(&user, str);
    
    result_t* result = iplus1_parse_user(&user);
    user_destroy(&user);
    return result;
}

void iplus1_parse_full_free(result_t* result)
{
    free(result);
}



