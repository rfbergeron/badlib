#ifndef __BADMAP_H__
#define __BADMAP_H__
#include <stddef.h>

#include "badlib.h"

typedef struct bucket {
  void *key;
  void *value;
  struct bucket *next;
} Bucket;

typedef struct map {
  Bucket *buckets;
  size_t size;
  size_t count;
  size_t key_size;
  void *default_key;
  void *default_value;
  BlibDestroyer key_destroy;
  BlibDestroyer value_destroy;
  BlibComparator key_compare;
  BlibError last_status;
} Map;

int map_init(Map *map, size_t size, size_t key_size, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp);
int map_destroy(Map *map);

void *map_get(Map *map, void *key);
int map_insert(Map *map, void *key, void *value);
int map_delete(Map *map, void *key);

size_t map_size(Map *map);
int map_empty(Map *map);
#endif
