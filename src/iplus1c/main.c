#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unicode/uclean.h>

#include "iplus1.h"
#include "collection.h"

#include "anki.h"
#include "result.h"


int _compare_int(void* a, void* b)
{
    int* at = a;
    int* bt = b;
    return *at - *bt;
}

int _compare_str(void* a, void* b)
{
    char* at = a;
    char* bt = b;
    return strcmp(at, bt);
}

int _free_sentences(void* p, void* param)
{
    iplus1_sentence_t* sen = (iplus1_sentence_t*)p;
    iplus1_sentence_destroy(sen);
    free(sen);
    return IPLUS1_SUCCESS;
}

int _free_words(void* p, void* param)
{
    iplus1_word_t* word = (iplus1_word_t*)p;
    iplus1_word_destroy(word);
    free(word);
    return IPLUS1_SUCCESS;
}

int _free_search_results(void* p, void* param)
{
    search_result_t* result = (search_result_t*)p;
    search_result_destroy(result);
    free(result);
    return IPLUS1_SUCCESS;
}

void usage(char* name)
{
    printf("%s -n [native language] -t [target language] -i [anki deck]\n", name);
    printf("  -l\t\tlist languages\n");
    
}

void list_languages()
{
    iplus1_t iplus1;
    iplus1_init(&iplus1);
    
    int i;
    for(i = 0; iplus1.language[i] != NULL; i++) {
        printf("%s\t%s\n", iplus1.language[i]->lang, iplus1.language[i]->full_lang);
    }
    
    iplus1_destroy(&iplus1);
}

char* readline(FILE* fd)
{
    int len = 0;
    int start = ftell(fd);
    
    while(1) {
        int c = fgetc(fd);
        if (c == '\n' || c == EOF) {
            break;
        }
        len++;
    }
    
    if (len == 0) {
        return NULL;
    }
    
    char* buf = malloc(len+1);
    fseek(fd, start, SEEK_SET);
    fread(buf, len, 1, fd);
    buf[len] = '\0';
    
    fgetc(fd); // get that linebreak out of there
        
    return buf;
}

iplus1_list_t* get_sentences_with_new_words(char* path, iplus1_tree_t* words, iplus1_lang_t* target_lang, iplus1_lang_t* native_lang)
{
    iplus1_list_t* out = malloc(sizeof(iplus1_list_t));
    iplus1_list_init(out);
    
    FILE* sentence_db = fopen(path, "r");
    if (sentence_db == NULL) {
        fprintf(stderr, "could not load '%s'\n", path);
        return NULL;
    }
    char* line;
    while ((line = readline(sentence_db)) != NULL) {
        char* s;
        char* position;
        int id;
        char* sentence = NULL;
        s = strtok_r(line, "\t", &position);
        if (s != NULL) {
            id = atol(s);
        }
        else {
            goto error;
        }
        s = strtok_r(NULL, "\t", &position);
        if (s != NULL) {
        }
        else {
            goto error;
        }
        s = strtok_r(NULL, "\t", &position);
        if (s != NULL) {
            sentence = strdup(s);
        }
        else {
            goto error;
        }
        
        iplus1_sentence_t* sen = malloc(sizeof(iplus1_sentence_t));
        if (iplus1_sentence_init(sen, target_lang, sentence) == IPLUS1_FAIL) {
            free(sen);
            goto error;
        }
        sen->id = id;
        
        
        search_result_t* result = malloc(sizeof(search_result_t));
        search_result_init(result);
        
        int i;
        for(i = 0; sen->words[i] != NULL; i++) {
            if (iplus1_tree_get(words, sen->words[i]) == NULL) {
                char* word = strdup(sen->words[i]);
                iplus1_list_append(result->new_words, word);
            }
        }
        
        if (result->new_words->length > 0) {
            result->original = strdup(sen->str);
            
            while ((s = strtok_r(NULL, "\t", &position)) != NULL) {
                char* sub_pos;
                
                char* lang = strtok_r(s, ":", &sub_pos);
                char* tr_sen = strtok_r(NULL, ":", &sub_pos);
                
                if (strncmp(native_lang->lang, lang, 3) == 0) {
                    result->translated = strdup(tr_sen);
                    break;
                }
            }
            
            if (result->translated) {
                iplus1_list_append(out, result);
            }
            else {
                search_result_destroy(result);
                free(result);
            }
        }
        else {
            search_result_destroy(result);
            free(result);
        }
                
        iplus1_sentence_destroy(sen);
        free(sen);

error:
        free(sentence);
        free(line);
    }
    fclose(sentence_db);
    
    return out;
}



