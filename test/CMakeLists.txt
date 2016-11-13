add_executable(trie_test Test_trie.c)
target_link_libraries(trie_test libdocket Unity)
add_test(word_trie trie_test)

add_test(interactive_tree "python" "../../test/test.py")

find_program (BASH_PROGRAM bash)
if(BASH_PROGRAM)
    add_test(timers ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/integration/runner timers)
    add_test(tree ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/integration/runner tree)
endif (BASH_PROGRAM)
