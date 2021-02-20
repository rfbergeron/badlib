#include "badalist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* internal functions */
int alist_valid(ArrayList *list) {
  return list != NULL && list->data != NULL && list->size > 0 &&
         list->max_size > 0;
}

int alist_init(ArrayList *list, size_t size) {
  if (size < 1) return 1;
  size_t to_alloc = 1;
  while (to_alloc < size) to_alloc = to_alloc * 2;
  list->data = malloc(to_alloc * sizeof(void *));
  list->size = size;
  list->max_size = to_alloc;
  return 0;
}

int alist_destroy(ArrayList *list) {
  if (!alist_valid(list)) return 1;
  free(list->data);
}

void *alist_get(ArrayList *list, size_t index) {
  if (!alist_valid(list)) return NULL;
  if (index >= list->size) return NULL;
  return list->data[index];
}

int alist_insert(ArrayList *list, void *element, size_t index) {
  if (!alist_valid(list)) return 1;
  if (index >= list->size) return 1;
  list->data[index] = element;
  return 0;
}

int alist_delete(ArrayList *list, size_t index) {
  if (!alist_valid(list)) return 1;
  if (index >= list->size) return 1;
  list->data[index] = NULL;
  return 0;
}

int alist_resize(ArrayList *list, size_t size) {
  if (!alist_valid(list)) return 1;
  if (size < 1) return 1;

  if (size < list->max_size) {
    list->size = size;
  } else {
    size_t to_alloc = 1;
    while (to_alloc < size) to_alloc = to_alloc * 2;
    void **new_data = malloc(to_alloc * sizeof(void *));
    memcpy(new_data, list->data, list->size * sizeof(void *));
    free(list->data);
    list->data = new_data;
    list->size = size;
    list->max_size = to_alloc;
  }
}

int alist_clear(ArrayList *list) {
  if (!alist_valid(list)) return 1;
  memset(list->data, 0, list->size * sizeof(void *));
  return 0;
}

size_t alist_size(ArrayList *list) { return list->size; }
