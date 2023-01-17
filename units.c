#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "badalist.h"
#include "badllist.h"
#include "badmap.h"

typedef struct complicated {
  int *bingus;
  float *dingus;
} Complicated;

Complicated *init_complicated(void) {
  Complicated *ret = malloc(sizeof(Complicated));
  ret->bingus = malloc(1 * sizeof(int));
  ret->dingus = malloc(4 * sizeof(float));
  return ret;
}

void free_complicated(struct complicated *somedata) {
  free(somedata->bingus);
  free(somedata->dingus);
  free(somedata);
}

ArrayList *arraylist = NULL;
LinkedList *linkedlist = NULL;
Map *map = NULL;
int test_data[10] = {0, 2, 3, 1, 6, 5, 4, 9, 7, 8};
Complicated *comp1;
Complicated *comp2;
Complicated *comp3;

int init_llist_suite(void) {
  linkedlist = malloc(sizeof(*linkedlist));
  if (linkedlist == NULL || llist_init(linkedlist, free, NULL)) return 1;
  return 0;
}

int clean_llist_suite(void) {
  if (llist_destroy(linkedlist)) return 1;
  free(linkedlist);
  linkedlist = NULL;
  return 0;
}

int init_liter_suite(void) {
  linkedlist = malloc(sizeof(*linkedlist));
  if (linkedlist == NULL ||
      llist_init(linkedlist, (void (*)(void *))free_complicated, NULL))
    return 1;
  comp1 = init_complicated();
  comp2 = init_complicated();
  comp3 = init_complicated();
  llist_push_front(linkedlist, comp1);
  llist_push_front(linkedlist, comp2);
  llist_push_front(linkedlist, comp3);
  return 0;
}

int clean_liter_suite(void) {
  if (llist_destroy(linkedlist)) return 1;
  free(linkedlist);
  linkedlist = NULL;
  return 0;
}

void test_llist_defaults(void) {
  CU_ASSERT_PTR_EQUAL(linkedlist->anchor->next, linkedlist->anchor->prev);
  CU_ASSERT_PTR_EQUAL(linkedlist->anchor->next, linkedlist->anchor);
}

void test_llist_queue(void) {
  int *f = malloc(sizeof(int));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, f));
  CU_ASSERT_PTR_EQUAL(f, linkedlist->anchor->next->data);
  CU_ASSERT_PTR_EQUAL(f, llist_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(f, llist_pop_front(linkedlist));
  CU_ASSERT_EQUAL(0, linkedlist->size);
  CU_ASSERT_FALSE(llist_push_back(linkedlist, f));
  CU_ASSERT_PTR_EQUAL(f, linkedlist->anchor->prev->data);
  CU_ASSERT_PTR_EQUAL(f, llist_back(linkedlist));
  CU_ASSERT_PTR_EQUAL(f, llist_pop_back(linkedlist));
  CU_ASSERT_EQUAL(0, linkedlist->size);
  free(f);
}

void test_llist_deque(void) {
  int *f = malloc(sizeof(int));
  int *g = malloc(sizeof(int));
  int *h = malloc(sizeof(int));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, f));
  CU_ASSERT_FALSE(llist_push_back(linkedlist, g));
  CU_ASSERT_PTR_EQUAL(f, llist_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(g, llist_back(linkedlist));
  CU_ASSERT_PTR_EQUAL(f, llist_pop_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(llist_front(linkedlist), llist_back(linkedlist));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, h));
  CU_ASSERT_FALSE(llist_push_back(linkedlist, f));
  CU_ASSERT_PTR_EQUAL(h, llist_get(linkedlist, 0));
  CU_ASSERT_PTR_EQUAL(g, llist_get(linkedlist, 1));
  CU_ASSERT_PTR_EQUAL(f, llist_get(linkedlist, 2));
  CU_ASSERT_FALSE(llist_clear(linkedlist));
  CU_ASSERT_TRUE(llist_empty(linkedlist));
}

void test_llist_errors(void) {
  CU_ASSERT_TRUE(llist_push_front(NULL, NULL));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, NULL));
  CU_ASSERT_PTR_NULL(llist_pop_front(NULL));
}

