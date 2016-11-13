add_library(libdocket STATIC
    cfdm.c
    trie.c
    filter.c
    strbuf.c
    options.c
)

add_executable(docket
    main.c
    command.c
    docket.c
    config.c
    trie.c
    trie_util.c
    filter.c
    strbuf.c
    cfdm.c
    scanner.c
    abspath.c
    options.c
    dckerr.c
    report.c
    kv.c
    commands/add.c
    commands/list.c
    commands/tree.c
    commands/timer.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

if(FLAVOUR STREQUAL prod)
    target_compile_definitions(docket PRIVATE DOCKET_TIMER_STD_PATH="$ENV{HOME}/.docket_timer")
elseif(FLAVOUR STREQUAL test)
    target_compile_definitions(docket PRIVATE DOCKET_TIMER_STD_PATH="$ENV{HOME}/.docket_timer_test")
endif()

target_include_directories(docket PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

install (TARGETS docket  DESTINATION bin)
