#include "unity.h"
#include "../src/trie.h"

void test_trie_get_path(void) {
    struct word_trie *root = trie_new();
    struct word_trie *trie = NULL;
    trie = trie_add(root, "a");
    trie = trie_add(trie, "b");
    trie = trie_add(trie, "c");
    trie = trie_add(trie, "d");
    struct word_trie *trie2 = trie_get_path(root, "a:b:c:d");
    TEST_ASSERT_EQUAL(trie, trie2);
}

void test_trie_get_path2(void) {
    struct word_trie *root = trie_new();
    struct word_trie *trie = NULL;
    trie = trie_add(root, "moody");
    trie = trie_add(trie, "banana");
    trie = trie_add(trie, "going");
    trie = trie_add(trie, "to");
    trie = trie_add(trie, "dance");
    struct word_trie *trie2 = trie_get_path(root, "moody:banana:going:to:dance");
    TEST_ASSERT_EQUAL(trie, trie2);
}

void test_trie_insert_path(void) {
    struct word_trie *root = trie_new();
    struct word_trie *trie = NULL;
    struct word_trie *trie2 = NULL;
    trie = trie_insert_path(root, "moody:banana:going:to:dance");
    trie2 = trie_get_path(root, "moody:banana:going:to:dance");

    TEST_ASSERT_EQUAL(trie, trie2);
}

void test_trie_insert_path2(void) {
    struct word_trie *root = trie_new();
    struct word_trie *trie = NULL;
    struct word_trie *trie2 = NULL;
    trie_insert_path(root, "moody:banana:going:to:dance");
    trie_insert_path(root, "moody:orange:going:to:dance");
    trie_insert_path(root, "pretty:orphan:going:to:dance");
    
    trie = trie_get_path(root, "moody:banana");
    trie = trie_get_path(trie, "going:to:dance");
    trie2 = trie_get_path(root, "moody:banana:going:to:dance");

    TEST_ASSERT_EQUAL(trie, trie2);
}

int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_trie_get_path);
    RUN_TEST(test_trie_get_path2);
    RUN_TEST(test_trie_insert_path);
    RUN_TEST(test_trie_insert_path2);
    UNITY_END();
}
