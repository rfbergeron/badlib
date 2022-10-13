#ifndef __BADLLIST_H__
#define __BADLLIST_H__
#include <stddef.h>

#include "badlib.h"

#define BLIB_LLIST_EMPTY \
  { NULL, NULL, NULL, 0 }

typedef struct node {
  struct node *next;
  struct node *prev;
  void *data;
} Node;

typedef struct llist {
  Node *anchor;
  BlibDestroyer data_destroy;
  BlibComparator data_compare;
  size_t size;
} LinkedList;

typedef struct llist_iter {
  struct node *node;
  LinkedList *list;
} ListIter;

int llist_init(LinkedList *list, BlibDestroyer dest, BlibComparator comp);
int llist_destroy(LinkedList *list);
int llist_clear(LinkedList *list);
int llist_copy(LinkedList *dest, const LinkedList *src);

int llist_push_front(LinkedList *list, void *element);
void *llist_pop_front(LinkedList *list);
void *llist_front(const LinkedList *list);

int llist_push_back(LinkedList *list, void *element);
void *llist_pop_back(LinkedList *list);
void *llist_back(const LinkedList *list);

int llist_rotate_forwards(LinkedList *list);
int llist_rotate_backwards(LinkedList *list);

void *llist_get(const LinkedList *list, size_t index);
int llist_insert(LinkedList *list, void *element, size_t index);
int llist_delete(LinkedList *list, size_t index);
void *llist_extract(LinkedList *list, size_t index);
size_t llist_find(const LinkedList *list, void *target);
size_t llist_rfind(const LinkedList *list, void *target);
void llist_foreach(LinkedList *list, void (*fn)(void *));
int llist_sort(LinkedList *list, int (*compare)(void *, void *));

size_t llist_size(const LinkedList *list);
int llist_empty(const LinkedList *list);
int llist_status(const LinkedList *list);

ListIter *llist_iter_begin(LinkedList *list);
ListIter *llist_iter_end(LinkedList *list);
ListIter *llist_iter_last(LinkedList *list);
ListIter *llist_iter_at(LinkedList *list, size_t index);
int liter_ins_before(ListIter *iter, void *data);
int liter_ins_after(ListIter *iter, void *data);
int liter_delete(ListIter *iter);
int liter_push_back(ListIter *iter, ListIter **out, size_t count, ...);
int liter_push_front(ListIter *iter, ListIter **out, size_t count, ...);
int liter_advance(ListIter *iter, ptrdiff_t count);
ListIter *liter_next(ListIter *iter, size_t count);
ListIter *liter_prev(ListIter *iter, size_t count);
ListIter *liter_insert(ListIter *iter, size_t count, ...);
ListIter *liter_copy(ListIter *iter);
void *liter_get(ListIter *iter);
int liter_set(ListIter *iter, void *data);
int liter_end(ListIter *iter);
int liter_status(ListIter *iter);
#endif
