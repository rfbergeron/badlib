#include "badllist.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include "badlib.h"

static BlibError last_status = BLIB_SUCCESS;

/* internal functions */
static int llist_valid(const LinkedList *list) {
  if (!list) {
    return 0;
  } else if (!(list->anchor)) {
    last_status = BLIB_INVALID_STRUCT;
    return 0;
  }
  return 1;
}
static int node_init(LinkedList *list, Node *pred, Node *succ, void *element) {
  Node *new_node = malloc(sizeof(Node));
  new_node->data = element;
  new_node->next = succ;
  new_node->prev = pred;
  pred->next = new_node;
  succ->prev = new_node;
  ++list->size;
  return 0;
}

static int node_destroy(LinkedList *list, Node *node, void **wants_data) {
  node->next->prev = node->prev;
  node->prev->next = node->next;
  if (wants_data) {
    *wants_data = node->data;
  } else if (list->data_destroy) {
    (list->data_destroy)(node->data);
  }
  free(node);
  --list->size;
  return 0;
}

static Node *node_at(const LinkedList *list, size_t index) {
  size_t current_index = 0;
  Node *current_node = list->anchor->next;
  while (current_index < index) {
    current_node = current_node->next;
    ++current_index;
  }
  return current_node;
}

/* initializer/destructor */
int llist_init(LinkedList *list, BlibDestroyer dest, BlibComparator comp) {
  list->anchor = malloc(sizeof(Node));
  if (list->anchor == NULL) return 1;
  list->anchor->next = list->anchor;
  list->anchor->prev = list->anchor;
  list->anchor->data = NULL;
  list->data_destroy = dest;
  list->data_compare = comp;
  list->size = 0;
  return 0;
}

int llist_destroy(LinkedList *list) {
  if (list == NULL)
    return 1;
  else if (list->anchor == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return 1;
  }

  while (list->anchor->next != list->anchor) {
    node_destroy(list, list->anchor->next, NULL);
  }
  free(list->anchor);
  /* paranoid free */
  list->anchor = NULL;
  return 0;
}

int llist_clear(LinkedList *list) {
  if (list == NULL)
    return 1;
  else if (list->anchor == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return 1;
  }

  while (list->anchor->next != list->anchor) {
    node_destroy(list, list->anchor->next, NULL);
  }
  return 0;
}

/* NOTE: this is a shallow copy; freeing the list and its nodes is fine, but
 * not the elements, which is why the destination's destroyer is NULL
 */
int llist_copy(LinkedList *dest, const LinkedList *src) {
  if (!dest || !llist_valid(src)) return -1;
  int status = llist_init(dest, NULL, src->data_compare);
  if (status) return status;

  size_t i;
  for (i = 0; i < llist_size(src); ++i) {
    status = llist_push_back(dest, llist_get(src, i));
    if (status) return status;
  }
  return 0;
}

/* queue functions */
int llist_push_front(LinkedList *list, void *element) {
  if (!llist_valid(list)) return 1;
  return node_init(list, list->anchor, list->anchor->next, element);
}

void *llist_pop_front(LinkedList *list) {
  if (!llist_valid(list)) {
    return NULL;
  } else if (llist_empty(list)) {
    last_status = BLIB_EMPTY;
    return NULL;
  }
  void *ret = NULL;
  node_destroy(list, list->anchor->next, &ret);
  return ret;
}

void *llist_front(const LinkedList *list) {
  if (!llist_valid(list)) return NULL;
  return list->anchor->next->data;
}

/* deque functions */
int llist_push_back(LinkedList *list, void *element) {
  if (!llist_valid(list)) return 1;
  return node_init(list, list->anchor->prev, list->anchor, element);
}

void *llist_pop_back(LinkedList *list) {
  if (!llist_valid(list)) {
    return NULL;
  } else if (llist_empty(list)) {
    last_status = BLIB_EMPTY;
    return NULL;
  }
  void *ret = NULL;
  node_destroy(list, list->anchor->prev, &ret);
  return ret;
}

void *llist_back(const LinkedList *list) {
  if (!llist_valid(list)) return NULL;
  return list->anchor->prev->data;
}

int llist_rotate_forwards(LinkedList *list) {
  if (!llist_valid(list)) return 1;
  if (list->anchor->next == list->anchor->prev) return 0;
  Node *temp = list->anchor->next;
  temp->next = list->anchor;
  temp->prev = list->anchor->prev;
  list->anchor->next = list->anchor->next->next;
  list->anchor->next->prev = list->anchor;
  list->anchor->prev->next = temp;
  list->anchor->prev = temp;
  return 0;
}

