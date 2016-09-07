add_library(docket STATIC
    cfdm.c
)
 
target_include_directories(docket PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
