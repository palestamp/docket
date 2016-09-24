#include <stdlib.h>
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

void test_trie_insert_path_many(void) {
	struct word_trie *root = trie_new();
	struct word_trie *trie = NULL;
	struct word_trie *trie2 = NULL;
	trie_insert_path(root, "a");
	trie_insert_path(root, "b");
	trie_insert_path(root, "c");
	trie_insert_path(root, "d");
	trie_insert_path(root, "e");
	trie_insert_path(root, "f");
	trie_insert_path(root, "g");
	trie_insert_path(root, "h");
	trie_insert_path(root, "i");
	trie_insert_path(root, "j");
	trie_insert_path(root, "k");
	trie_insert_path(root, "l");
	trie_insert_path(root, "m");
	trie_insert_path(root, "n");
	trie_insert_path(root, "o");
	trie_insert_path(root, "p");
	trie_insert_path(root, "q");
	trie_insert_path(root, "r");
	trie_insert_path(root, "s");
	trie_insert_path(root, "t");

	trie = trie_get_path(root, "moody:banana");
	trie = trie_get_path(trie, "going:to:dance");
	trie2 = trie_get_path(root, "moody:banana:going:to:dance");

	trie_print(root);
	TEST_ASSERT_EQUAL(trie, trie2);
}

static int
trie_last_level(struct word_trie *trie) {
	if(trie->len) {
		return 0;
	}
	return 1;
}

void test_trie_loop(void) {
	struct word_trie *root = trie_new();
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "programming:langs:c");
	trie_insert_path(root, "programming:git");
	trie_insert_path(root, "programming:langs:python:env");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "programming:langs:c");
	trie_insert_path(root, "life:common:credentials");
	trie_insert_path(root, "programming:langs:c");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "programming:environ:vim");
	trie_insert_path(root, "life:education");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "work:schibsted:techskills:postgresql");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "life:money:pennywise");
	trie_insert_path(root, "programming:environ");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "programming:dsa");
	trie_insert_path(root, "programming:regex");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "programming:langs:bash");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "programming:langs:bash");
	trie_insert_path(root, "programming:langs:golang");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "programming:langs:python");
	trie_insert_path(root, "life:common:credentials");
	trie_insert_path(root, "programming:patterns");
	trie_insert_path(root, "programming:common:books");
	trie_insert_path(root, "programming:git");
	trie_insert_path(root, "programming:git");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "life:education");
	trie_insert_path(root, "do");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "programming:langs:c:learning");
	trie_insert_path(root, "work:schibsted");
	trie_insert_path(root, "work:schibsted");
	trie_insert_path(root, "programming:environ:vim");
	trie_insert_path(root, "programming:environ:vim");
	trie_insert_path(root, "programming:common:linux");
	trie_insert_path(root, "programming:common");
	trie_insert_path(root, "work:schibsted");
	trie_insert_path(root, "work:schibsted:english");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "dnotes");
	trie_insert_path(root, "projects:owl");
	trie_insert_path(root, "dnotes");
	trie_insert_path(root, "programming:lang:c");
	trie_insert_path(root, "programming:langs:c");
	trie_insert_path(root, "programming");
	trie_insert_path(root, "programming:regex");
	trie_insert_path(root, "work:schibsted:techskills");
	trie_insert_path(root, "work:schibsted:passwords");
	trie_insert_path(root, "projects:metadb");
	trie_insert_path(root, "projects:rtq");

	struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
	TRIE_LOOP_INIT(&loop);

	while(1) {
        loop_ptr = trie_loop_branch(root, loop_ptr, trie_last_level) ;
		if (loop_ptr == NULL) break;
		loop_stack_print(loop_ptr);
	};
}

void test_insert_by_path(void) {
	struct word_trie *root = trie_new();
    const char *first = "first";
    const char *second = "second";
    const char *third = "third";
    const char *fourth = "fourth";

	trie_insert_by_path(root, "programming:langs:c", (void *)first);
	trie_insert_by_path(root, "programming:langs:c", (void *)second);
	trie_insert_by_path(root, "programming:langs:c", (void *)third);
	trie_insert_by_path(root, "programming:langs:c", (void *)fourth);

    struct word_trie *req = trie_get_path(root, "programming:langs:c"); 
    if (req) {
        struct leaf_list *entry = NULL; 
        TAILQ_FOREACH(entry, &req->leafs, leaf) {
            printf("%s\n", (char *)entry->data);
        }
    }
}


int
main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_trie_get_path);
	RUN_TEST(test_trie_get_path2);
	RUN_TEST(test_trie_insert_path);
	RUN_TEST(test_trie_insert_path2);
	RUN_TEST(test_trie_insert_path_many);
	RUN_TEST(test_trie_loop);
	RUN_TEST(test_insert_by_path);
	UNITY_END();
}