int llist_rotate_backwards(LinkedList *list) {
  if (!llist_valid(list)) return 1;
  if (list->anchor->next == list->anchor->prev) return 0;
  Node *temp = list->anchor->prev;
  temp->next = list->anchor->next;
  temp->prev = list->anchor;
  list->anchor->prev = list->anchor->prev->prev;
  list->anchor->prev->next = list->anchor;
  list->anchor->next->prev = temp;
  list->anchor->next = temp;
  return 0;
}

/* array functions */
void *llist_get(const LinkedList *list, size_t index) {
  if (!llist_valid(list)) {
    return NULL;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  }
  Node *target = node_at(list, index);
  return target->data;
}

int llist_insert(LinkedList *list, void *element, size_t index) {
  if (!llist_valid(list)) {
    return 1;
  } else if (index >= (list->size + 1)) { /* allow inserting at the very end */
    last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  }
  Node *target = node_at(list, index);
  return node_init(list, target->prev, target, element);
}

int llist_delete(LinkedList *list, size_t index) {
  if (!llist_valid(list)) {
    return 1;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return 1;
  }
  Node *target = node_at(list, index);
  return node_destroy(list, target, NULL);
}

void *llist_extract(LinkedList *list, size_t index) {
  if (!llist_valid(list)) {
    return NULL;
  } else if (index >= list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  }
  Node *target = node_at(list, index);
  void *ret = NULL;
  node_destroy(list, target, &ret);
  return ret;
}

size_t llist_find(const LinkedList *list, void *target) {
  if (!llist_valid(list) || !target) return -1;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    void *element = llist_get(list, i);
    if (list->data_compare) {
      if ((list->data_compare)(target, element)) return i;
    } else if (target == element) {
      return i;
    }
  }
  return -1;
}

size_t llist_rfind(const LinkedList *list, void *target) {
  if (!llist_valid(list) || !target) return -1;
  size_t i, list_size = llist_size(list);
  for (i = 1; i <= list_size; ++i) {
    void *element = llist_get(list, list_size - i);
    if (list->data_compare) {
      if ((list->data_compare)(target, element)) return i;
    } else if (target == element) {
      return i;
    }
  }
  return -1;
}

void llist_foreach(LinkedList *list, void (*fn)(void *)) {
  if (!llist_valid(list) || !fn) return;
  size_t i;
  for (i = 0; i < list->size; ++i) {
    void *element = llist_get(list, i);
    (fn)(element);
  }
}

/* compare function assumed to be equivalent to <= */
int llist_sort(LinkedList *list, int (*compare)(void *, void *)) {
  size_t i;
  for (i = 0; i < list->size; ++i) {
    size_t max_index = i;
    void *max = llist_extract(list, max_index);
    size_t j;
    for (j = i; j < list->size; ++j) {
      if ((compare)(max, llist_get(list, j))) {
        max_index = j;
        /* do not remove from list; just hold onto it for comparisons */
        max = llist_get(list, max_index);
      }
    }
    /* maximum must be extracted */
    max = llist_extract(list, max_index);
    llist_push_front(list, max);
  }
  return 0;
}

/* status functions */
size_t llist_size(const LinkedList *list) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return 0;
  }
  return list->size;
}

int llist_empty(const LinkedList *list) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return 1;
  }
  return list->size == 0;
}

int llist_status(const LinkedList *list) {
  if (!llist_valid(list)) return BLIB_INVALID_STRUCT;
  /* TODO(Robert): more robust way of storing status */
  return last_status;
}

ListIter *llist_iter_begin(LinkedList *list) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = list;
  ret->node = list->anchor->next;
  return ret;
}

ListIter *llist_iter_end(LinkedList *list) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = list;
  ret->node = list->anchor;
  return ret;
}

ListIter *llist_iter_last(LinkedList *list) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = list;
  ret->node = list->anchor->prev;
  return ret;
}

ListIter *llist_iter_at(LinkedList *list, size_t index) {
  if (!llist_valid(list)) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  } else if (index > list->size) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = list;
  ret->node = node_at(list, index);
  return ret;
}

int liter_ins_before(ListIter *iter, void *data) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }
  return node_init(iter->list, iter->node->prev, iter->node, data);
}

int liter_ins_after(ListIter *iter, void *data) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }
  return node_init(iter->list, iter->node, iter->node->next, data);
}

