#include "badalist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "badlib.h"

static BlibError last_status = BLIB_SUCCESS;

/* internal functions */
static int alist_valid(const ArrayList *list) {
  if (list == NULL || list->cap == 0) {
    last_status = BLIB_INVALID_STRUCT;
    return 0;
  } else if (list->data == NULL) {
    last_status = BLIB_UNINITIALIZED_ARG;
    return 0;
  } else {
    return 1;
  }
}

/* external functions */
int alist_init(ArrayList *list, size_t size) {
  if (list == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }

  list->size = size;
  list->cap = 1;
  while (list->cap < list->size) list->cap <<= 1;
  list->data = malloc(sizeof(void *) * list->cap);
  if (list->data == NULL) {
    last_status = BLIB_ALLOC_FAIL;
    return -1;
  }
  memset(list->data, 0, sizeof(void *) * list->size);
  last_status = BLIB_SUCCESS;
  return 0;
}

int alist_destroy(ArrayList *list, BlibDestroyer destroy) {
  if (list == NULL)
    return 0;
  else if (!alist_valid(list))
    return -1;

  size_t i;
  for (i = 0; i < list->size; ++i)
    if (destroy && list->data[i]) DESTROY_DATA(destroy, list->data[i]);

  free(list->data);
  /* paranoid free */
  memset(list, 0, sizeof(ArrayList));
  return 0;
}

int alist_clear(ArrayList *list, BlibDestroyer destroy) {
  if (!alist_valid(list)) return -1;

  size_t i;
  for (i = 0; i < list->size; ++i) {
    if (list->data[i]) {
      if (destroy) DESTROY_DATA(destroy, list->data[i]);
      list->data[i] = NULL;
    }
  }

  return 0;
}

void *alist_get(const ArrayList *list, size_t index) {
  if (!alist_valid(list)) {
    return NULL;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  } else {
    void *ret = list->data[index];
    last_status = ret == NULL ? W_BLIB_NOT_FOUND : BLIB_SUCCESS;
    return ret;
  }
}

int alist_insert(ArrayList *list, void *element, size_t index,
                 BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return -1;
  } else if (index > list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return -1;
  } else if (index == list->size) {
    if (index == list->cap) {
      void **new_data = realloc(list->data, sizeof(void *) * (list->cap <<= 1));
      if (new_data == NULL) {
        last_status = BLIB_ALLOC_FAIL;
        return -1;
      }
      list->data = new_data;
    }
    ++list->size;
  } else if (list->data[index] && destroy) {
    /* don't call destroy on list->data[list->size]; it is garbage */
    DESTROY_DATA(destroy, list->data[index]);
  }

  list->data[index] = element;
  return 0;
}

int alist_delete(ArrayList *list, size_t index, BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return -1;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return -1;
  }

  if (list->data[index]) {
    if (destroy) DESTROY_DATA(destroy, list->data[index]);
    list->data[index] = NULL;
  }

  return 0;
}

int alist_push(ArrayList *list, void *data) {
  if (!alist_valid(list)) return -1;

  if (list->size == list->cap) {
    void **new_data = realloc(list->data, sizeof(void *) * (list->cap <<= 1));
    if (new_data == NULL) {
      last_status = BLIB_ALLOC_FAIL;
      return -1;
    }
    list->data = new_data;
  }
  list->data[list->size++] = data;
  return 0;
}

int alist_pop(ArrayList *list, BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return -1;
  } else if (list->size == 0) {
    last_status = BLIB_EMPTY;
    return -1;
  } else {
    --list->size;
    if (list->data[list->size] && destroy)
      DESTROY_DATA(destroy, list->data[list->size]);
    return 0;
  }
}

void *alist_peek(ArrayList *list) {
  if (!alist_valid(list)) {
    return NULL;
  } else if (list->size == 0) {
    last_status = BLIB_EMPTY;
    return NULL;
  } else {
    void *ret = list->data[list->size - 1];
    last_status = ret == NULL ? W_BLIB_NOT_FOUND : BLIB_SUCCESS;
    return ret;
  }
}

size_t alist_find(const ArrayList *list, void *target, BlibComparator compare) {
  if (!alist_valid(list)) {
    return -1;
  }

  size_t i;
  for (i = 0; i < list->size; ++i) {
    void *element = list->data[i];
    if (element && compare(target, element)) {
      return i;
    }
  }

  last_status = W_BLIB_NOT_FOUND;
  return list->size;
}

size_t alist_rfind(const ArrayList *list, void *target,
                   BlibComparator compare) {
  if (!alist_valid(list)) {
    return -1;
  }

  size_t i;
  for (i = 1; i <= list->size; ++i) {
    void *element = list->data[list->size - i];
    if (element && compare(target, element)) {
      return list->size - i;
    }
  }

  last_status = W_BLIB_NOT_FOUND;
  return list->size;
}

void alist_foreach(ArrayList *list, void (*fn)(void *)) {
  if (!alist_valid(list)) {
    return;
  } else if (!fn) {
    last_status = BLIB_INVALID_STRUCT;
    return;
  }

  size_t i;
  for (i = 0; i < list->size; ++i) {
    void *element = alist_get(list, i);
    if (element) fn(element);
  }
}

void alist_ssort(ArrayList *list, BlibComparator greater_equal) {
  if (!alist_valid(list)) {
    return;
  } else if (!greater_equal) {
    last_status = BLIB_INVALID_STRUCT;
    return;
  }

  size_t i;
  for (i = 0; i < list->size; ++i) {
    size_t j, min_index = i;
    for (j = i; j < list->size; ++j)
      if (greater_equal(list->data[min_index], list->data[j])) min_index = j;
    void *temp = list->data[min_index];
    list->data[min_index] = list->data[i];
    list->data[i] = temp;
  }
}

int alist_resize(ArrayList *list, size_t size, BlibDestroyer destroy) {
  if (!alist_valid(list)) {
    return -1;
  }

  if (size > list->size) {
    if (size > list->cap) {
      while (size > list->cap) list->cap <<= 1;
      void **new_data = realloc(list->data, sizeof(void *) * list->cap);
      if (new_data == NULL) {
        last_status = BLIB_ALLOC_FAIL;
        return -1;
      }
      list->data = new_data;
    }
    void **first_new = list->data + list->size;
    size_t size_new = size - list->size;
    memset(first_new, 0, sizeof(void *) * size_new);
  } else {
    size_t i;
    for (i = size; i < list->size; ++i)
      if (list->data[i] && destroy) DESTROY_DATA(destroy, list->data[i]);

    if (list->cap >> 1 > size) {
      while (list->cap >> 1 > size) list->cap >>= 1;
      void **new_data = realloc(list->data, sizeof(void *) * list->cap);
      if (new_data == NULL) {
        last_status = BLIB_ALLOC_FAIL;
        return -1;
      }
      list->data = new_data;
    }
  }
  list->size = size;
  return 0;
}

size_t alist_count(ArrayList *list) {
  if (!alist_valid(list)) return -1;

  size_t i, count = 0;
  for (i = 0; i < list->size; ++i)
    if (list->data[i]) ++count;
  return count;
}

size_t alist_size(const ArrayList *list) { return list->size; }
size_t alist_cap(const ArrayList *list) { return list->cap; }
int alist_empty(const ArrayList *list) { return list->size == 0; }
int alist_status(const ArrayList *list) {
  (void)alist_valid(list);
  /* TODO(Robert): more robust (threadsafe?) way of getting status */
  return last_status;
}
