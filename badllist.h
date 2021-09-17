#ifndef __BADLLIST_H__
#define __BADLLIST_H__
#include <stddef.h>

#include "badlib.h"

#define BLIB_LLIST_EMPTY { NULL, 0, 0, NULL, NULL, BLIB_SUCCESS }

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
} LinkedList;

int llist_init(LinkedList *list, BlibDestroyer dest, BlibComparator comp);
int llist_destroy(LinkedList *list);
int llist_copy(LinkedList *dest, const LinkedList *src);

int llist_push_front(LinkedList *list, void *element);
void *llist_pop_front(LinkedList *list);
void *llist_front(const LinkedList *list);

int llist_push_back(LinkedList *list, void *element);
void *llist_pop_back(LinkedList *list);
void *llist_back(const LinkedList *list);

void *llist_get(const LinkedList *list, size_t index);
int llist_insert(LinkedList *list, void *element, size_t index);
int llist_delete(LinkedList *list, size_t index);
void *llist_extract(LinkedList *list, size_t index);
size_t llist_find(const LinkedList *list, void *target);
size_t llist_rfind(const LinkedList *list, void *target);
void llist_foreach(LinkedList *list, void (*fn)(void *));

size_t llist_size(const LinkedList *list);
int llist_empty(const LinkedList *list);
int llist_status(const LinkedList *list);
#endif
