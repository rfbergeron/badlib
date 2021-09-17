#ifndef __BADMAP_H__
#define __BADMAP_H__
#include <stddef.h>

#include "badllist.h"
#include "badlib.h"

#define BLIB_MAP_EMPTY { NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, BLIB_SUCCESS }

typedef struct map_bucket {
  void *key;
  void *value;
  size_t key_size;
  struct map_bucket *next;
} MapBucket;

typedef struct map {
  MapBucket *buckets;
  size_t entry_count;
  size_t bucket_count;
  BlibDestroyer key_destroy;
  BlibDestroyer value_destroy;
  BlibComparator key_compare;
} Map;

int map_init(Map *map, size_t bucket_count, BlibDestroyer key_dest,
             BlibDestroyer value_dest, BlibComparator key_comp);
int map_destroy(Map *map);

void *map_get(const Map *map, void *key, const size_t key_size);
int map_insert(Map *map, void *key, size_t key_size, void *value);
int map_delete(Map *map, void *key, size_t key_size);
int map_find(const Map *map, void *key, size_t key_size, size_t *out);
int map_keys(const Map *map, LinkedList *out);
int map_values(const Map *map, LinkedList *out);

void map_foreach_key(Map *map, void (*fn)(void *));
void map_foreach_value(Map *map, void (*fn)(void *));
void map_foreach_pair(Map *map, void (*fn)(void *, void *));
size_t map_size(const Map *map);
int map_empty(const Map *map);
int map_status(const Map *map);
#endif
