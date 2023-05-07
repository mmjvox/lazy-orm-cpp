cmake_minimum_required(VERSION 3.5)

project(lazy-orm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include/
)

set(LazyORM_HEADERS
    include/DbVariant.h
    include/FilterVariant.h
    include/AbstractLazy.h
    include/FilteringAbstractLazy.h

    include/MariadbLazy.h
    include/MariadbFilteringLazy.h

    include/SqliteLazy.h
    include/SqliteFilteringLazy.h

    include/PostgreLazy.h
    include/PostgreFilteringLazy.h
)

add_library(lazy-orm STATIC
    ${LazyORM_HEADERS}

    src/DbVariant.cpp
    src/FilterVariant.cpp

    src/AbstractLazy.cpp
    src/FilteringAbstractLazy.cpp

    src/MariadbLazy.cpp
    src/MariadbFilteringLazy.cpp

    src/SqliteLazy.cpp
    src/SqliteFilteringLazy.cpp

    src/PostgreLazy.cpp
    src/PostgreFilteringLazy.cpp
)

target_compile_definitions(lazy-orm PRIVATE lazy-orm_LIBRARY)

option(tests "build tests" OFF)
if(tests)
add_subdirectory(tests)
endif(tests)


#set_target_properties(lazy-orm PROPERTIES PUBLIC_HEADER include)
include(GNUInstallDirs)
install(FILES ${LazyORM_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lazy-orm)
install(TARGETS lazy-orm
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lazy-orm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
