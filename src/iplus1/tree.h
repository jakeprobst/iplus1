#ifndef _TREE_H_
#define _TREE_H_

// some sort of avl tree thing that I attempted to implment after reading about
// it on wikipedia. it seems to be correct.
// used for reference:
// http://www.geeksforgeeks.org/avl-tree-set-1-insertion/ 
// http://www.geeksforgeeks.org/avl-tree-set-2-deletion/
// http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_avl.aspx

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



int iplus1_tree_init(iplus1_tree_t*, int (*compare)(void*, void*));
int iplus1_tree_destroy(iplus1_tree_t*);

int iplus1_tree_insert(iplus1_tree_t*, void*, void*);
int iplus1_tree_remove(iplus1_tree_t*, void*, int (*func)(void*, void*, void*));
void* iplus1_tree_get(iplus1_tree_t*, void*);

int iplus1_tree_foreach_inorder(iplus1_tree_t*, int (*func)(void*, void*, void*), void*);
int iplus1_tree_foreach_postorder(iplus1_tree_t*, int (*func)(void*, void*, void*), void*);

int iplus1_tree_compare_int(void*, void*);
int iplus1_tree_compare_str(void*, void*);

int iplus1_tree_free_key(void*, void*, void*);
int iplus1_tree_free_keyvalue(void*, void*, void*);




#endif /* _TREE_H_ */
