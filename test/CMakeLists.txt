add_executable(suite_1_app
    Test_cfdm.c
)

target_link_libraries(suite_1_app
    docket
    Unity
)

add_test(suite_1_test suite_1_app)
