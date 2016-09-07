cmake_minimum_required (VERSION 2.6)

set(CMAKE_C_COMPILER gcc)

project ("cowl" C)

set(COWL_MAJOR_VERSION 0)
set(COWL_MINOR_VERSION 0)
set(COWL_PATCH_VERSION 1)

set(TARGET_GROUP production CACHE STRING "Group to build")

add_subdirectory(src)


if(TARGET_GROUP STREQUAL production)
elseif(TARGET_GROUP STREQUAL test)
    include(CTest)

    add_subdirectory(external)
    add_subdirectory(test)
else()
    message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()
