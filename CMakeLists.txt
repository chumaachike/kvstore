cmake_minimum_required(VERSION 3.20)
project(kvstore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(store
    src/store.cpp
)

add_library(logger
    src/logger.cpp
)

target_include_directories(store
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(logger
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(parser
    src/parser.cpp
)

add_library(persistence
    src/persistence.cpp
)

target_include_directories(persistence
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(server
    src/server.cpp
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
        logger
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
        logger
        persistence
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
    tests/aof_test.cpp
)

target_link_libraries(kvstore_test
     PRIVATE
        store
        parser
        command_executor
        persistence
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(kvstore_test)