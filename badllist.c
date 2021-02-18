#include "badllist.h"

#include <stddef.h>
#include <stdlib.h>
#include "badlib.h"

/* internal functions */
static int llist_valid(LinkedList *list) {
  return list != NULL && list->anchor != NULL;
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
    int status = 0;
    node->next->prev = node->prev;
    node->prev->next = node->next;
    if(wants_data) {
        *wants_data = node->data;
    } else if(list->data_destroy) {
        (list->data_destroy)(node->data);
    }
    free(node);
    --list->size;
    return status;
}

static Node *node_at(LinkedList *list, size_t index) {
    size_t current_index = 0;
    Node *current_node = list->anchor->next;
    while (current_index < index) {
        current_node = current_node->next;
        ++current_index;
    }
    return current_node;
}

/* initializer/destructor */
int llist_init(LinkedList *list, Blib_Destroyer dest, Blib_Comparator comp) {
  list->anchor = malloc(sizeof(Node));
  if (list->anchor == NULL) return 1;
  list->anchor->next = list->anchor;
  list->anchor->prev = list->anchor;
  list->data_destroy = dest;
  list->data_compare = comp;
  list->size = 0;
  return 0;
}

int llist_destroy(LinkedList *list) {
  if (!llist_valid(list)) return 1;
  while (!llist_empty(list)) {
    void *to_free = llist_pop_front(list);
    int status = 0;
    if(list->data_destroy != NULL) (*list->data_destroy)(to_free);
  }
  free(list->anchor);
  return 0;
}

/* queue functions */
int llist_push_front(LinkedList *list, void *element) {
  if (!llist_valid(list)) return 1;
  return node_init(list, list->anchor, list->anchor->next, element);
}

void *llist_pop_front(LinkedList *list) {
  if (!llist_valid(list) || llist_empty(list)) return NULL;
  void *ret = NULL;
  int status = node_destroy(list, list->anchor->next, &ret);
  return ret;
}

void *llist_front(LinkedList *list) {
  if (!llist_valid(list)) return NULL;
  return list->anchor->next->data;
}

/* deque functions */
int llist_push_back(LinkedList *list, void *element) {
  if (!llist_valid(list)) return 1;
  return node_init(list, list->anchor->prev, list->anchor, element);
}

void *llist_pop_back(LinkedList *list) {
  if (!llist_valid(list) || llist_empty(list)) return NULL;
  void *ret = NULL;
  int status = node_destroy(list, list->anchor->prev, &ret);
  return ret;
}

void *llist_back(LinkedList *list) {
  if (!llist_valid(list)) return NULL;
  return list->anchor->prev->data;
}

/* array functions */
void *llist_get(LinkedList *list, size_t index) {
    if (!llist_valid(list) || index >= list->size) return NULL;
    Node *target = node_at(list, index);
    return target->data;
}

int llist_insert(LinkedList *list, void *element, size_t index) {
  int status = 0;
  if(!llist_valid(list) || index >= list->size) status = 1;
  if(!status) {
    Node *target = node_at(list, index);
    node_init(list, target->prev, target, element);
  }
  return status;
}

int llist_delete(LinkedList *list, size_t index) {
  int status = 0;
  if(!llist_valid(list) || llist_empty(list)) status = 1;
  if(!status) {
    Node *target = node_at(list, index);
    node_destroy(list, target, NULL);
  }
  return status;
}

/* status functions */
size_t llist_size(LinkedList *list) { return list->size; }

int llist_empty(LinkedList *list) { return list->size == 0; }
