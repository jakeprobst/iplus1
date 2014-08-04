from cffi import FFI
import os

ffi = FFI()
ffi.cdef("""
    typedef struct iplus1_lang_t {
        char lang[4]; // 3 character language code
        char* full_lang;
        
        void* param;
        char** (*parse)(char*, void*);
        int (*destroy)(struct iplus1_lang_t*);
        
        void* dlhandle;
    } iplus1_lang_t;
    
    typedef struct iplus1_tree_node_t {
        void* key;
        void* value;
        
        int height;
        struct iplus1_tree_node_t* left;
        struct iplus1_tree_node_t* right;
    } iplus1_tree_node_t;


    typedef struct iplus1_tree_t {
        iplus1_tree_node_t* root;
        
        int (*compare)(void*, void*);
    } iplus1_tree_t;
    
    typedef struct user_t {
        iplus1_lang_t* nlang;
        iplus1_lang_t* tlang;
        iplus1_tree_t words;
    } user_t;

    typedef struct result_t {
        int64_t nid;
        int64_t tid;
    } result_t;

    void iplus1_init();
    struct iplus1_lang_t* iplus1_get_lang(char*);
    char** iplus1_lang_parse(iplus1_lang_t*, char*);
    void iplus1_lang_parse_free(char**);
    result_t* iplus1_parse_full(char* nlang, char* tlang, char** str);
    void iplus1_parse_full_free(result_t*);
""")

# TODO: figure where exactly these headers will be going
iplus1 = ffi.verify("""
#include "iplus1/iplus1.h"
#include "iplus1/lang.h"
#include "iplus1/tree.h"
#include "iplus1/user.h"
#include "iplus1/parse.h"
""", libraries=["iplus1", 'hiredis'],
     runtime_library_dirs=["."],
     include_dirs=["./src"])


iplus1.iplus1_init()

def lang_parse(langcode, sent):
    lang = iplus1.iplus1_get_lang(langcode)
    if lang == ffi.NULL:
        return []
    words = iplus1.iplus1_lang_parse(lang, sent)

    word_list = []
    i = 0
    while words[i] != ffi.NULL:
        w = ''
        n = 0
        word_list.append(ffi.string(words[i]))
        i += 1

    iplus1.iplus1_lang_parse_free(words)

    return word_list



def parse_full(nlang, tlang, tato_sen):
    sens = []
    for t in tato_sen:
        sens.append(ffi.new("char[]", t))
    sens.append(ffi.NULL)
    sens_arg = ffi.new("char *[]", sens)
    
    results = iplus1.iplus1_parse_full(nlang, tlang, sens_arg)
    r = []
    i = 0
    while results[i].nid != 0:
        r.append({"nid": results[i].nid, "tid": results[i].tid})
        i += 1
    iplus1.iplus1_parse_full_free(results)

    return r




















