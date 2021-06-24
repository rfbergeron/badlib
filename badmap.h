#ifndef __BADMAP_H__
#define __BADMAP_H__
#include <stddef.h>

#include "badlib.h"

typedef struct map_bucket {
  void *key;
  void *value;
  size_t key_size;
  struct map_bucket *next;
} MapBucket;

typedef struct map {
  MapBucket *buckets;
  size_t size;
  size_t count;
  void *default_key;
  void *default_value;
  BlibDestroyer key_destroy;
  BlibDestroyer value_destroy;
  BlibComparator key_compare;
  BlibError last_status;
} Map;

int map_init(Map *map, size_t size, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp);
int map_destroy(Map *map);

void *map_get(Map *map, void *key, size_t key_size);
int map_insert(Map *map, void *key, size_t key_size, void *value);
int map_delete(Map *map, void *key, size_t key_size);
int map_find(Map *map, void *key, size_t key_size, size_t *out);

void map_foreach_key(Map *map, void (*fn)(void *));
void map_foreach_value(Map *map, void (*fn)(void *));
void map_foreach_pair(Map *map, void (*fn)(void *, void *));
size_t map_size(Map *map);
int map_empty(Map *map);
#endif
