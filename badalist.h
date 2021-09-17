#ifndef __BADALIST_H__
#define __BADALIST_H__
#include <stddef.h>

#include "badlib.h"

#define BLIB_ALIST_EMPTY { NULL, 0, 0, 0, NULL, NULL, BLIB_SUCCESS }

typedef struct alist {
  void **data;
  size_t size;
  size_t max_size;
  size_t count;
  BlibDestroyer data_destroy;
  BlibComparator data_compare;
} ArrayList;

int alist_init(ArrayList *list, size_t size, BlibDestroyer dest,
               BlibComparator comp);
int alist_destroy(ArrayList *list);

void *alist_get(const ArrayList *list, size_t index);
int alist_insert(ArrayList *list, void *data, size_t index);
int alist_delete(ArrayList *list, size_t index);
size_t alist_find(const ArrayList *list, void *target);
size_t alist_rfind(const ArrayList *list, void *target);
int alist_resize(ArrayList *list, size_t size);
void alist_foreach(ArrayList *list, void (*fn)(void *));

size_t alist_size(const ArrayList *list);
int alist_empty(const ArrayList *list);
int alist_status(const ArrayList *list);
#endif
