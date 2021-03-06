#include <string.h>
#include <stdlib.h>
#include "unity.h"
#include "../src/trie.h"
#include "../src/filter.h"

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

    char test_buf[1024] = "";

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
    TRIE_BRANCH_LOOP_INIT(&loop, NULL);

    int i = 0;

    while((loop_ptr = trie_loop_branch(root, loop_ptr))) {
        memset(test_buf, 0, 1024);
        sprintf(test_buf, "%d %s" ,loop_ptr->depth,  loop_stack_sprint(loop_ptr));

        if (comp[i] != NULL) {
            TEST_ASSERT_EQUAL_STRING(comp[i], test_buf);
            i++;
        }
    }
}

void test_loop_branch_one_node(void) {
    struct word_trie *root = trie_new();
    trie_insert_path(root, "programming");
    trie_insert_path(root, "langs");
    trie_insert_path(root, "git");
    trie_insert_path(root, "python");

    char test_buf[1024] = "";
    char *comp[] = {
        "0 programming",
        "0 langs",
        "0 git",
        "0 python",
        NULL
    };

    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    TRIE_BRANCH_LOOP_INIT(&loop, NULL);
    int i = 0;
    while((loop_ptr = trie_loop_branch(root, loop_ptr))) {
        memset(test_buf, 0, 1024);
        sprintf(test_buf, "%d %s" ,loop_ptr->depth,  loop_stack_sprint(loop_ptr));

        if (comp[i] != NULL) {
            TEST_ASSERT_EQUAL_STRING(comp[i], test_buf);
            i++;
        }
    }
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
        struct data_leaf *entry = NULL;
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

    const char *expect[] = {"0", "1", "2", "3", NULL};
    trie_insert_by_path(root, "programming:langs:c:0", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs:c:1", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:b:1", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c:2", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:3", (void *)(comp[3]));

    root = trie_get_path(root, "programming:langs:c");
    struct word_trie *loop_trie = NULL;

    int i = 0;
    while((loop_trie = trie_loop_children(loop_trie, root))) {
        if (expect[i]) {
            TEST_ASSERT_EQUAL_STRING(expect[i], loop_trie->word);
            i++;
        }
    }
}


void test_filter_branch_simple(void) {
    struct word_trie *root = trie_new();
    trie_insert_path(root, "1:2:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:6");
    trie_insert_path(root, "1:a:3:4:5:g");

    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("1:*:*:*:*:g");
    TRIE_DATA_LOOP_INIT(&loop, pf);
    char buf[1024] = "";
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        sprintf(buf, "%d %s", loop_ptr->depth,  loop_stack_sprint(loop_ptr));
        TEST_ASSERT_EQUAL_STRING(buf, "5 1:a:3:4:5:g");
    }
}

void test_filter_final_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("programming:langs:c:a");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_filter_accompanying_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("programming:langs:c:a");
    TRIE_DATA_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(counter, 1);
}

void test_filter_accompanying_tilda_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("programming:~");
    TRIE_DATA_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(3, counter);
}

void test_end_filter_tilda_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));
    trie_insert_by_path(root, "programming:langs:c:b", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("programming:~");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(2, counter);
}

void test_accompanying_filter_tilda_only_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));
    trie_insert_by_path(root, "programming:langs:c:b", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("~");
    TRIE_DATA_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(5, counter);
}

void test_end_filter_tilda_only_nodes(void) {
    struct word_trie *root = trie_new();
    const char *comp[] = {
        "first",
        "second",
        "third",
        "fourth",
        NULL
    };

    trie_insert_by_path(root, "programming", (void *)(comp[0]));
    trie_insert_by_path(root, "programming:langs", (void *)(comp[1]));
    trie_insert_by_path(root, "programming:langs:c", (void *)(comp[2]));
    trie_insert_by_path(root, "programming:langs:c:a", (void *)(comp[3]));
    trie_insert_by_path(root, "programming:langs:c:b", (void *)(comp[3]));


    int counter = 0;
    struct trie_loop loop = {0};
    struct trie_loop *loop_ptr = &loop;
    struct path_filter *pf = compile_filter_from_s("~");
    TRIE_BRANCH_LOOP_INIT(&loop, pf);
    while((loop_ptr = trie_filter_branch(root, loop_ptr))) {
        counter++;
    }

    TEST_ASSERT_EQUAL_INT(2, counter);
}
int
main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_trie_insert_path);
    RUN_TEST(test_trie_insert_path2);
    RUN_TEST(test_trie_loop);
    RUN_TEST(test_trie_loop_children);
    RUN_TEST(test_insert_by_path);
    RUN_TEST(test_loop_branch_one_node);
    RUN_TEST(test_filter_branch_simple);
    RUN_TEST(test_filter_final_nodes);
    RUN_TEST(test_filter_accompanying_nodes);
    RUN_TEST(test_filter_accompanying_tilda_nodes);
    RUN_TEST(test_end_filter_tilda_nodes);
    RUN_TEST(test_accompanying_filter_tilda_only_nodes);
    RUN_TEST(test_end_filter_tilda_only_nodes);
    return UNITY_END();
}
