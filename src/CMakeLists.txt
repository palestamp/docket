add_library(libdocket STATIC
    cfdm.c
    trie.c
)

add_executable(docket
    main.c
    add.c
    config.c
    trie.c
)
 
target_include_directories(docket PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
