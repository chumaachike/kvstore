cmake_minimum_required(VERSION 3.20)
project(kvstore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(store
    src/store.cpp
)

target_include_directories(store
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(parser
    src/parser.cpp
)

target_include_directories(parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(server
    src/server.cpp
)

add_library(server_loop
    src/server_loop.cpp
)

target_include_directories(server_loop
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(server
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(command_executor
    src/command_executor.cpp
)

target_include_directories(command_executor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(command_executor
    PUBLIC
        store
        parser
)

target_link_libraries(server_loop
    PUBLIC
        command_executor
)

add_executable(kvstore
    src/main.cpp
)

target_link_libraries(kvstore
    PRIVATE
        store
        parser
        server
        command_executor
        server_loop
)

include(CTest)
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 7140cd416cecd7462a8aae488024abeee55598e4
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(kvstore_test
    tests/store_test.cpp
    tests/parser_test.cpp
    tests/concurrency_test.cpp
    tests/command_executor_test.cpp
    tests/tcp_connection_test.cpp
)

target_link_libraries(kvstore_test
    PRIVATE
        store
        parser
        command_executor
        server_loop
        server
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(kvstore_test)