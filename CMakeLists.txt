cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)

project(arbitrary_precision_float LANGUAGES CXX)

option(ENABLE_ASAN "Enable address santizer" false)
option(ENABLE_UBSAN "Enable undefined behaviour santizer" false)

if(ENABLE_ASAN)
    message(STATUS "Enabled ASAN.")
    string(APPEND CMAKE_CXX_FLAGS " -fsanitize=address ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS "-fsanitize=address ")
endif()

if(ENABLE_UBSAN)
    message(STATUS "Enabled UBSAN.")
    string(APPEND CMAKE_CXX_FLAGS " -fsanitize=undefined ")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=undefined ")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

string(APPEND CMAKE_CXX_FLAGS " -Wall -Werror -Werror=vla ")

add_executable(tests
    ${CMAKE_SOURCE_DIR}/tests/test_main.cpp
    ${CMAKE_SOURCE_DIR}/tests/test_bitarray.cpp
    ${CMAKE_SOURCE_DIR}/tests/test_float.cpp
)
