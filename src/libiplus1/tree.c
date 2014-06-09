#include <stdlib.h>

#include "iplus1.h"
#include "tree.h"

int iplus1_tree_init(iplus1_tree_t* tree, int (*compare)(void*, void*))
{
    tree->root = NULL;
    tree->compare = compare;
    
    return IPLUS1_SUCCESS;
}

int _iplus1_tree_node_destroy(iplus1_tree_node_t* node)
{
    if (node == NULL)
        return IPLUS1_FAIL;
    _iplus1_tree_node_destroy(node->left);
    _iplus1_tree_node_destroy(node->right);
    free(node);
    
    return IPLUS1_SUCCESS;
}

int iplus1_tree_destroy(iplus1_tree_t* tree)
{
    _iplus1_tree_node_destroy(tree->root);
    
    return IPLUS1_SUCCESS;
}

int _max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

int _height(iplus1_tree_node_t* node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

int _balance(iplus1_tree_node_t* node)
{
    if (node == NULL)
        return 0;
    return _height(node->left) - _height(node->right);
}

iplus1_tree_node_t* _rotate_left(iplus1_tree_node_t* node)
{
    iplus1_tree_node_t* tmp = node->right;
    node->right = tmp->left;
    tmp->left = node;
    
    node->height = _max(_height(node->left), _height(node->right)) + 1;
    tmp->height = _max(_height(tmp->left), _height(tmp->right)) + 1;
    return tmp;
}

iplus1_tree_node_t* _rotate_right(iplus1_tree_node_t* node)
{
    iplus1_tree_node_t* tmp = node->left;
    node->left = tmp->right;
    tmp->right = node;
    
    node->height = _max(_height(node->left), _height(node->right)) + 1;
    tmp->height = _max(_height(tmp->left), _height(tmp->right)) + 1;
    return tmp;
}


iplus1_tree_node_t* _tree_insert(iplus1_tree_node_t* root, iplus1_tree_node_t* node, int (*compare)(void*, void*))
{
    if (root == NULL) {
        return node;
    }
    
    int comp = compare(root->key, node->key);
    if (comp > 0) { // root < node
        root->left = _tree_insert(root->left, node, compare);
    }
    else if (comp < 0){ // root > node
        root->right = _tree_insert(root->right, node, compare);
    }
    else {
        // equal, just ignore it I guess. (this shouldnt happen)
        // memory leak if an item is added where a key already exists
    }
    
    root->height = _max(_height(root->right), _height(root->left)) + 1;
    int balance = _balance(root);
    
    if (balance > 1 && compare(root->left->key, node->key) > 0) { // root > node
        return _rotate_right(root);
    }
    if (balance > 1 && compare(root->left->key, node->key) < 0) { // root < node
        root->left = _rotate_left(root->left);
        return _rotate_right(root);
    }
    if (balance < -1 && compare(root->right->key, node->key) < 0) { // root < node
        return _rotate_left(root);
    }
    if (balance < -1 && compare(root->right->key, node->key) > 0) { // root < node
        root->right = _rotate_right(root->right);
        return _rotate_left(root);
    }
    
    return root;
}

int iplus1_tree_insert(iplus1_tree_t* tree, void* key, void* value)
{
    iplus1_tree_node_t* node = malloc(sizeof(iplus1_tree_node_t));
    if (node == NULL)
        return IPLUS1_FAIL;
    node->key = key;
    node->value = value;
    node->height = 1;
    node->left = node->right = NULL;
    
    tree->root = _tree_insert(tree->root, node, tree->compare);
    
    return IPLUS1_SUCCESS;
}

void* iplus1_tree_get(iplus1_tree_t* tree, void* key)
{
    iplus1_tree_node_t* cursor = tree->root;
    for(;;) {
        if (cursor == NULL) {
            return NULL;
        }
        if (tree->compare(cursor->key, key) > 0) {
            cursor = cursor->left;
        }
        else if (tree->compare(cursor->key, key) < 0) {
            cursor = cursor->right;
        }
        else {
            return cursor->value;
        }
    }
    return NULL;
}

int _iplus1_tree_node_foreach_inorder(iplus1_tree_node_t* node, int (*func)(void*, void*), void* param)
{
    
    if (node != NULL) {
        _iplus1_tree_node_foreach_inorder(node->left, func, param);
        func(node->value, param);
        _iplus1_tree_node_foreach_inorder(node->right, func, param);
    }
    return IPLUS1_SUCCESS;
}

int _iplus1_tree_node_foreach_postorder(iplus1_tree_node_t* node, int (*func)(void*, void*), void* param)
{
    
    if (node != NULL) {
        _iplus1_tree_node_foreach_postorder(node->left, func, param);
        _iplus1_tree_node_foreach_postorder(node->right, func, param);
        func(node->value, param);
    }
    return IPLUS1_SUCCESS;
}

int iplus1_tree_foreach_inorder(iplus1_tree_t* tree, int (*func)(void*, void*), void* param)
{
    _iplus1_tree_node_foreach_inorder(tree->root, func, param);
    return IPLUS1_SUCCESS;
}

int iplus1_tree_foreach_postorder(iplus1_tree_t* tree, int (*func)(void*, void*), void* param)
{
    _iplus1_tree_node_foreach_postorder(tree->root, func, param);
    return IPLUS1_SUCCESS;
}




