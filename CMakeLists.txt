cmake_minimum_required(VERSION 3.5)

project(LazyOrm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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

    include/Transaction.h
)

add_library(LazyOrm STATIC
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

    src/Transaction.cpp
)

target_compile_definitions(LazyOrm PRIVATE LazyOrm_LIBRARY)

option(tests "build tests" OFF)
if(tests)
add_subdirectory(tests)
endif(tests)


include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LazyOrmConfig.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/LazyOrm
)

install(FILES ${LazyORM_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LazyOrmConfig.cmake
#              ${CMAKE_CURRENT_BINARY_DIR}/FooConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LazyOrm)
install(TARGETS LazyOrm
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LazyOrm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
