#ifndef _APKG_H_
#define _APKG_H_

#include <sqlite3.h>


typedef struct apkg_t {
    sqlite3* db;
    
    
    
    
    
} apkg_t;




int apkg_init(apkg_t*, char*);
int apkg_destroy(apkg_t*);

int apkg_add_card(apkg_t*, char*, char*);












#endif /* _APKG_H_ */