void test_llist_sequence(void) {
  struct vec {
    float x;
    float y;
    float z;
  };
  float f = 4.2f;
  float *i = malloc(sizeof(int));
  *i = 255;
  char strong[] = "yeehaw";
  struct vec v = {2.5f, 3.14f, 1.0f};

  CU_ASSERT_FALSE(llist_push_front(linkedlist, &f));
  CU_ASSERT_PTR_EQUAL(&f, llist_pop_front(linkedlist));
  CU_ASSERT_PTR_NULL(llist_pop_front(linkedlist));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, i));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, &v));
  CU_ASSERT_FALSE(llist_push_front(linkedlist, strong));
  CU_ASSERT_PTR_EQUAL(strong, llist_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(strong, llist_pop_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(&v, llist_pop_front(linkedlist));
  CU_ASSERT_PTR_EQUAL(i, llist_pop_front(linkedlist));
  CU_ASSERT_PTR_NULL(llist_pop_front(linkedlist));
  CU_ASSERT_TRUE(llist_empty(linkedlist));

  free(i);
}

void test_liter_creation(void) {
  ListIter *begin_iter = llist_iter_begin(linkedlist);
  CU_ASSERT_PTR_NOT_NULL(begin_iter);
  CU_ASSERT_PTR_EQUAL(begin_iter->node, linkedlist->anchor->next);

  ListIter *end_iter = llist_iter_end(linkedlist);
  CU_ASSERT_PTR_NOT_NULL(end_iter);
  CU_ASSERT_PTR_EQUAL(end_iter->node, linkedlist->anchor);

  ListIter *last_iter = llist_iter_at(linkedlist, 2);
  CU_ASSERT_PTR_NOT_NULL(last_iter);
  CU_ASSERT_PTR_EQUAL(last_iter->node, linkedlist->anchor->prev);

  ListIter *other_end_iter = liter_next(last_iter, 1);
  CU_ASSERT_PTR_NOT_NULL(other_end_iter);
  CU_ASSERT_PTR_EQUAL(other_end_iter->node, end_iter->node);

  ListIter *other_begin_iter = liter_prev(end_iter, 3);
  CU_ASSERT_PTR_NOT_NULL(other_begin_iter);
  CU_ASSERT_PTR_EQUAL(other_begin_iter->node, begin_iter->node);
  free(begin_iter);
  free(end_iter);
  free(last_iter);
  free(other_end_iter);
  free(other_begin_iter);
}

void test_liter_mutation(void) {
  ListIter *iter = llist_iter_begin(linkedlist);
  CU_ASSERT_FALSE(liter_advance(iter, 3));
  ListIter *end_iter = llist_iter_end(linkedlist);
  CU_ASSERT_PTR_EQUAL(iter->node, end_iter->node);
  CU_ASSERT_TRUE(liter_advance(iter, 1));
  CU_ASSERT_EQUAL(llist_status(linkedlist), BLIB_OUT_OF_BOUNDS);
  free(iter);
  free(end_iter);

  iter = llist_iter_end(linkedlist);
  CU_ASSERT_PTR_NOT_NULL(iter);
  CU_ASSERT_FALSE(liter_advance(iter, -1));
  ListIter *last_iter = llist_iter_at(linkedlist, 2);
  CU_ASSERT_PTR_EQUAL(iter->node, last_iter->node);
  free(iter);
  free(last_iter);
}

void test_liter_iteration(void) {
  ListIter *iter = llist_iter_begin(linkedlist);
  ;
  size_t count = 0;
  while (!liter_end(iter)) {
    CU_ASSERT_PTR_EQUAL(liter_get(iter), llist_get(linkedlist, count));
    ++count;
    CU_ASSERT_FALSE(liter_advance(iter, 1));
  }
  CU_ASSERT_PTR_EQUAL(iter->node, linkedlist->anchor);
  free(iter);
}

int init_alist_suite(void) {
  if (NULL == (arraylist = malloc(sizeof(*arraylist))) ||
      alist_init(arraylist, 10))
    return 1;
  return 0;
}

int clean_alist_suite(void) {
  if (alist_destroy(arraylist, free)) return 1;
  free(arraylist);
  arraylist = NULL;
  return 0;
}

void test_alist_defaults(void) {
  CU_ASSERT_PTR_NOT_NULL_FATAL(arraylist);
  CU_ASSERT_PTR_NOT_NULL_FATAL(arraylist->data);
  CU_ASSERT_EQUAL(10, alist_size(arraylist));
  CU_ASSERT_EQUAL(16, alist_cap(arraylist));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));
}

