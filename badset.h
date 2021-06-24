#ifndef __BADMAP_H__
#define __BADMAP_H__
#include <stddef.h>

#include "badlib.h"

typedef struct set_bucket {
  void *element;
  size_t element_size;
  struct set_bucket *next;
} SetBucket;

typedef struct set {
  SetBucket *buckets;
  size_t capacity;
  size_t length;
  void *default_element;
  BlibDestroyer element_destroy;
  BlibComparator element_compare;
  BlibError last_status;
} Set;

int set_init(Set *set, size_t capacity, BlibDestroyer element_dest,
             BlibComparator element_comp);
int set_destroy(Set *set);

void *set_get(Set *set, void *element, size_t element_size);
int set_insert(Set *set, void *element, size_t element_size);
int set_delete(Set *set, void *element, size_t element_size);
int set_find(Set *set, void *element, size_t element_size, size_t *out);

void set_foreach(Set *set, void (*fn)(void *));
int set_empty(Set *set);
#endif
