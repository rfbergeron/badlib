#include "badmap.h"

#include <stdlib.h>

#include "murmur3/murmur3.h"

static BlibError last_status = BLIB_SUCCESS;

/* simple comparison function used as a placeholder when the user does not
 * provide one of their own.
 */
int default_comp(void *k1, void *k2) { return k1 == k2; }

int map_init(Map *map, size_t bucket_count, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp) {
  if (!map || bucket_count < 1) return 1;

  map->buckets = malloc(bucket_count * sizeof(MapBucket));
  size_t i;
  for (i = 0; i < bucket_count; ++i) {
    /* As a consequence of using NULL as the key for the anchors, the user may
     * not use NULL as a key for any of their values. While this is less than
     * ideal, having NULL keys does seem a little silly to me.
     */
    map->buckets[i].key = NULL;
    map->buckets[i].value = NULL;
    map->buckets[i].next = (map->buckets + i);
  }

  map->entry_count = 0;
  map->bucket_count = bucket_count;
  map->key_destroy = key_dest;
  map->value_destroy = value_dest;
  map->key_compare = key_comp ? key_comp : default_comp;
  return 0;
}

int map_destroy(Map *map) {
  if (!map || !map->buckets) return 1;

  size_t i;
  for (i = 0; i < map->bucket_count; ++i) {
    while (map->buckets[i].next != (map->buckets + i)) {
      MapBucket *to_free = map->buckets[i].next;
      int status = map_delete(map, to_free->key, to_free->key_size);
      if (status) return status;
    }
  }

  free(map->buckets);
  /* paranoid free */
  map->buckets = NULL;
  return 0;
}

void *map_get(const Map *map, void *key, size_t key_size) {
  if (!map || !map->buckets || !key) return NULL;

  size_t hash = 0;
  MurmurHash3_x86_32(key, key_size, 0, &hash);
  hash = hash % map->bucket_count;
  MapBucket *current = map->buckets[hash].next;

  while (current != (map->buckets + hash) &&
         !((map->key_compare)(key, current->key))) {
    current = current->next;
  }

  /* the anchor's value will be NULL or a user-defined default */
  return current->value;
}

int map_insert(Map *map, void *key, size_t key_size, void *value) {
  if (!map || !map->buckets || !key) return 1;

  size_t hash = 0;
  MurmurHash3_x86_32(key, key_size, 0, &hash);
  hash = hash % map->bucket_count;

  if ((map->key_compare)(map->buckets[hash].key, key)) {
    /* the anchor's value may not be modified */
    return 1;
  }

  MapBucket *prev = map->buckets + hash;

  while (prev->next != (map->buckets + hash) &&
         !(map->key_compare)(prev->next->key, key))
    prev = prev->next;

  if ((map->key_compare)(prev->next->key, key)) {
    /* key already present; replace value */
    if (map->value_destroy) (map->value_destroy)(prev->next->value);
    prev->next->value = value;
  } else {
    /* insert new bucket */
    MapBucket *new_bucket = malloc(sizeof(MapBucket));
    new_bucket->key = key;
    new_bucket->value = value;
    new_bucket->key_size = key_size;
    new_bucket->next = prev->next;
    prev->next = new_bucket;
    ++(map->entry_count);
  }

  return 0;
}

int map_delete(Map *map, void *key, size_t key_size) {
  if (!map || !map->buckets || !key) return 1;

  size_t hash = 0;
  MurmurHash3_x86_32(key, key_size, 0, &hash);
  hash = hash % map->bucket_count;

  MapBucket *prev = map->buckets + hash;
  while (prev->next != (map->buckets + hash) &&
         !(map->key_compare)(prev->next->key, key))
    prev = prev->next;

  if (prev->next == (map->buckets + hash)) {
    /* key not found or matches anchor; do not delete anchor */
    return 1;
  } else {
    MapBucket *to_free = prev->next;
    prev->next = to_free->next;

    if (map->key_destroy) (map->key_destroy)(to_free->key);
    if (map->value_destroy) (map->value_destroy)(to_free->value);

    free(to_free);
    --(map->entry_count);
  }
  return 0;
}

int map_find(const Map *map, void *key, size_t key_size, size_t *out) {
  if (!map || !map->buckets || !key || !out) return 0;

  size_t hash = 0;
  MurmurHash3_x86_32(key, key_size, 0, &hash);
  hash = hash % map->bucket_count;
  MapBucket *current = map->buckets[hash].next;

  size_t i = 0;
  while (current != (map->buckets + hash) &&
         !((map->key_compare)(key, current->key))) {
    current = current->next;
    ++i;
  }

  out[0] = hash;
  out[1] = i;
  return (map->key_compare)(key, current->key);
}

int map_keys(const Map *map, LinkedList *out) {
  if (!map || !map->buckets) return 1;
  size_t i;
  for (i = 0; i < map->bucket_count; ++i) {
    MapBucket *current = map->buckets[i].next;
    while (current != (map->buckets + i)) {
      int status = llist_push_back(out, current->key);
      if (status) return status;
      current = current->next;
    }
  }
  return 0;
}

int map_values(const Map *map, LinkedList *out) {
  if (!map || !map->buckets) return 1;
  size_t i;
  for (i = 0; i < map_size(map); ++i) {
    MapBucket *current = map->buckets[i].next;
    while (current != (map->buckets + i)) {
      int status = llist_push_back(out, current->value);
      if (status) return status;
      current = current->next;
    }
  }
  return 0;
}

int map_foreach_key(Map *map, void (*fn)(void *)) {
  if (!map || !map->buckets || !fn) return 1;
  size_t i;
  for (i = 0; i < map->bucket_count; ++i) {
    MapBucket *current = map->buckets[i].next;
    while (current != map->buckets + i) {
      (fn)(current->key);
      current = current->next;
    }
  }
  return 0;
}

int map_foreach_value(Map *map, void (*fn)(void *)) {
  if (!map || !map->buckets || !fn) return 1;
  size_t i;
  for (i = 0; i < map->bucket_count; ++i) {
    MapBucket *current = map->buckets[i].next;
    while (current != map->buckets + i) {
      (fn)(current->value);
      current = current->next;
    }
  }
  return 0;
}

int map_foreach_pair(Map *map, void (*fn)(void *, void *)) {
  if (!map || !map->buckets || !fn) return 1;
  size_t i;
  for (i = 0; i < map->bucket_count; ++i) {
    MapBucket *current = map->buckets[i].next;
    while (current != map->buckets + i) {
      (fn)(current->key, current->value);
      current = current->next;
    }
  }
  return 0;
}

size_t map_size(const Map *map) { return map->entry_count; }
int map_empty(const Map *map) { return map->entry_count == 0; }
int map_status(const Map *map) {
  /* TODO(Robert): handle this better */
  return last_status;
}
