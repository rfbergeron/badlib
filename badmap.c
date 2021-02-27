#include "badmap.h"

#include <stdio.h>
#include <stdlib.h>

#include "murmur3/murmur3.h"

int map_init(Map *map, size_t size, size_t key_size, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp) {
  if (!map) return 1;

  map->buckets = malloc(size * sizeof(Bucket));
  size_t i;
  for (i = 0; i < size; ++i) {
    /* As a consequence of using NULL as the key for the anchors, the user may
     * not use NULL as a key for any of their values. While this is less than
     * ideal, having NULL keys does seem a little silly to me.
     */
    map->buckets[i].key = NULL;
    map->buckets[i].value = NULL;
    map->buckets[i].next = (map->buckets + i);
  }

  map->size = size;
  map->count = 0;
  map->key_size = key_size;
  map->key_destroy = key_dest;
  map->value_destroy = value_dest;
  map->key_compare = key_comp;
  return 0;
}

int map_destroy(Map *map) {
  if (!map) return 1;

  size_t i;
  for (i = 0; i < map->size; ++i) {
    while (map->buckets[i].next != (map->buckets + i)) {
      printf("\nPointer of anchor: %p\n", map->buckets + i);
      printf("\nDeleting bucket at index %i, with pointer %p\n", i,
             map->buckets[i].next);
      map_delete(map, map->buckets[i].next->key);
    }
  }

  free(map->buckets);
  return 0;
}

void *map_get(Map *map, void *key) {
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;
  Bucket *current = map->buckets[hash].next;

  while (current != (map->buckets + hash) && current->key != key) {
    printf("\nKEY: %p not found; advancing to %p\n", key, current->next);
    current = current->next;
  }

  /* the anchor's value will be NULL or a user-defined default */
  return current->value;
}

int map_insert(Map *map, void *key, void *value) {
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;

  if (key == map->buckets[hash].key) {
    /* the anchor's value may not be modified */
    return 1;
  }

  Bucket *prev = map->buckets + hash;

  while (prev->next != (map->buckets + hash) && prev->next->key != key)
    prev = prev->next;

  if (prev->next->key == key) {
    /* key already present; replace value */
    if (map->value_destroy) (map->value_destroy)(prev->next->value);
    prev->next->value = value;
  } else {
    /* insert new bucket */
    Bucket *new_bucket = malloc(sizeof(Bucket));
    new_bucket->key = key;
    new_bucket->value = value;
    new_bucket->next = prev->next;
    prev->next = new_bucket;
  }

  return 0;
}

int map_delete(Map *map, void *key) {
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;

  Bucket *prev = map->buckets + hash;
  while (prev->next != (map->buckets + hash) && prev->next->key != key)
    prev = prev->next;

  if (prev->next == (map->buckets + hash)) {
    /* key not found or matches anchor; do not delete anchor */
    return 1;
  } else {
    Bucket *to_free = prev->next;
    prev->next = to_free->next;

    if (map->key_destroy) (map->key_destroy)(to_free->key);
    if (map->value_destroy) (map->value_destroy)(to_free->value);

    free(to_free);
  }
  return 0;
}

size_t map_size(Map *map) { return map->size; }
int map_empty(Map *map) { return map->count; }
