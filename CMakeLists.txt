cmake_minimum_required(VERSION 3.20)
project(kvstore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(store
    internal/store/store.cpp
)

target_include_directories(store PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/store
)

add_library(cli
    internal/cli/parser.cpp
)

target_include_directories(cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/cli
)

add_executable(kvstore
    cmd/main.cpp
)

target_link_libraries(kvstore PRIVATE
    store
    cli
)

# Enable testing
include(CTest)

# Download GoogleTest
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 7140cd416cecd7462a8aae488024abeee55598e4
)

# Avoid runtime library mismatch issues
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test Executable
add_executable(kvstore_test
    tests/store_test.cpp
)

target_link_libraries(kvstore_test
    PRIVATE
        store
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(kvstore_test)