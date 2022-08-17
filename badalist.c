#include "badalist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "badlib.h"

static BlibError last_status = BLIB_SUCCESS;

/* internal functions */
int alist_valid(const ArrayList *list) {
  if (!list) {
    return 0;
  } else if (list->data == NULL && list->size > 0) {
    last_status = BLIB_INVALID_STRUCT;
    return 0;
  } else {
    return 1;
  }
}

/* external functions */
int alist_init(ArrayList *list, size_t size) {
  list->data = calloc(size, sizeof(void *));
  list->size = size;
  list->count = 0;
  last_status = BLIB_SUCCESS;
  return 0;
}

int alist_destroy(ArrayList *list, BlibDestroyer destroy) {
  if (!alist_valid(list)) return 1;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    if (destroy && list->data[i]) {
      destroy(list->data[i]);
    }
  }
  free(list->data);
  return 0;
}

void *alist_get(const ArrayList *list, size_t index) {
  if (!alist_valid(list)) {
    return NULL;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  } else if (!(list->data[index])) {
    last_status = W_BLIB_NOT_FOUND;
    return NULL;
  }

  return list->data[index];
}

int alist_insert(ArrayList *list, void *element, size_t index,
                 BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return 1;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  }

  if (list->data[index]) {
    if (destroy) destroy(list->data[index]);
    --list->count;
  }

  if (element != NULL) ++list->count;
  list->data[index] = element;
  return 0;
}

int alist_delete(ArrayList *list, size_t index, BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return 1;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  }

  if (list->data[index]) {
    if (destroy) destroy(list->data[index]);
    --list->count;
  }

  list->data[index] = NULL;
  return 0;
}

size_t alist_find(const ArrayList *list, void *target, BlibComparator compare) {
  if (!alist_valid(list)) {
    return 1;
  } else if (!target) {
    return 1;
  }

  size_t i;
  for (i = 0; i < alist_size(list); ++i) {
    void *element = list->data[i];
    if (element && compare(target, element)) {
      return i;
    }
  }
  return -1;
}

size_t alist_rfind(const ArrayList *list, void *target,
                   BlibComparator compare) {
  if (!alist_valid(list)) {
    return 1;
  } else if (!target) {
    return 1;
  }

  size_t i, list_size = alist_size(list);
  for (i = 1; i <= list_size; ++i) {
    void *element = list->data[list_size - i];
    if (element && compare(target, element)) {
      return list_size - i;
    }
  }
  return list_size;
}

void alist_foreach(ArrayList *list, void (*fn)(void *)) {
  if (!alist_valid(list) || !fn) return;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    void *element = alist_get(list, i);
    if (element) fn(element);
  }
}

void alist_ssort(ArrayList *list, BlibComparator greater_equal) {
  if (!alist_valid(list) || !greater_equal) return;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    size_t min_index = i;
    size_t j;
    for (j = i; j < list->size; ++j) {
      void *current = list->data[j];
      if (greater_equal(list->data[min_index], current)) min_index = j;
    }
    void *temp = list->data[min_index];
    list->data[min_index] = list->data[i];
    list->data[i] = temp;
  }
}

int alist_resize(ArrayList *list, size_t size, BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return 1;
  }

  if (size > list->size) {
    list->data = realloc(list->data, size);
    void **first_new = list->data + list->size;
    size_t length_new = size - list->size;
    memset(first_new, 0, sizeof(void *) * length_new);
  } else {
    size_t i;
    for (i = size; i < list->size; ++i) {
      if (list->data[i]) {
        --list->count;
        if (destroy) destroy(list->data[i]);
      }
    }
    list->data = realloc(list->data, size);
  }
  list->size = size;

  return 0;
}

size_t alist_size(const ArrayList *list) { return list->size; }
int alist_empty(const ArrayList *list) { return list->count == 0; }
size_t alist_count(const ArrayList *list) { return list->count; }
int alist_status(const ArrayList *list) {
  if (!alist_valid(list)) return BLIB_INVALID_STRUCT;
  /* TODO(Robert): more robust (threadsafe?) way of getting status */
  return last_status;
}
