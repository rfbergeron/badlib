#include "badset.h"

#include <stdlib.h>

#include "murmur3/murmur3.h"

/* simple comparison function used as a placeholder when the user does not
 * provide one of their own.
 */
static int default_comp(void *e1, void *e2) { return e1 == e2; }

int set_init(Set *set, size_t capacity, BlibDestroyer element_dest,
             BlibComparator element_comp) {
  if (!set) return 1;

  set->buckets = malloc(capacity * sizeof(SetBucket));
  size_t i;
  for (i = 0; i < capacity; ++i) {
    /* As a consequence of using NULL as the element for the anchors, the user
     * may not use NULL as a element for any of their values. While this is less
     * than ideal, having NULL elements does seem a little silly to me.
     */
    set->buckets[i].element = NULL;
    set->buckets[i].next = (set->buckets + i);
  }

  set->capacity = capacity;
  set->length = 0;
  set->element_destroy = element_dest;
  set->element_compare = element_comp ? element_comp : default_comp;
  return 0;
}

int set_destroy(Set *set) {
  if (!set) return 1;
  if (!set->buckets) return 1;

  size_t i;
  for (i = 0; i < set->capacity; ++i) {
    while (set->buckets[i].next != (set->buckets + i)) {
      SetBucket *to_free = set->buckets[i].next;
      int status = set_delete(set, to_free->element, to_free->element_size);
      if (status) return status;
    }
  }

  free(set->buckets);
  return 0;
}

void *set_get(Set *set, void *element, size_t element_size) {
  if (!set || !(set->buckets) || !element) return NULL;

  size_t hash = 0;
  MurmurHash3_x86_32(element, element_size, 0, &hash);
  hash = hash % set->capacity;
  SetBucket *current = set->buckets[hash].next;

  while (current != (set->buckets + hash) &&
         !((set->element_compare)(element, current->element))) {
    current = current->next;
  }

  /* the anchor's value will be NULL or a user-defined default */
  return current->element;
}

int set_insert(Set *set, void *element, size_t element_size) {
  if (!set || !(set->buckets) || !element) return 1;

  size_t hash = 0;
  MurmurHash3_x86_32(element, element_size, 0, &hash);
  hash = hash % set->capacity;

  if ((set->element_compare)(set->buckets[hash].element, element)) {
    /* the anchor's value may not be modified */
    return 1;
  }

  SetBucket *prev = set->buckets + hash;

  while (prev->next != (set->buckets + hash) &&
         !(set->element_compare)(prev->next->element, element))
    prev = prev->next;

  /* make sure element is not already present; if it is, do nothing */
  if (!(set->element_compare)(prev->next->element, element)) {
    /* insert new bucket */
    SetBucket *new_bucket = malloc(sizeof(SetBucket));
    new_bucket->element = element;
    new_bucket->element_size = element_size;
    new_bucket->next = prev->next;
    prev->next = new_bucket;
  }

  return 0;
}

int set_delete(Set *set, void *element, size_t element_size) {
  if (!set || !(set->buckets) || !element) return 1;

  size_t hash = 0;
  MurmurHash3_x86_32(element, element_size, 0, &hash);
  hash = hash % set->capacity;

  SetBucket *prev = set->buckets + hash;
  while (prev->next != (set->buckets + hash) &&
         !(set->element_compare)(prev->next->element, element))
    prev = prev->next;

  if (prev->next == (set->buckets + hash)) {
    /* element not found or matches anchor; do not delete anchor */
    return 1;
  } else {
    SetBucket *to_free = prev->next;
    prev->next = to_free->next;

    if (set->element_destroy) (set->element_destroy)(to_free->element);

    free(to_free);
  }
  return 0;
}

int set_find(Set *set, void *element, size_t element_size, size_t *out) {
  if (!set || !(set->buckets) || !element || !out) return 0;

  size_t hash = 0;
  MurmurHash3_x86_32(element, element_size, 0, &hash);
  hash = hash % set->capacity;
  SetBucket *current = set->buckets[hash].next;

  size_t i = 0;
  while (current != (set->buckets + hash) &&
         !((set->element_compare)(element, current->element))) {
    current = current->next;
    ++i;
  }

  out[0] = hash;
  out[1] = i;
  return (set->element_compare)(element, current->element);
}

void set_foreach(Set *set, void (*fn)(void *)) {
  if (!set || !fn) return;
  size_t i;
  for (i = 0; i < set->capacity; ++i) {
    SetBucket *current = set->buckets[i].next;
    while (current != set->buckets + i) {
      (fn)(current->element);
      current = current->next;
    }
  }
}

int set_empty(Set *set) { return set->length; }
