cmake_minimum_required(VERSION 3.5)

project(lazy-orm-test1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../include/
)

add_executable(lazy-orm-test1 main.cpp)

target_link_libraries(lazy-orm-test1 PRIVATE lazy-orm)
