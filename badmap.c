#include "badmap.h"

#include <stdlib.h>

#include "murmur3/murmur3.h"

/* internal functions */
static int bucket_node_init(Map *map, size_t index, void *key, void *value) {
  BucketNode *new_node = map->buckets[index].head;
  while (new_node) new_node = new_node->next;
  new_node = malloc(sizeof(BucketNode));
  new_node->key = key;
  new_node->value = value;
  ++(map->count);
}

static int bucket_node_destroy(Map *map, BucketNode *node, BucketNode *prev,
                               size_t index) {
  if (map->key_destroy) (map->key_destroy)(node->key);
  if (map->value_destroy) (map->value_destroy)(node->value);

  if (prev) {
    prev->next = node->next;
  } else {
    map->buckets[index].head = node->next;
  }

  free(node);
  --(map->count);
}

/* external functions */
int map_init(Map *map, size_t size, size_t key_size, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp) {
  map->buckets = malloc(sizeof(map->buckets) * size);
  map->size = size;
  map->key_size = key_size;
  map->key_destroy = key_dest;
  map->value_destroy = value_dest;
  map->key_compare = key_comp;
  return 0;
}

int map_destroy(Map *map) {
  for (size_t i = 0; i < map->size; ++i) {
    while (map->buckets[i].head) {
      bucket_node_destroy(map, map->buckets[i].head, NULL, i);
    }
  }
  free(map->buckets);
  return 0;
}

void *map_get(Map *map, void *key) {
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;
  BucketNode *current = map->buckets[hash].head;

  while (current && current->key != key) current = current->next;

  if (current)
    return current->value;
  else
    return NULL;
}

int map_insert(Map *map, void *key, void *value) {
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;
  bucket_node_init(map, hash, key, value);
}

int map_delete(Map *map, void *key) {
  int status = 0;
  size_t hash = 0;
  MurmurHash3_x86_32(key, map->key_size, 0, &hash);
  hash = hash % map->size;
  BucketNode *prev = map->buckets[hash].head;
  while (prev && prev->next && prev->next->key != key) {
    prev = prev->next;
  }

  if (prev) {
    BucketNode *target = prev->next;
    if (target) {
      if (target->key == key) {
        /* bucket is in the chain */
        status = bucket_node_destroy(map, target, prev, hash);
      } else {
        status = 1;
      }
    } else {
      if (prev->key == key) {
        /* bucket is at the start of the chain */
        status = bucket_node_destroy(map, prev, NULL, hash);
      } else {
        status = 1;
      }
    }
  } else {
    /* bucket not found */
    status = 1;
  }

  return status;
}

size_t map_size(Map *map) { return map->size; }
int map_empty(Map *map) { return map->count; }
