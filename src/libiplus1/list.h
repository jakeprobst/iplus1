#ifndef _LIST_H_
#define _LIST_H_



typedef struct iplus1_list_node_t {
    void* data;
    struct iplus1_list_node_t* next;
} iplus1_list_node_t;

typedef struct iplus1_list_t {
    int length;
    iplus1_list_node_t* node;
    iplus1_list_node_t* last;
} iplus1_list_t;


int iplus1_list_init(iplus1_list_t*);
int iplus1_list_append(iplus1_list_t*, void*);
int iplus1_list_remove(iplus1_list_t*, iplus1_list_node_t*);
int iplus1_list_foreach(iplus1_list_t*, int(*func)(void*, void*), void*);
int iplus1_list_destroy(iplus1_list_t*);















#endif /* _LIST_H_ */
