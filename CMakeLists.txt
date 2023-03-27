cmake_minimum_required(VERSION 3.5)

project(lazy-orm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include/
)

add_library(lazy-orm STATIC

    include/DbVariant.h
    src/DbVariant.cpp
    include/FilterVariant.h
    src/FilterVariant.cpp

    include/AbstractLazy.h
    src/AbstractLazy.cpp
    include/FilteringAbstractLazy.h
    src/FilteringAbstractLazy.cpp

    include/MariadbLazy.h
    src/MariadbLazy.cpp
    include/MariadbFilteringLazy.h
    src/MariadbFilteringLazy.cpp

    include/SqliteLazy.h
    src/SqliteLazy.cpp
    include/SqliteFilteringLazy.h
    src/SqliteFilteringLazy.cpp
)

target_compile_definitions(lazy-orm PRIVATE lazy-orm_LIBRARY)

option(tests "build tests" OFF)
if(tests)
add_subdirectory(tests)
endif(tests)
