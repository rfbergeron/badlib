#ifndef __BADLLIST_H__
#define __BADLLIST_H__
#include <stddef.h>

#include "badlib.h"

typedef struct node {
  struct node *next;
  struct node *prev;
  void *data;
} Node;

typedef struct llist {
  Node *anchor;
  size_t size;
  size_t max_size;
  BlibDestroyer data_destroy;
  BlibComparator data_compare;
  BlibError last_status;
} LinkedList;

int llist_init(LinkedList *list, BlibDestroyer dest, BlibComparator comp);
int llist_destroy(LinkedList *list);

int llist_push_front(LinkedList *list, void *element);
void *llist_pop_front(LinkedList *list);
void *llist_front(LinkedList *list);

int llist_push_back(LinkedList *list, void *element);
void *llist_pop_back(LinkedList *list);
void *llist_back(LinkedList *list);

void *llist_get(LinkedList *list, size_t index);
int llist_insert(LinkedList *list, void *element, size_t index);
int llist_delete(LinkedList *list, size_t index);

void llist_foreach(LinkedList *list, void(*fn)(void* data, size_t index));
size_t llist_size(LinkedList *list);
int llist_empty(LinkedList *list);
#endif
