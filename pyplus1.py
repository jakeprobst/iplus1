from cffi import FFI

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

    void iplus1_init();
    struct iplus1_lang_t* iplus1_get_lang(char*);
    char** iplus1_lang_parse(iplus1_lang_t*, char*);
    void iplus1_lang_parse_free(char**);
""")

# TODO: figure where exactly these headers will be going
iplus1 = ffi.verify("""
#include "iplus1/iplus1.h"
#include "iplus1/lang.h"
""", libraries=["iplus1"],
     library_dirs=[""])

iplus1.iplus1_init()

def lang_parse(langcode, sent):
    lang = iplus1.iplus1_get_lang(langcode)
    words = iplus1.iplus1_lang_parse(lang, sent)

    word_list = []
    i = 0
    while words[i] != ffi.NULL:
        w = ''
        n = 0
        while words[i][n] != '\x00':
            w += str(words[i][n])
            n += 1
        word_list.append(w)
        i += 1

    iplus1.iplus1_lang_parse_free(words)

    return word_list

