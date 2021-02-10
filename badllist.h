#ifndef __BADLLIST_H__
#define __BADLLIST_H__
#include <stddef.h>

typedef struct node Node;
struct node {
    Node *next;
    Node *prev;
    void *data;
};

typedef struct llist LinkedList;
struct llist {
    Node *anchor;
    size_t size;
};

int llist_init(LinkedList *list);

int llist_destroy(LinkedList *list);

LinkedList *llist_construct();

int llist_push(LinkedList *list, void *element);

void *llist_pop(LinkedList *list);

void *llist_peek(LinkedList *list);

int llist_push_back(LinkedList *list, void *element);

void *llist_pop_back(LinkedList *list);

void *llist_peek_back(LinkedList *list);

void *llist_get(LinkedList *list, size_t index);

int llist_insert(LinkedList *list, void *element, size_t index);

int llist_delete(LinkedList *list, size_t index);

int llist_clear(LinkedList *list);

size_t llist_size(LinkedList *list);

int llist_empty(LinkedList *list);
#endif
