cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)

project(arbitrary_precision_float LANGUAGES CXX)

option(ENABLE_ASAN "Enable address santizer" false)
option(ENABLE_UBSAN "Enable undefined behaviour santizer" false)

option(ENABLE_TESTS "Generate unit tests" true)

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

include_directories(${PROJECT_SOURCE_DIR}/include)

if(ENABLE_TESTS)
    find_package(Catch2 REQUIRED)

    add_executable(tests
        ${PROJECT_SOURCE_DIR}/tests/test_main.cpp
        ${PROJECT_SOURCE_DIR}/tests/test_bitarray.cpp
        ${PROJECT_SOURCE_DIR}/tests/test_float.cpp
    )

    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
    target_compile_options(tests PUBLIC -Wall -Werror -Werror=vla)
endif()
