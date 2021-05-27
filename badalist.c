#include "badalist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "badlib.h"

/* internal functions */
int alist_valid(ArrayList *list) {
  if (!list) {
    return 0;
  } else if (!(list->data) || !(list->size > 0) || !(list->max_size > 0)) {
    list->last_status = BLIB_INVALID_STRUCT;
    return 0;
  } else {
    return 1;
  }
}

/* external functions */
int alist_init(ArrayList *list, size_t size, BlibDestroyer dest,
               BlibComparator comp) {
  if (size < 1) return 1;
  size_t to_alloc = 1;
  while (to_alloc < size) to_alloc = to_alloc * 2;
  list->data = calloc(to_alloc, sizeof(void *));
  list->size = size;
  list->max_size = to_alloc;
  list->count = 0;
  list->data_destroy = dest;
  list->data_compare = comp;
  list->last_status = BLIB_SUCCESS;
  return 0;
}

int alist_destroy(ArrayList *list) {
  if (!alist_valid(list)) return 1;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    if ((list->data_destroy) && list->data[i]) {
      (list->data_destroy)(list->data[i]);
    }
  }
  free(list->data);
  return 0;
}

void *alist_get(ArrayList *list, size_t index) {
  if (!alist_valid(list)) {
    return NULL;
  } else if (index >= list->size) {
    list->last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  } else if (!(list->data[index])) {
    list->last_status = W_BLIB_NOT_FOUND;
    return NULL;
  }

  return list->data[index];
}

int alist_insert(ArrayList *list, void *element, size_t index) {
  if (!alist_valid(list)) {
    return 1;
  } else if (index >= list->size) {
    list->last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  }

  /* remember to get rid of the old element if it exists */
  if (list->data_destroy && list->data[index]) {
    (list->data_destroy)(list->data[index]);
    --(list->count);
  }

  list->data[index] = element;
  ++(list->count);
  return 0;
}

int alist_delete(ArrayList *list, size_t index) {
  if (!alist_valid(list)) {
    return 1;
  } else if (index >= list->size) {
    list->last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  } else if (!(list->data[index])) {
    list->last_status = W_BLIB_NOT_FOUND;
    return 1;
  }

  if (list->data_destroy) (list->data_destroy)(list->data[index]);
  list->data[index] = NULL;
  --(list->count);
  return 0;
}

size_t alist_find(ArrayList *list, void *target) {
  if (!alist_valid(list)) {
    return 1;
  } else if (!target) {
    return 1;
  }

  size_t i;
  for (i = 0; i < alist_size(list); ++i) {
    void *element = list->data[i];
    if ((list->data_compare)(target, element)) {
      return i;
    }
  }
  return -1;
}

size_t alist_rfind(ArrayList *list, void *target) {
  if (!alist_valid(list)) {
    return 1;
  } else if (!target) {
    return 1;
  }

  size_t i;
  for (i = alist_size(list); i >= 0; --i) {
    void *element = list->data[i];
    if ((list->data_compare)(target, element)) {
      return i;
    }
  }
  return -1;
}

void alist_foreach(ArrayList *list, void(*fn)(void*)) {
    if (!alist_valid(list) || !fn) return;
    size_t i;
    for (i = 0; i < list->size; ++i) {
        void *element = alist_get(list, i);
        (fn)(element);
    }
}


int alist_resize(ArrayList *list, size_t size) {
  if (!alist_valid(list)) {
    return 1;
  } else if (size < 1) {
    list->last_status = BLIB_INVALID_SIZE;
    return 1;
  }

  if (size < list->max_size) {
    list->size = size;
  } else {
    size_t to_alloc = 1;
    while (to_alloc < size) to_alloc *= 2;
    void **new_data = calloc(to_alloc, sizeof(void *));
    memcpy(new_data, list->data, list->size * sizeof(void *));
    free(list->data);
    list->data = new_data;
    list->size = size;
    list->max_size = to_alloc;
  }

  return 0;
}

size_t alist_size(ArrayList *list) { return list->size; }
int alist_empty(ArrayList *list) { return list->count == 0; }
