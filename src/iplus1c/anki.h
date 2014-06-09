#ifndef _ANKI_H_
#define _ANKI_H_

#include <sqlite3.h>


typedef struct anki_t {
    sqlite3* db;
} anki_t;


int anki_init(anki_t*, char*);
int anki_destroy(anki_t*);


int anki_foreach(anki_t*, int (*func)(char*, void*), void*);










#endif /* _ANKI_H_ */
