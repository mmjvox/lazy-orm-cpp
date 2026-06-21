cmake_minimum_required(VERSION 3.15)
project(lazyorm VERSION 1.0.12 LANGUAGES CXX)
add_definitions (-Dlazy_orm_version="${CMAKE_PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)

find_package(lazyvariant CONFIG QUIET)

if(lazyvariant_FOUND)
    message(STATUS "Using installed lazyvariant ${lazyvariant_VERSION}")
else()
    set(_lazyvariant_submodule "")
    foreach(_candidate IN ITEMS lazy-variant lazy-variant)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_candidate}/CMakeLists.txt")
            set(_lazyvariant_submodule "${_candidate}")
            break()
        endif()
    endforeach()

    if(_lazyvariant_submodule)
        message(STATUS "lazyvariant not found; using bundled ${_lazyvariant_submodule}")
        add_subdirectory(${_lazyvariant_submodule} _lazyvariant_submodule)
    else()
        message(FATAL_ERROR "lazyvariant not found. Install it and set lazyvariant_DIR or provide the submodule.")
    endif()
endif()

if(NOT TARGET lazyvariant::lazyvariant)
    if(TARGET lazyvariant)
        add_library(lazyvariant::lazyvariant ALIAS lazyvariant)
    else()
        message(FATAL_ERROR "lazyvariant target is unavailable")
    endif()
endif()

set(LazyORM_HEADERS
    include/WhereFilter.h
    include/WhereFilterPostgre.h
    include/HavingFilter.h
    include/FilterVariant.h
    include/AbstractLazy.h
    include/FilteringAbstractLazy.h
    include/LazyOrm.h
    include/FilteringLazy.h

    include/MariadbLazy.h
    include/MariadbFilteringLazy.h

    include/SqliteLazy.h
    include/SqliteFilteringLazy.h

    include/PostgreLazy.h
    include/PostgreFilteringLazy.h

    include/Transaction.h
    include/TransactionMariadb.h
    include/TransactionPostgre.h
    include/TransactionSqlite.h
    include/LazyTransaction.h
)

add_library(lazyorm STATIC
    ${LazyORM_HEADERS}

    src/WhereFilter.cpp
    src/FilterVariant.cpp
    src/WhereFilterPostgre.cpp
    src/LazyOrm.cpp
    src/FilteringLazy.cpp

    src/AbstractLazy.cpp
    src/FilteringAbstractLazy.cpp

    src/MariadbLazy.cpp
    src/MariadbFilteringLazy.cpp

    src/SqliteLazy.cpp
    src/SqliteFilteringLazy.cpp

    src/PostgreLazy.cpp
    src/PostgreFilteringLazy.cpp

    src/Transaction.cpp
    src/TransactionMariadb.cpp
    src/TransactionPostgre.cpp
    src/TransactionSqlite.cpp
    src/LazyTransaction.cpp

    include/JoinAbstract.h
    src/JoinAbstract.cpp
    include/JoinMariadb.h
    src/JoinMariadb.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC lazyvariant::lazyvariant)

target_compile_definitions(lazyorm PRIVATE LazyOrm_LIBRARY)


option(orm_tests "build tests of lazy-orm-cpp" OFF)
if(orm_tests)
add_subdirectory(tests)
endif(orm_tests)


target_include_directories(lazyorm
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm>
)


string(JOIN ";" LazyORM_HEADERS_STRING ${LazyORM_HEADERS})
set_target_properties(lazyorm PROPERTIES
    PUBLIC_HEADER "${LazyORM_HEADERS_STRING}"
)

# install(TARGETS lazyorm)


include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lazyormConfig.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/lazyorm
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lazyormConfig.cmake
#              ${CMAKE_CURRENT_BINARY_DIR}/FooConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazyorm)
install(TARGETS lazyorm
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
