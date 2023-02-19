cmake_minimum_required(VERSION 3.5)

project(lazy-orm-test1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ../include/
    /home/mmjvox/dev-libs/install/include/
)

file(GLOB_RECURSE CppFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

foreach(each_file ${CppFiles})
    string(REPLACE ".cpp" "" EACH_NAME ${each_file})
    add_executable(${EACH_NAME} ${each_file})
    target_link_libraries(${EACH_NAME} PRIVATE lazy-orm)
endforeach()
