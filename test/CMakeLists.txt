add_executable(trie_test Test_trie.c)
target_link_libraries(trie_test libdocket Unity)
add_test(word_trie trie_test)

# add_test(interactive_tree "python" "../../test/test_tree.py")
