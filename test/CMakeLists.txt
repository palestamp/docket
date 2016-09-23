add_executable(suite_1_app Test_cfdm.c)
target_link_libraries(suite_1_app libdocket Unity)
add_test(suite_1_test suite_1_app)


add_executable(suite_2_app Test_trie.c)
target_link_libraries(suite_2_app libdocket Unity)
add_test(suite_2_test suite_2_app)

