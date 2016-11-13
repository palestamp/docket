cmake_minimum_required (VERSION 2.6)

set(CMAKE_C_COMPILER gcc)
SET(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-missing-field-initializers")

project ("docket" C)

set(DOCKET_MAJOR_VERSION 0)
set(DOCKET_MINOR_VERSION 0)
set(DOCKET_PATCH_VERSION 1)

set(FLAVOUR prod CACHE STRING "Group to build")

add_subdirectory(src)


if(FLAVOUR STREQUAL prod)
    add_definitions(-DDOCKET_TIMER_STD_PATH $ENV{HOME}/.docket_timer)
elseif(FLAVOUR STREQUAL test)
    include(CTest)
    add_subdirectory(external)
    add_subdirectory(test)
else()
    message(FATAL_ERROR "Given FLAVOUR unknown")
endif()