void test_alist_arr(void) {
  /* insert and delete */
  int *foo = malloc(sizeof(int));
  *foo = 216;
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, foo, 5, NULL));
  CU_ASSERT_PTR_EQUAL(alist_get(arraylist, 5), foo);
  CU_ASSERT_EQUAL(1, alist_count(arraylist));
  CU_ASSERT_EQUAL(0, alist_delete(arraylist, 5, free));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));

  /* insert, the insert again, replacing previous element */
  int *bar = malloc(sizeof(int));
  *bar = 12;
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, bar, 5, NULL));
  size_t i;
  for (i = 0; i < 10; ++i)
    CU_ASSERT_EQUAL(0, alist_insert(arraylist, test_data + i, i, free));
  CU_ASSERT_EQUAL(10, alist_count(arraylist));
  CU_ASSERT_EQUAL(10, alist_size(arraylist));
  for (i = 0; i < 10; ++i)
    CU_ASSERT_EQUAL(test_data[i], *(int *)alist_get(arraylist, i));
  for (i = 0; i < 10; ++i) CU_ASSERT_EQUAL(0, alist_delete(arraylist, i, NULL));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));

  /* get and delete out of bounds */
  CU_ASSERT_PTR_NULL(alist_get(arraylist, 10))
  CU_ASSERT_EQUAL(BLIB_OUT_OF_BOUNDS, alist_status(arraylist));
  CU_ASSERT_NOT_EQUAL(0, alist_delete(arraylist, 10, free));
  CU_ASSERT_EQUAL(BLIB_OUT_OF_BOUNDS, alist_status(arraylist));
}

void test_alist_resize(void) {
  /* insert past the end */
  CU_ASSERT_NOT_EQUAL(0, alist_insert(arraylist, test_data, 11, free));
  CU_ASSERT_EQUAL(10, alist_size(arraylist));
  CU_ASSERT_PTR_EQUAL(NULL, alist_get(arraylist, 11));

  /* insert at the end */
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, test_data, 10, free));
  CU_ASSERT_EQUAL(11, alist_size(arraylist));
  CU_ASSERT_EQUAL(16, alist_cap(arraylist));
  CU_ASSERT_PTR_EQUAL(test_data, alist_get(arraylist, 10));
  CU_ASSERT_EQUAL(0, alist_delete(arraylist, 10, NULL));

  /* fill, then resize to zero */
  size_t i;
  for (i = 0; i < 10; ++i) {
    int *temp = malloc(sizeof(int));
    *temp = test_data[i];
    CU_ASSERT_EQUAL(0, alist_insert(arraylist, temp, i, NULL));
  }
  CU_ASSERT_EQUAL(10, alist_count(arraylist));
  CU_ASSERT_EQUAL(0, alist_resize(arraylist, 0, free));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));
  CU_ASSERT_EQUAL(0, alist_size(arraylist));
  CU_ASSERT_EQUAL(1, alist_cap(arraylist));
  CU_ASSERT_TRUE(alist_empty(arraylist));

  /* insert at end of zero-length array */
  int *foo[3];
  foo[0] = malloc(sizeof(int)), foo[1] = malloc(sizeof(int)),
  foo[2] = malloc(sizeof(int));
  *foo[0] = 6, *foo[1] = 17, *foo[2] = 18;
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, foo[0], 0, free));
  CU_ASSERT_EQUAL(1, alist_size(arraylist));
  CU_ASSERT_EQUAL(1, alist_cap(arraylist));
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, foo[1], 1, free));
  CU_ASSERT_EQUAL(2, alist_size(arraylist));
  CU_ASSERT_EQUAL(2, alist_cap(arraylist));
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, foo[2], 2, free));
  CU_ASSERT_EQUAL(3, alist_size(arraylist));
  CU_ASSERT_EQUAL(4, alist_cap(arraylist));
  size_t j;
  for (j = 0; j < 3; ++j) CU_ASSERT_PTR_EQUAL(foo[j], alist_get(arraylist, j));

  /* resize to 20 */
  CU_ASSERT_EQUAL(0, alist_resize(arraylist, 20, free));
  CU_ASSERT_EQUAL(20, alist_size(arraylist));
  CU_ASSERT_EQUAL(32, alist_cap(arraylist));

  /* insert in and at end of resized array */
  int *bar = malloc(sizeof(int)), *baz = malloc(sizeof(int));
  *bar = 2, *baz = 3;
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, bar, 18, free));
  CU_ASSERT_PTR_EQUAL(bar, alist_get(arraylist, 18));
  CU_ASSERT_EQUAL(4, alist_count(arraylist));
  CU_ASSERT_EQUAL(0, alist_insert(arraylist, baz, 20, free));
  CU_ASSERT_PTR_EQUAL(baz, alist_get(arraylist, 20));
  CU_ASSERT_EQUAL(21, alist_size(arraylist));

  /* resize back to 10 and clear */
  CU_ASSERT_EQUAL(0, alist_resize(arraylist, 10, free));
  CU_ASSERT_EQUAL(3, alist_count(arraylist));
  CU_ASSERT_EQUAL(10, alist_size(arraylist));
  CU_ASSERT_EQUAL(16, alist_cap(arraylist));
  CU_ASSERT_EQUAL(0, alist_clear(arraylist, free));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));
}

