#include "badllist.h"

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

/* NOTE: this is a shallow copy; freeing the list and its nodes is fine, but
 * not the elements, which is why the destination's destroyer is NULL
 */
int llist_copy(LinkedList *dest, const LinkedList *src) {
  if (!dest || !llist_valid(src)) return -1;
  int status = llist_init(dest, NULL, src->data_compare);
  if (status) return status;

  size_t i;
  for (i = 0; i < llist_size(src); ++i) {
    int status = llist_push_back(dest, llist_get(src, i));
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
    if ((list->data_compare)(target, element)) return i;
  }
  return -1;
}

size_t llist_rfind(const LinkedList *list, void *target) {
  if (!llist_valid(list) || !target) return -1;
  size_t i;
  for (i = llist_size(list); i >= 0; --i) {
    void *element = llist_get(list, i);
    if ((list->data_compare)(target, element)) return i;
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

/* status functions */
size_t llist_size(const LinkedList *list) { return list->size; }
int llist_empty(const LinkedList *list) { return list->size == 0; }
int llist_status(const LinkedList *list) {
  /* TODO(Robert): more robust way of storing status */
  return last_status;
}

