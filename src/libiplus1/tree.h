#ifndef _TREE_H_
#define _TREE_H_

// some sort of avl tree thing that I attempted to implment after reading about
// it on wikipedia. it seems to be correct.
// used for reference:
// http://www.geeksforgeeks.org/avl-tree-set-1-insertion/ 
// http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_avl.aspx

// no remove function cause it isn`t needed and I`m lazy

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
void* iplus1_tree_get(iplus1_tree_t*, void*);

int iplus1_tree_foreach_inorder(iplus1_tree_t*, int (*func)(void*, void*), void*);
int iplus1_tree_foreach_postorder(iplus1_tree_t*, int (*func)(void*, void*), void*);









#endif /* _TREE_H_ */
