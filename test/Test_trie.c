#include <stdlib.h>
#include <string.h>
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

    trie = trie_get_path(trie_get_path(root, "moody:banana"), "going:to:dance");
    trie2 = trie_get_path(root, "moody:banana:going:to:dance");

    TEST_ASSERT_EQUAL(trie, trie2);
}

static int
trie_last_level(const struct word_trie *trie) {
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

    char test_buf[1024];

    char *comp[] = {
        "2 programming:common:books",
        "2 programming:common:linux",
        "3 programming:langs:c:learning",
        "3 programming:langs:python:env",
        "2 programming:langs:bash",
        "2 programming:langs:golang",
        "1 programming:git",
        "2 programming:environ:vim",
        "1 programming:dsa",
        "1 programming:regex",
        "1 programming:patterns",
        "2 programming:lang:c",
        "2 life:common:credentials",
        "1 life:education",
        "2 life:money:pennywise",
        "1 projects:owl",
        "1 projects:metadb",
        "1 projects:rtq",
        "3 work:schibsted:techskills:postgresql",
        "2 work:schibsted:english",
        "2 work:schibsted:passwords",
        "0 do",
        "0 dnotes",
        NULL
    };

    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    TRIE_LOOP_INIT(&loop);

    int i = 0;

    while(1) {
        memset(test_buf, 0, 1024);
        loop_ptr = trie_loop_branch(root, loop_ptr, trie_last_level) ;
        if (loop_ptr == NULL) break;
        sprintf(test_buf, "%d %s" ,loop_ptr->depth,  loop_stack_sprint(loop_ptr));

        if (comp[i] != NULL) {
            TEST_ASSERT_EQUAL_STRING(comp[i], test_buf);
            i++;
        }
    };
}

void test_insert_by_path(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[3]));

    struct word_trie *req = trie_get_path(root, "programming:langs:c");
    if (req) {
        int i = 0;
        char buf[1024] = "";
        struct leaf_list *entry = NULL;
        TAILQ_FOREACH(entry, &req->leafs, leaf) {
            sprintf(buf, "%s", (char *)entry->data);
            TEST_ASSERT_EQUAL_STRING(buf, comp[i]);
            i++;
        }
    } else {
        TEST_FAIL();
    }
}

void
test_trie_loop_children(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };
    trie_insert_by_path(root, "programming:langs:c:0", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs:c:1", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c:2", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:3", (void *)(comp[3]));

    root = trie_get_path(root, "programming:langs:c");
    struct word_trie *loop_trie = NULL;
    while((loop_trie = trie_loop_children(loop_trie, root))) {
        fprintf(stderr, "%s", loop_trie->word);
    }
}


int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_trie_get_path);
    RUN_TEST(test_trie_get_path2);
    RUN_TEST(test_trie_insert_path);
    RUN_TEST(test_trie_insert_path2);
    RUN_TEST(test_trie_loop);
    RUN_TEST(test_trie_loop_children);
    RUN_TEST(test_insert_by_path);

    return UNITY_END();
}
