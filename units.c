#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "badllist.h"
#include <CUnit/Basic.h>

static LinkedList *list = NULL;

int init_llist_suite(void) {
    list = malloc(sizeof(*list));
    if (list == NULL || llist_init(list)) return 1;
    return 0;
}

int clean_llist_suite(void) {
    if(llist_destroy(list)) return 1;
    free(list);
    return 0;
}

void test_llist_push_pop_peek(void) {
    float *f = malloc(sizeof(float));
    *f = 2.5f;
    llist_push(list, f);
    CU_ASSERT(f == list->anchor->next->data);
    CU_ASSERT(*f == *((float*)list->anchor->next->data));
    CU_ASSERT(f == llist_peek(list));
    CU_ASSERT(f == llist_pop(list));
    CU_ASSERT(list->size == 0);
    free(f);
}

void test_llist_errors(void) {
    CU_ASSERT(1 == llist_push(NULL, NULL));
    CU_ASSERT(0 == llist_push(list, NULL));
    CU_ASSERT(NULL == llist_pop(NULL));
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
    struct vec v = { 2.5f, 3.14f, 1.0f };

    CU_ASSERT(0 == llist_push(list, &f));
    CU_ASSERT(&f == llist_pop(list));
    CU_ASSERT(NULL == llist_pop(list));
    CU_ASSERT(0 == llist_push(list, i));
    CU_ASSERT(0 == llist_push(list, &v));
    CU_ASSERT(0 == llist_push(list, strong));
    CU_ASSERT(strong == llist_peek(list));
    CU_ASSERT(strong == llist_pop(list));
    CU_ASSERT(&v == llist_pop(list));
    CU_ASSERT(i == llist_pop(list));
    CU_ASSERT(NULL == llist_pop(list));
    CU_ASSERT(1 == llist_empty(list));

    free(i);
}

int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("LinkedList Suite 1", init_llist_suite, clean_llist_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(pSuite, "test of push, pop, peek", test_llist_push_pop_peek)) ||
        (NULL == CU_add_test(pSuite, "test of error handling", test_llist_errors)) ||
        (NULL == CU_add_test(pSuite, "test of operation sequence", test_llist_sequence)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
