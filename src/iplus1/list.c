#include <stdlib.h>

#include "list.h"
#include "iplus1.h"



int iplus1_list_init(iplus1_list_t* list)
{
    list->length = 0;
    list->node = NULL;
    list->last = NULL;
    
    return IPLUS1_SUCCESS;
}

int iplus1_list_append(iplus1_list_t* list, void* data)
{
    iplus1_list_node_t* node = malloc(sizeof(iplus1_list_node_t));
    node->data = data;
    node->next = NULL;
    
    if (list->node == NULL) {
        list->node = node;
    }
    else {
        list->last->next = node;
    }
    list->last = node;
    list->length++;
    
    return IPLUS1_SUCCESS;
}

int iplus1_list_remove(iplus1_list_t* list, iplus1_list_node_t* node)
{
    iplus1_list_node_t* index;
    for(index = list->node; index != NULL; index = index->next) {
        if (index->next == node) {
            index->next = node->next;
            free(node);
            return 1;
        }
    }
    return 0;
}

int iplus1_list_foreach(iplus1_list_t* list, int (*func)(void*, void*), void* param)
{
    iplus1_list_node_t* node;
    for(node = list->node; node != NULL; node = node->next) {
        func(node->data, param);
    }
    
    return IPLUS1_SUCCESS;
}

int iplus1_list_destroy(iplus1_list_t* list)
{
    iplus1_list_node_t* node;
    for(node = list->node; node != NULL;) {
        iplus1_list_node_t* tmp = node->next;
        free(node);
        node = tmp;
    }
    return IPLUS1_SUCCESS;
}










