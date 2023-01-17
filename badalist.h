#ifndef __BADALIST_H__
#define __BADALIST_H__
#include <stddef.h>

#include "badlib.h"

#define BLIB_ALIST_EMPTY \
  { NULL, 0, 0, 0, NULL, NULL, BLIB_SUCCESS }

typedef struct alist {
  void **data;
  size_t size;
  size_t cap;
} ArrayList;

int alist_init(ArrayList *list, size_t size);
int alist_destroy(ArrayList *list, BlibDestroyer destroyer);
int alist_clear(ArrayList *list, BlibDestroyer destroy);

void *alist_get(const ArrayList *list, size_t index);
int alist_insert(ArrayList *list, void *data, size_t index,
                 BlibDestroyer destroyer);
int alist_delete(ArrayList *list, size_t index, BlibDestroyer destroyer);

int alist_push(ArrayList *list, void *data);
int alist_pop(ArrayList *list, BlibDestroyer destroyer);
void *alist_peek(ArrayList *list);

size_t alist_find(const ArrayList *list, void *target, BlibComparator compare);
size_t alist_rfind(const ArrayList *list, void *target, BlibComparator compare);
void alist_foreach(ArrayList *list, void (*fn)(void *));
void alist_ssort(ArrayList *list, BlibComparator greater_equal);
int alist_resize(ArrayList *list, size_t size, BlibDestroyer destroyer);
size_t alist_count(ArrayList *list);

size_t alist_size(const ArrayList *list);
size_t alist_cap(const ArrayList *list);
int alist_empty(const ArrayList *list);
int alist_status(const ArrayList *list);
#endif
