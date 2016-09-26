add_library(libdocket STATIC
    cfdm.c
    trie.c
    strbuf.c
)

add_executable(docket
    main.c
    docket.c
    config.c
    trie.c
    strbuf.c
    cfdm.c
    scanner.c
    abspath.c
    commands/add.c
    commands/list.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

target_include_directories(docket PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
