#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "badllist.h"
#include "badalist.h"


static ArrayList *arraylist = NULL;
static LinkedList *linkedlist = NULL;

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

int init_alist_suite(void) {
  arraylist = malloc(sizeof(*arraylist));
  if (arraylist == NULL || alist_init(arraylist, 10)) return 1;
  return 0;
}

int clean_alist_suite(void) {
  if (alist_destroy(arraylist)) return 1;
  free(arraylist);
  arraylist = NULL;
  return 0;
}

void test_alist(void) {
    float *f = malloc(sizeof(float));
    *f = 1.25f;
    CU_ASSERT_PTR_NOT_NULL_FATAL(arraylist);
    CU_ASSERT(arraylist->size == 10);
    CU_ASSERT(arraylist->max_size == 16);
    CU_ASSERT(0 == alist_insert(arraylist, f, 5));
    CU_ASSERT_PTR_EQUAL(alist_get(arraylist, 5), f);
    CU_ASSERT(0 == alist_delete(arraylist, 5));
    free(f);
}

void test_alist_errors(void) {
    CU_ASSERT_PTR_NULL(alist_get(NULL, 10));
    CU_ASSERT_PTR_NULL(alist_get(arraylist, 11));
    CU_ASSERT(1 == alist_insert(arraylist, NULL, 11));
    CU_ASSERT(1 == alist_resize(arraylist, 0));
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

int main() {
  CU_pSuite pSuite = NULL;
  CU_pSuite pSuite2 = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  pSuite =
      CU_add_suite("LinkedList Suite 1", init_llist_suite, clean_llist_suite);
  pSuite2 =
      CU_add_suite("ArrayList Suite 1", init_alist_suite, clean_alist_suite);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL ==
       CU_add_test(pSuite, "test of queue functions", test_llist_queue)) ||
      (NULL ==
       CU_add_test(pSuite, "test of error handling", test_llist_errors)) ||
      (NULL == CU_add_test(pSuite, "test of operation sequence",
                           test_llist_sequence)) ||
      (NULL == CU_add_test(pSuite2, "test of basic array",
                           test_alist)) ||
      (NULL == CU_add_test(pSuite2, "test of array error handling",
                           test_alist_errors)) ||
      (NULL ==
       CU_add_test(pSuite, "test of deque functions", test_llist_deque))) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
