#include "badllist.h"
#include <stdlib.h>
#include <stddef.h>

/* internal functions */
Node *llist_head(LinkedList *list) {
    return list->anchor->next;
}

Node *llist_tail(LinkedList *list) {
    return list->anchor->prev;
}

int llist_valid(LinkedList *list) {
    return list != NULL && list->anchor != NULL;
}

/* initializer/destructor */
int llist_init(LinkedList *list) {
    list->anchor = malloc(sizeof(Node));
    if (list->anchor == NULL) return 1;
    list->anchor->next = list->anchor;
    list->anchor->prev = list->anchor;
    list->size = 0;
    return 0;
}

int llist_destroy(LinkedList *list) {
    if(!llist_valid(list)) return 1;
    while(!llist_empty(list)) llist_pop(list);
    free(list->anchor);
    return 0;
}

/* queue functions */
int llist_push(LinkedList *list, void *element) {
    if(!llist_valid(list)) return 1;
    Node *newnode = malloc(sizeof(*newnode));
    newnode->data = element;
    newnode->next = list->anchor->next;
    newnode->prev = list->anchor;
    list->anchor->next = newnode;
    ++(list->size);
    return 0;
}

void *llist_pop(LinkedList *list) {
    /* TODO(Robert): check if head is the anchor and output some kind of
     * meaningful error
     */
    if (!llist_valid(list) || llist_empty(list)) return NULL;
    Node *to_free = llist_head(list);
    void *ret = to_free->data;
    list->anchor->next = to_free->next;
    list->anchor->next->prev = list->anchor;
    free(to_free);
    --(list->size);
    return ret;
}

void *llist_peek(LinkedList *list) {
    if (!llist_valid(list)) return NULL;
    return llist_head(list)->data;
}

/* deque functions */
int llist_push_back(LinkedList *list, void *element) {
    if(!llist_valid(list)) return 1;
    Node *newnode = malloc(sizeof(*newnode));
    newnode->data = element;
    newnode->next = list->anchor;
    newnode->prev = list->anchor->prev;
    list->anchor->prev = newnode;
    ++(list->size);
    return 0;
}

void *llist_pop_back(LinkedList *list) {
    /* TODO(Robert): check if head is the anchor and output some kind of
     * meaningful error
     */
    if (!llist_valid(list) || llist_empty(list)) return NULL;
    Node *to_free = llist_tail(list);
    void *ret = to_free->data;
    list->anchor->prev = to_free->prev;
    list->anchor->prev->next = list->anchor;
    free(to_free);
    --(list->size);
    return ret;
}

void *llist_peek_back(LinkedList *list) {
    if (!llist_valid(list)) return NULL;
    return llist_tail(list)->data;
}

/* array functions */
void *llist_get(LinkedList *list, const size_t index) {
    size_t count = 0;
    Node *current = list->anchor->next;
    while (current != list->anchor && count < index) {
        current = current->next;
        ++count;
    }
    /* the anchor's data is just NULL so we can return it */
    current->data;
}

int llist_insert(LinkedList *list, void *element, size_t index) {
    size_t count = 0;
    Node *current = list->anchor->next;
    while (current != list->anchor && count < index) {
        current = current->next;
        ++count;
    }
    return 0;
}

int llist_delete(LinkedList *list, size_t index) {
    size_t count = 0;
    Node *current = list->anchor->next;
    while (current != list->anchor && count < index) {
        current = current->next;
        ++count;
    }
    if(current == list->anchor) return 1;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    free(current);
    return 0;
}

/* status functions */
size_t llist_size(LinkedList *list) {
    return list->size;
}

int llist_empty(LinkedList *list) {
    return list->size == 0;
}
