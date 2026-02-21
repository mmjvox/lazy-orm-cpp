cmake_minimum_required(VERSION 3.5)

project(lazy-orm-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

include_directories(
    ../include/
)

file(GLOB_RECURSE CppFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include(CTest)
include(Catch)

foreach(each_file ${CppFiles})
    string(REPLACE ".cpp" "" EACH_NAME ${each_file})
    string(REPLACE "/" "_" EACH_NAME ${EACH_NAME})
    add_executable(${EACH_NAME} ${each_file})
    target_link_libraries(${EACH_NAME} PRIVATE lazyorm Catch2::Catch2WithMain)
    catch_discover_tests(${EACH_NAME})
endforeach()

