#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mecab.h>

#include "iplus1.h"
#include "lang.h"


typedef struct iplus1_japanese_t {
    mecab_t* mecab;
    
} iplus1_japanese_t;

int valid_word(char* s, const mecab_node_t* node)
{
    // the documentation for these was found in a magical scroll hidden away in 
    // a forgotten shrine
    if (42 <= node->posid && node->posid <= 44) { // 42: 名詞,固有名詞,人名,一般 general names
        return 0;                                 // 43: 名詞,固有名詞,人名,姓 surname
    }                                             // 44: 名詞,固有名詞,人名,名 name
    if (node->posid == 25) {                      // 25: 助動詞 aux verbs
        return 0;                                 //  3: 記号,アルファベット alphabet
    }                                             //  4: 記号,一般 general
    if (3 <= node->posid && node->posid <= 9) {   //  5: 記号,括弧開: paren open
        return 0;                                 //  6: 記号,括弧閉 paren close
    }                                             //  7: 記号,句点: period
                                                  //  8: 記号,空白: whitespace
    return 1;                                     //  9: 記号,読点: comma
}

char** parse(char* str, void* param)
{
    iplus1_japanese_t* jpn = (iplus1_japanese_t*)param;
    
    const mecab_node_t* node;
    
    node = mecab_sparse_tonode(jpn->mecab, str);
    if (node == NULL) {
        fprintf(stderr, "jpn: could not parse: %s\n", str);
        return NULL;
    }
    
    int output_size = 1; // null terminated array... unlike SOME THINGS
    const mecab_node_t* tmpnode;
    for(tmpnode = node; tmpnode; tmpnode = tmpnode->next) {
        output_size++;
    }
    char** output = calloc(sizeof(char*), output_size);
    
    // first node has nothing, starts on node->next
    int i;
    for(i = 0, node = node->next; node; node = node->next, i++) {
        // fucking mecab WHY IS THIS THE ONLY STRING IN THE ENTIRE API THAT IS NOT
        // NULL TERMINATED, WHAT THE FUCK
        // MORE LIKE WHY IS THIS THE ONLY THING I`VE COME ACROSS EVER THAT ISNT
        // NULL TERMINATED, SERIOUSLY, WHO DESIGNED THIS
        // actually now that I think about it, it is probably just a pointer to
        // the input string...
        if (node->length == 0) {
            continue;
        }
        output[i] = calloc(sizeof(char), node->length + 1);
        strncpy(output[i], node->surface, node->length);
        if (!valid_word(output[i], node)) {
            if (node->posid != 7)
                printf("not valid: %s %d\n", output[i], node->posid);
            free(output[i]);
            i--;
        }
    }
    
    return output;
}

int init(iplus1_lang_t* lang)
{
    strcpy(lang->lang, "jpn");
    lang->full_lang = strdup("japanese");
    lang->param = malloc(sizeof(iplus1_japanese_t));
    if (lang->param == NULL)
        return IPLUS1_FAIL;
    lang->parse = parse;
    
    iplus1_japanese_t* jpn = (iplus1_japanese_t*)lang->param;
    jpn->mecab = mecab_new(0, NULL);
    
    
    return IPLUS1_SUCCESS;
}

int destroy(iplus1_lang_t* lang)
{
    iplus1_japanese_t* jpn = (iplus1_japanese_t*)lang->param;
    
    mecab_destroy(jpn->mecab);
    free(lang->full_lang);
    free(lang->param);
    
    return IPLUS1_SUCCESS;
}