int main(int argc, char** argv)
{
    char* nlang = NULL;
    char* tlang = NULL;
    int list_lang = 0;
    char* anki_deck_path;
    
    char c;
    while ((c = getopt(argc, argv, "n:t:i:l")) != -1) {
        switch (c) {
            case 'n':
                nlang = strdup(optarg);
                break;
            case 't':
                tlang = strdup(optarg);
                break;
            case 'i':
                anki_deck_path = strdup(optarg);
                break;
            case 'l':
                list_lang = 1;
                break;
            case '?':
                if (optopt == 'n' || optopt == 't'){
                }
                else {
                    fprintf(stderr, "invalid argument: %c\n", optopt);
                }
                return -1;
                break;
            default:
                fprintf(stderr, "defaulted\n");
                break;
        }
    }
    if (list_lang == 1) {
        list_languages();
        return 0;
    }
    
    if (nlang == NULL || tlang == NULL || anki_deck_path == NULL) {
        usage(argv[0]);
        return -1;
    }
    
    iplus1_t iplus1;
    iplus1_init(&iplus1);
    
    iplus1_lang_t* native_lang;
    iplus1_lang_t* target_lang;
    
    if ((native_lang = iplus1_get_lang(&iplus1, nlang)) == NULL) {
        fprintf(stderr, "%s not a valid language\n", native_lang->lang);
        return -1;
    }
    if ((target_lang = iplus1_get_lang(&iplus1, tlang)) == NULL) {
        fprintf(stderr, "%s not a valid language\n", target_lang->lang);
        return -1;
    }
    free(nlang);
    free(tlang);
    
    anki_t anki_deck;
    if (anki_init(&anki_deck, anki_deck_path) == IPLUS1_FAIL) {
        fprintf(stderr, "could not load anki deck: %s\n", anki_deck_path);
        return -1;
    }
    free(anki_deck_path);
    
    iplus1_list_t anki_cards;
    iplus1_tree_t anki_words;
    iplus1_list_init(&anki_cards);
    iplus1_tree_init(&anki_words, &_compare_str);
    
    int _add_to_list(char* str, void* param)
    {
        char* pos;
        char* ins = strtok_r(str, "\x1f", &pos);
        
        iplus1_sentence_t* sen = malloc(sizeof(iplus1_sentence_t));
        iplus1_sentence_init(sen, target_lang, ins);
        
        iplus1_list_append(&anki_cards, sen);
        
        int i;
        for(i = 0; sen->words[i] != NULL; i++) {
            iplus1_word_t* word = iplus1_tree_get(&anki_words, sen->words[i]);
            if (word == NULL) {
                word = malloc(sizeof(iplus1_word_t));
                iplus1_word_init(word, sen->words[i]);
                iplus1_tree_insert(&anki_words, word->str, word);
            }
            iplus1_word_add_sentence(word, sen);
        }
        return IPLUS1_SUCCESS;
    }
    
    anki_foreach(&anki_deck, &_add_to_list, NULL);

    char path[FILENAME_MAX];
    sprintf(path, "data/%s.csv", target_lang->lang);
    iplus1_list_t* new_sentences = get_sentences_with_new_words(path, &anki_words, target_lang, native_lang);
    
    
    
    iplus1_list_node_t* node;
    for(node = new_sentences->node; node != NULL; node = node->next) {
        search_result_t* result = node->data;
        
        printf("'%s' -> '%s': [", result->original, result->translated);
        iplus1_list_node_t* sub_node;
        for(sub_node = result->new_words->node; sub_node != NULL; sub_node = sub_node->next) {
            char* word = sub_node->data;
            printf("'%s', ", word);
        }
        printf("\b\b]\n");
    }

    iplus1_list_foreach(new_sentences, &_free_search_results, NULL);
    iplus1_list_destroy(new_sentences);
    free(new_sentences);
    
    iplus1_list_foreach(&anki_cards, &_free_sentences, NULL);
    iplus1_list_destroy(&anki_cards);
    
    iplus1_tree_foreach_postorder(&anki_words, &_free_words, NULL);
    iplus1_tree_destroy(&anki_words);
    
    
    anki_destroy(&anki_deck);
    iplus1_destroy(&iplus1);
    
    u_cleanup();
    return 0;
}
