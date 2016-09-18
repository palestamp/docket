#include <stdlib.h>

#include "unity.h"

#include "../src/list.h"

static int
cmp_ints(const void *a, const void *b) {
    if(!(a && b)) {
        return -1;
    }
    int *ad = (int *)a;
    int *bd = (int *)b;
    return *ad - *bd;
}

void test_lnode_has(void) {
    struct lnode l = { .next = NULL, .data = NULL };
    int a = 1;
    int b = 1;
    l.data = (void *)(&a);

    TEST_ASSERT_EQUAL_INT(1, list_has(&l, (void *)(&b), cmp_ints)); 
}

void test_lnode_has2(void) {
    struct lnode l = { .next = NULL, .data = NULL };
    int a = 1;
    int b = 2;
    l.data = (void *)(&a);

    TEST_ASSERT_EQUAL_INT(0, list_has(&l, (void *)(&b), cmp_ints)); 
}

void test_lnode_has_ptr(void) {
    struct lnode l = { .next = NULL, .data = NULL };
    int a = 1;
    l.data = (void *)(&a);

    TEST_ASSERT_EQUAL(1, list_has(&l, (void *)(&a), NULL)); 
}

void test_lnode_has_ptr2(void) {
    struct lnode l = { .next = NULL, .data = NULL };
    int a = 1;
    int b = 1;
    l.data = (void *)(&a);

    TEST_ASSERT_EQUAL(0, list_has(&l, (void *)(&b), NULL)); 
}

void test_list_insert(void) {
    int data = 134;
    int data2 = 234;
    struct lnode l = { .next = NULL, .data = NULL };
    list_insert(&l, (void *)(&data));
    list_insert(&l, NULL);
    list_insert(&l, NULL);
    TEST_ASSERT_EQUAL_INT(1, list_has(&l, (void *)(&data), cmp_ints));
    TEST_ASSERT_EQUAL_INT(0, list_has(&l, (void *)(&data2), cmp_ints));
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lnode_has);
    RUN_TEST(test_lnode_has2);
    RUN_TEST(test_lnode_has_ptr);
    RUN_TEST(test_lnode_has_ptr2);
    RUN_TEST(test_list_insert);
    UNITY_END();
}
