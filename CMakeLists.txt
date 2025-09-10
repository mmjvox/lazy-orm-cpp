cmake_minimum_required(VERSION 3.15)
project(lazyorm VERSION 1.0.8 LANGUAGES CXX)
add_definitions (-Dlazy_orm_version="${CMAKE_PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    include/
)

set(LazyORM_HEADERS
    include/DbVariant.h
    include/DbList.h
    include/WhereFilter.h
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

    include/Result.h
    include/ResultRow.h

)

add_library(lazyorm STATIC
    ${LazyORM_HEADERS}

    src/DbVariant.cpp
    src/DbList.cpp
    src/WhereFilter.cpp
    src/FilterVariant.cpp
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

    src/Result.cpp
    src/ResultRow.cpp
)

target_compile_definitions(lazyorm PRIVATE LazyOrm_LIBRARY)


option(tests "build tests" OFF)
if(tests)
add_subdirectory(tests)
endif(tests)


target_include_directories(lazyorm PUBLIC include)


string(JOIN ";" LazyORM_HEADERS_STRING ${LazyORM_HEADERS})
set_target_properties(lazyorm PROPERTIES
    PUBLIC_HEADER "${LazyORM_HEADERS_STRING}"
)

# install(TARGETS lazyorm)


include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lazyormConfig.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/lazyorm
)

install(FILES ${LazyORM_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lazyormConfig.cmake
#              ${CMAKE_CURRENT_BINARY_DIR}/FooConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lazyorm)
install(TARGETS lazyorm
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
