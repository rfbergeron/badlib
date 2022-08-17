#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "badalist.h"
#include "badllist.h"
#include "badmap.h"

static ArrayList *arraylist = NULL;
static LinkedList *linkedlist = NULL;
static Map *map = NULL;

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

void test_llist_defaults(void) {
  /* tests that the default values hold true for linked lists */
  CU_ASSERT_PTR_EQUAL(free, linkedlist->data_destroy);
  CU_ASSERT_PTR_EQUAL(linkedlist->anchor->next, linkedlist->anchor->prev);
  CU_ASSERT_PTR_EQUAL(linkedlist->anchor->next, linkedlist->anchor);
}

void test_llist_queue(void) {
  float *f = malloc(sizeof(float));
  *f = 2.5f;
  llist_push_front(linkedlist, f);
  CU_ASSERT(f == linkedlist->anchor->next->data);
  CU_ASSERT(*f == *((float *)linkedlist->anchor->next->data));
  CU_ASSERT(f == llist_front(linkedlist));
  CU_ASSERT(f == llist_pop_front(linkedlist));
  CU_ASSERT(linkedlist->size == 0);
  free(f);
}

void test_llist_deque(void) {
  float *f = malloc(sizeof(float));
  *f = 2.5f;
  llist_push_back(linkedlist, f);
  CU_ASSERT(f == linkedlist->anchor->prev->data);
  CU_ASSERT(*f == *((float *)linkedlist->anchor->prev->data));
  CU_ASSERT(f == llist_back(linkedlist));
  CU_ASSERT(f == llist_pop_back(linkedlist));
  CU_ASSERT(linkedlist->size == 0);
  free(f);
}

void test_llist_errors(void) {
  CU_ASSERT(1 == llist_push_front(NULL, NULL));
  CU_ASSERT(0 == llist_push_front(linkedlist, NULL));
  CU_ASSERT(NULL == llist_pop_front(NULL));
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

  CU_ASSERT(0 == llist_push_front(linkedlist, &f));
  CU_ASSERT(&f == llist_pop_front(linkedlist));
  CU_ASSERT(NULL == llist_pop_front(linkedlist));
  CU_ASSERT(0 == llist_push_front(linkedlist, i));
  CU_ASSERT(0 == llist_push_front(linkedlist, &v));
  CU_ASSERT(0 == llist_push_front(linkedlist, strong));
  CU_ASSERT(strong == llist_front(linkedlist));
  CU_ASSERT(strong == llist_pop_front(linkedlist));
  CU_ASSERT(&v == llist_pop_front(linkedlist));
  CU_ASSERT(i == llist_pop_front(linkedlist));
  CU_ASSERT(NULL == llist_pop_front(linkedlist));
  CU_ASSERT(1 == llist_empty(linkedlist));

  free(i);
}

int init_alist_suite(void) {
  arraylist = malloc(sizeof(*arraylist));
  if (arraylist == NULL || alist_init(arraylist, 10)) return 1;
  return 0;
}

int clean_alist_suite(void) {
  if (alist_destroy(arraylist, free)) return 1;
  free(arraylist);
  arraylist = NULL;
  return 0;
}

void test_alist_defaults(void) {
  CU_ASSERT_EQUAL(0, arraylist->count);
  CU_ASSERT_EQUAL(BLIB_SUCCESS, alist_status(arraylist));
}

void test_alist(void) {
  float *f = malloc(sizeof(float));
  *f = 1.25f;
  CU_ASSERT_PTR_NOT_NULL_FATAL(arraylist);
  CU_ASSERT(arraylist->size == 10);
  CU_ASSERT(0 == alist_insert(arraylist, f, 5, NULL));
  CU_ASSERT_PTR_EQUAL(alist_get(arraylist, 5), f);
  CU_ASSERT(0 == alist_delete(arraylist, 5, free));
  CU_ASSERT(1 == alist_empty(arraylist));
}

void test_alist_errors(void) {
  CU_ASSERT_PTR_NULL(alist_get(NULL, 10));
  CU_ASSERT_PTR_NULL(alist_get(arraylist, 11));
  CU_ASSERT(1 == alist_insert(arraylist, NULL, 11, free));
  CU_ASSERT(0 == alist_resize(arraylist, 0, NULL));
  CU_ASSERT_PTR_NULL(alist_get(arraylist, 0));
  CU_ASSERT(BLIB_OUT_OF_BOUNDS == alist_status(arraylist));
  CU_ASSERT(1 == alist_delete(arraylist, 0, free));
  CU_ASSERT(BLIB_OUT_OF_BOUNDS == alist_status(arraylist));
}

int init_map_suite(void) {
  /* use integers for keys */
  map = malloc(sizeof(Map));
  return map_init(map, 20, NULL, NULL, NULL);
}

int clean_map_suite(void) { return map_destroy(map); }

void test_map_basic(void) {
  int k = 5;
  char *v = "fuck you";
  CU_ASSERT(0 == map_insert(map, &k, sizeof(k), v));
  CU_ASSERT_PTR_EQUAL(v, map_get(map, &k, sizeof(k)));
  CU_ASSERT(0 == map_delete(map, &k, sizeof(k)));
  CU_ASSERT_PTR_NULL(map_get(map, &k, sizeof(k)));

  /* test that freeing of values works */
  map->value_destroy = free;
  float *fs = malloc(4 * sizeof(float));
  fs[0] = 5.5f;
  fs[3] = 1.6f;
  char *k2 = "eat ass";

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
  CU_pSuite pSuite = NULL;
  CU_pSuite pSuite2 = NULL;
  CU_pSuite pSuite3 = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  pSuite =
      CU_add_suite("LinkedList Suite", init_llist_suite, clean_llist_suite);
  pSuite2 =
      CU_add_suite("ArrayList Suite", init_alist_suite, clean_alist_suite);
  pSuite3 = CU_add_suite("Map Suite", init_map_suite, clean_map_suite);
  if (NULL == pSuite || NULL == pSuite2 || NULL == pSuite3) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if (
      /* linked list tests */
      (NULL == CU_add_test(pSuite, "default values", test_llist_defaults)) ||
      (NULL == CU_add_test(pSuite, "queue functions", test_llist_queue)) ||
      (NULL == CU_add_test(pSuite, "deque functions", test_llist_deque)) ||
      (NULL == CU_add_test(pSuite, "error handling", test_llist_errors)) ||
      (NULL ==
       CU_add_test(pSuite, "operation sequence", test_llist_sequence)) ||
      /* array list tests */
      (NULL == CU_add_test(pSuite2, "default values", test_alist_defaults)) ||
      (NULL == CU_add_test(pSuite2, "basic functions", test_alist)) ||
      (NULL == CU_add_test(pSuite2, "error handling", test_alist_errors)) ||
      (NULL == CU_add_test(pSuite3, "basic functions", test_map_basic)) ||
      (NULL == CU_add_test(pSuite3, "bucket filling", test_map_buckets))
      /* map tests */
  ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
