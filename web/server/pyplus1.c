#include <Python.h>
#include "iplus1.h"
#include "lang.h"

static PyObject* pyplus1_lang_parse(PyObject* self, PyObject* args)
{
    char* lang_code;
    char* str;
    
    
    if (!PyArg_ParseTuple(args, "ss", &lang_code, &str)) {
        return NULL;
    }
    
    iplus1_lang_t* lang = iplus1_get_lang(lang_code);
    
    char** words = iplus1_lang_parse(lang, str);
    
    PyObject* list = PyList_New(0);
    
    int i;
    for(i = 0; words[i] != NULL; i++) {
        PyList_Append(list, Py_BuildValue("s", words[i]));
    }
    
    iplus1_lang_parse_free(words);
    
    return list;
}


static PyMethodDef pyplus1_methods[] = {
    {"lang_parse", pyplus1_lang_parse, METH_VARARGS, "return stemmed and tokenized list of words"},
    {NULL, NULL, 0, NULL}
};



PyMODINIT_FUNC
initpyplus1(void)
{
    iplus1_init();
    
    Py_InitModule("pyplus1", pyplus1_methods);
}