void test_alist_stack(void) {
  int *foo[3];
  foo[0] = malloc(sizeof(int)), foo[1] = malloc(sizeof(int)),
  foo[2] = malloc(sizeof(int));
  *foo[0] = 6, *foo[1] = 12, *foo[2] = 16;

  /* push to array with existing size */
  size_t i;
  for (i = 0; i < 3; ++i) CU_ASSERT_EQUAL(0, alist_push(arraylist, foo[i]));
  CU_ASSERT_EQUAL(13, alist_size(arraylist));
  for (i = 0; i < 10; ++i) CU_ASSERT_PTR_EQUAL(NULL, alist_get(arraylist, i));
  for (i = 0; i < 3; ++i) CU_ASSERT_EQUAL(foo[i], alist_get(arraylist, i + 10));

  /* pop from array with existing size */
  for (i = 0; i < 3; ++i) {
    CU_ASSERT_EQUAL(foo[3 - i - 1], alist_peek(arraylist));
    CU_ASSERT_EQUAL(0, alist_pop(arraylist, free));
  }
  CU_ASSERT_EQUAL(10, alist_size(arraylist));

  /* pop until empty */
  while (!alist_empty(arraylist))
    CU_ASSERT_EQUAL(0, alist_pop(arraylist, free));
  CU_ASSERT_EQUAL(0, alist_size(arraylist));
  CU_ASSERT_EQUAL(16, alist_cap(arraylist));

  /* resize to 0 to decrease capacity */
  CU_ASSERT_EQUAL(0, alist_resize(arraylist, 0, free));
  CU_ASSERT_EQUAL(0, alist_size(arraylist));
  CU_ASSERT_EQUAL(1, alist_cap(arraylist));

  /* push to empty list, then clear */
  for (i = 0; i < 10; ++i)
    CU_ASSERT_EQUAL(0, alist_push(arraylist, test_data + i));
  for (i = 0; i < 10; ++i)
    CU_ASSERT_EQUAL(test_data + i, alist_get(arraylist, i));
  CU_ASSERT_EQUAL(10, alist_count(arraylist));
  CU_ASSERT_EQUAL(10, alist_size(arraylist));
  CU_ASSERT_EQUAL(16, alist_cap(arraylist));
  CU_ASSERT_EQUAL(0, alist_clear(arraylist, NULL));
  CU_ASSERT_EQUAL(0, alist_count(arraylist));
}

int init_map_suite(void) {
  /* use integers for keys */
  map = malloc(sizeof(Map));
  return map_init(map, 20, NULL, NULL, NULL);
}

int clean_map_suite(void) { return map_destroy(map); }

