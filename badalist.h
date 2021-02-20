#ifndef __BADALIST_H__
#define __BADALIST_H__
#include <stddef.h>

typedef struct alist ArrayList;
struct alist {
  void **data;
  size_t size;
  size_t max_size;
};

int alist_init(ArrayList *list, size_t size);
int alist_destroy(ArrayList *list);

void *alist_get(ArrayList *list, size_t index);
int alist_insert(ArrayList *list, void *data, size_t index);
int alist_delete(ArrayList *list, size_t index);

int alist_resize(ArrayList *list, size_t size);

int alist_clear(ArrayList *list);

size_t alist_size(ArrayList *list);
#endif