int liter_delete(ListIter *iter) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }
  if (iter->node == iter->list->anchor) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return -1;
  }
  Node *to_delete = iter->node;
  if (to_delete->next == iter->list->anchor) {
    iter->node = to_delete->prev;
  } else {
    iter->node = to_delete->next;
  }
  return node_destroy(iter->list, to_delete, NULL);
}

int liter_push_back(ListIter *iter, ListIter **out, size_t count, ...) {
  va_list args;
  va_start(args, count);
  size_t i;
  ListIter *current = (out != NULL && *out == iter) ? iter : liter_copy(iter);
  if (current == NULL) return -1;
  for (i = 0; i < count; ++i) {
    void *element = va_arg(args, void *);
    int status =
        node_init(current->list, current->node, current->node->next, element);
    if (status) {
      free(current);
      return -1;
    }
    current->node = current->node->next;
  }
  if (out != NULL)
    *out = current;
  else
    free(current);
  return 0;
}

int liter_push_front(ListIter *iter, ListIter **out, size_t count, ...) {
  va_list args;
  va_start(args, count);
  size_t i;
  for (i = 0; i < count; ++i) {
    void *element = va_arg(args, void *);
    int status = node_init(iter->list, iter->node->prev, iter->node, element);
    if (status) return -1;
    if (i == 0 && out != NULL)
      *out = *out == iter ? (status = liter_advance(iter, -1), iter)
                          : liter_prev(iter, 1);
    if (status) return -1;
  }
  if (count == 0 && out != NULL) *out = liter_copy(iter);
  if (out != NULL && *out == NULL) return -1;
  return 0;
}

int liter_advance(ListIter *iter, ptrdiff_t count) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }
  ptrdiff_t i;
  if (count < 0) {
    for (i = 0; i > count; --i) {
      if (iter->node->prev == iter->list->anchor) {
        last_status = BLIB_OUT_OF_BOUNDS;
        return -1;
      } else {
        iter->node = iter->node->prev;
      }
    }
  } else {
    for (i = 0; i < count; ++i) {
      if (iter->node == iter->list->anchor) {
        last_status = BLIB_OUT_OF_BOUNDS;
        return -1;
      } else {
        iter->node = iter->node->next;
      }
    }
  }
  return 0;
}

ListIter *liter_next(ListIter *iter, size_t count) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  Node *next = iter->node;
  size_t i;
  for (i = 0; i < count; ++i) {
    if (next == iter->list->anchor) {
      last_status = BLIB_OUT_OF_BOUNDS;
      return NULL;
    } else {
      next = next->next;
    }
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = iter->list;
  ret->node = next;
  return ret;
}

ListIter *liter_prev(ListIter *iter, size_t count) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  Node *prev = iter->node;
  size_t i;
  for (i = 0; i < count; ++i) {
    if (prev->prev == iter->list->anchor) {
      last_status = BLIB_OUT_OF_BOUNDS;
      return NULL;
    } else {
      prev = prev->prev;
    }
  }
  ListIter *ret = malloc(sizeof(ListIter));
  ret->list = iter->list;
  ret->node = prev;
  return ret;
}

ListIter *liter_insert(ListIter *iter, size_t count, ...) {
  va_list args;
  va_start(args, count);
  size_t i;
  ListIter *ret = liter_copy(iter);
  if (ret == NULL) return ret;
  for (i = 0; i < count; ++i) {
    void *element = va_arg(args, void *);
    int status = node_init(iter->list, iter->node, iter->node->next, element);
    if (status) {
      free(ret);
      return NULL;
    }
    status = liter_advance(iter, 1);
    if (status) {
      free(ret);
      return NULL;
    }
  }
  return ret;
}

ListIter *liter_copy(ListIter *iter) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  }
  ListIter *ret = malloc(sizeof(ListIter));
  *ret = *iter;
  return ret;
}

void *liter_get(ListIter *iter) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return NULL;
  } else if (iter->node == iter->list->anchor) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return NULL;
  }
  return iter->node->data;
}

int liter_set(ListIter *iter, void *data) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  } else if (iter->node == iter->list->anchor) {
    last_status = BLIB_OUT_OF_BOUNDS;
    return -1;
  }
  if (iter->list->data_destroy) iter->list->data_destroy(iter->node->data);
  iter->node->data = data;
  return 0;
}

int liter_end(ListIter *iter) {
  if (iter == NULL) {
    last_status = BLIB_INVALID_STRUCT;
    return -1;
  }
  return iter->node == iter->list->anchor;
}

int liter_status(ListIter *iter) {
  if (iter == NULL) {
    return BLIB_INVALID_STRUCT;
  }
  return llist_status(iter->list);
}