void test_map_basic(void) {
  int k = 5;
  char *v = "mantequilla";
  CU_ASSERT(0 == map_insert(map, &k, sizeof(k), v));
  CU_ASSERT_PTR_EQUAL(v, map_get(map, &k, sizeof(k)));
  CU_ASSERT(0 == map_delete(map, &k, sizeof(k)));
  CU_ASSERT_PTR_NULL(map_get(map, &k, sizeof(k)));

  /* test that freeing of values works */
  map->value_destroy = free;
  float *fs = malloc(4 * sizeof(float));
  fs[0] = 5.5f;
  fs[3] = 1.6f;
  char *k2 = "caribou";

  CU_ASSERT(0 == map_insert(map, k2, strlen(k2), fs));
  CU_ASSERT_PTR_EQUAL(fs, map_get(map, k2, strlen(k2)));
  CU_ASSERT(0 == map_delete(map, k2, strlen(k2)));

  float *fs2 = malloc(sizeof(float));
  short *fs3 = malloc(sizeof(short));
  *fs2 = 6.9f;
  *fs3 = 42;
  char *k3 = "ratioed";

  CU_ASSERT(0 == map_insert(map, k3, strlen(k3), fs2));
  CU_ASSERT_PTR_EQUAL(fs2, map_get(map, k3, strlen(k3)));
  CU_ASSERT(0 == map_insert(map, k3, strlen(k3), fs3));
  CU_ASSERT_PTR_EQUAL(fs3, map_get(map, k3, strlen(k3)));
  CU_ASSERT(0 == map_delete(map, k3, strlen(k3)));
}

void test_map_buckets(void) {
  size_t numpairs = 20;
  int **keys = malloc(numpairs * sizeof(*keys));
  int *values = malloc(numpairs * sizeof(int));
  size_t i;
  for (i = 0; i < numpairs; ++i) {
    int *key = malloc(sizeof(int));
    *key = rand();
    keys[i] = key;
    int *value = malloc(sizeof(int));
    *value = rand();
    values[i] = *value;

    CU_ASSERT(0 == map_insert(map, key, sizeof(key), value));
  }

  for (i = 0; i < numpairs; ++i) {
    int *val = map_get(map, keys[i], sizeof(keys[i]));
    CU_ASSERT(values[i] == *val);
  }

  map->value_destroy = free;
  map->key_destroy = free;
  free(keys);
  free(values);
}

int main() {
  CU_pSuite llist_pSuite = NULL;
  CU_pSuite liter_pSuite = NULL;
  CU_pSuite alist_pSuite = NULL;
  CU_pSuite map_pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  llist_pSuite =
      CU_add_suite("LinkedList Suite", init_llist_suite, clean_llist_suite);
  liter_pSuite =
      CU_add_suite("List Iterator Suite", init_liter_suite, clean_liter_suite);
  alist_pSuite =
      CU_add_suite("ArrayList Suite", init_alist_suite, clean_alist_suite);
  map_pSuite = CU_add_suite("Map Suite", init_map_suite, clean_map_suite);
  if (NULL == llist_pSuite || NULL == liter_pSuite || NULL == alist_pSuite ||
      NULL == map_pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if (
      /* linked list tests */
      (NULL ==
       CU_add_test(llist_pSuite, "default values", test_llist_defaults)) ||
      (NULL ==
       CU_add_test(llist_pSuite, "queue functions", test_llist_queue)) ||
      (NULL ==
       CU_add_test(llist_pSuite, "deque functions", test_llist_deque)) ||
      (NULL ==
       CU_add_test(llist_pSuite, "error handling", test_llist_errors)) ||
      (NULL ==
       CU_add_test(llist_pSuite, "operation sequence", test_llist_sequence)) ||
      /* list iterator tests */
      (NULL ==
       CU_add_test(liter_pSuite, "list iter creation", test_liter_creation)) ||
      (NULL ==
       CU_add_test(liter_pSuite, "list iter mutation", test_liter_mutation)) ||
      (NULL ==
       CU_add_test(liter_pSuite, "list iteration", test_liter_iteration)) ||
      /* array list tests */
      (NULL ==
       CU_add_test(alist_pSuite, "default values", test_alist_defaults)) ||
      (NULL == CU_add_test(alist_pSuite, "array functions", test_alist_arr)) ||
      (NULL ==
       CU_add_test(alist_pSuite, "resize functions", test_alist_resize)) ||
      (NULL ==
       CU_add_test(alist_pSuite, "stack functions", test_alist_stack)) ||
      /* map tests */
      (NULL == CU_add_test(map_pSuite, "basic functions", test_map_basic)) ||
      (NULL == CU_add_test(map_pSuite, "bucket filling", test_map_buckets))) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
