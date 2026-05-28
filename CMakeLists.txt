cmake_minimum_required(VERSION 3.20)
project(kvstore)

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