#ifndef __BADALIST_H__
#define __BADALIST_H__
#include <stddef.h>

#include "badlib.h"

typedef struct alist {
  void **data;
  size_t size;
  size_t max_size;
  size_t count;
  BlibDestroyer data_destroy;
  BlibComparator data_compare;
  BlibError last_status;
} ArrayList;

int alist_init(ArrayList *list, size_t size, BlibDestroyer dest,
               BlibComparator comp);
int alist_destroy(ArrayList *list);

void *alist_get(ArrayList *list, size_t index);
int alist_insert(ArrayList *list, void *data, size_t index);
int alist_delete(ArrayList *list, size_t index);

int alist_resize(ArrayList *list, size_t size);

size_t alist_size(ArrayList *list);
int alist_empty(ArrayList *list);
#endif
