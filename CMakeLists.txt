# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

#--------------------------------------------------------------------
# Project and version
#--------------------------------------------------------------------

set(MYYAML_VERSION_MAJOR 0)
set(MYYAML_VERSION_MINOR 2)
set(MYYAML_VERSION_PATCH 0)
set(MYYAML_VERSION_STRING "0.2.0")

project(Myyaml
        VERSION ${MYYAML_VERSION_STRING}
        DESCRIPTION "YAML parser and emitter for C and C++"
        HOMEPAGE_URL "https://github.com/djoezeke/myyaml"
        LANGUAGES C CXX
)

# Check if this project is the main project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MYYAML_IS_TOP_LEVEL ON)
else()
    set(MYYAML_IS_TOP_LEVEL OFF)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

#--------------------------------------------------------------------
# Options
#--------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" OFF)
option(MYYAML_BUILD_SHARED "Build myyaml as a shared library" OFF)
option(MYYAML_BUILD_C_API "Build the C API wrapper (MyyamlC)" ON)
option(MYYAML_BUILD_TESTS "Build the unit test suite" OFF)
option(MYYAML_BUILD_BENCHMARKS "Build the benchmark suite" OFF)
option(MYYAML_BUILD_EXAMPLES "Build the example programs" OFF)
option(MYYAML_BUILD_DOCS "Generate API documentation with Doxygen" OFF)
option(MYYAML_ENABLE_WARNINGS "Enable compiler warnings for library targets"
       ${MYYAML_IS_TOP_LEVEL})
option(MYYAML_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(MYYAML_ENABLE_SANITIZERS "Enable address and undefined-behaviour sanitizers" OFF)
option(MYYAML_ENABLE_STDLIB_ASSERTIONS "Enable standard library assertions" OFF)
option(MYYAML_INSTALL "Generate installation rules" ${MYYAML_IS_TOP_LEVEL})
cmake_dependent_option(MYYAML_INSTALL_DOCS "Install documentation" ${MYYAML_IS_TOP_LEVEL}
                       "MYYAML_INSTALL" OFF)
option(MYYAML_INSTALL_CPACK "Create a binary archive with CPack" OFF)

# C++17 is required by the implementation. C17 is the newest C standard
# needed by the public C header (it is C11 compatible).
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MYYAML_BUILD_SHARED)
    set(BUILD_SHARED_LIBS ON)
endif()

if(MYYAML_BUILD_TESTS OR MYYAML_BUILD_BENCHMARKS)
    include(CTest)
    enable_testing()
endif()

#--------------------------------------------------------------------
# Catch2 (unit tests and microbenchmarks)
#--------------------------------------------------------------------

if(MYYAML_BUILD_TESTS OR MYYAML_BUILD_BENCHMARKS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/Catch2/CMakeLists.txt")
        add_subdirectory(tests/vendor/Catch2)
        set(MYYAML_CATCH2_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/Catch2/extras")
    else()
        include(FetchContent)
        message(STATUS "Catch2 submodule not found; fetching Catch2 via FetchContent")
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.7.0
            GIT_SHALLOW ON
        )
        FetchContent_MakeAvailable(Catch2)
        set(MYYAML_CATCH2_MODULE_DIR "${catch2_SOURCE_DIR}/extras")
    endif()

    list(APPEND CMAKE_MODULE_PATH "${MYYAML_CATCH2_MODULE_DIR}")
    include(Catch)
endif()

#--------------------------------------------------------------------
# Warning and sanitizer flags
#--------------------------------------------------------------------

set(MYYAML_WARNING_FLAGS "")
if(MYYAML_ENABLE_WARNINGS)
    if(MSVC)
        set(MYYAML_WARNING_FLAGS /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(MYYAML_WARNING_FLAGS -Wall -Wextra -Wpedantic -Wshadow)
    endif()
    if(MYYAML_WARNINGS_AS_ERRORS)
        if(MSVC)
            list(APPEND MYYAML_WARNING_FLAGS /WX)
        else()
            list(APPEND MYYAML_WARNING_FLAGS -Werror)
        endif()
    endif()
endif()

if(MYYAML_ENABLE_SANITIZERS)
    string(APPEND CMAKE_CXX_FLAGS " -fno-omit-frame-pointer -fsanitize=address,undefined")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=address,undefined")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fsanitize=address,undefined")
endif()

if(MYYAML_ENABLE_STDLIB_ASSERTIONS)
    add_compile_definitions(_GLIBCXX_ASSERTIONS=1)
endif()

# Enable UTF-8 source support for MSVC.
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#--------------------------------------------------------------------
# Library targets
#--------------------------------------------------------------------

add_library(myyaml src/myyaml.cpp)
add_library(Myyaml::Myyaml ALIAS myyaml)

target_compile_features(myyaml PUBLIC cxx_std_17)
target_include_directories(myyaml
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(myyaml PROPERTIES
    EXPORT_NAME Myyaml
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    FOLDER "Myyaml"
)
if(MYYAML_WARNING_FLAGS)
    target_compile_options(myyaml PRIVATE ${MYYAML_WARNING_FLAGS})
endif()

set(MYYAML_EXPORT_TARGETS myyaml)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(myyaml PRIVATE MYYAML_EXPORTS)
    target_compile_definitions(myyaml INTERFACE MYYAML_IMPORTS)
else()
    target_compile_definitions(myyaml PUBLIC MYYAML_BUILD_STATIC)
endif()

if(MYYAML_BUILD_C_API)
    add_library(myyamlc src/myyamlc.cpp)
    add_library(Myyaml::MyyamlC ALIAS myyamlc)

    target_compile_features(myyamlc PUBLIC cxx_std_17)
    target_link_libraries(myyamlc PUBLIC myyaml)
    target_include_directories(myyamlc
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    set_target_properties(myyamlc PROPERTIES
        EXPORT_NAME MyyamlC
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        FOLDER "Myyaml"
    )
    if(MYYAML_WARNING_FLAGS)
        target_compile_options(myyamlc PRIVATE ${MYYAML_WARNING_FLAGS})
    endif()

    if(BUILD_SHARED_LIBS)
        target_compile_definitions(myyamlc PRIVATE MYYAMLC_EXPORTS)
        target_compile_definitions(myyamlc INTERFACE MYYAMLC_IMPORTS)
    else()
        target_compile_definitions(myyamlc PUBLIC MYYAMLC_BUILD_STATIC)
    endif()

    list(APPEND MYYAML_EXPORT_TARGETS myyamlc)
endif()

#--------------------------------------------------------------------
# Subdirectories
#--------------------------------------------------------------------

if(MYYAML_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if(MYYAML_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MYYAML_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(MYYAML_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

#--------------------------------------------------------------------
# Installation and package configuration
#--------------------------------------------------------------------

if(MYYAML_INSTALL)
    install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS ${MYYAML_EXPORT_TARGETS}
            EXPORT MyyamlTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT MyyamlTargets
            FILE MyyamlTargets.cmake
            NAMESPACE Myyaml::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Myyaml)

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MyyamlConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/MyyamlConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Myyaml)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/MyyamlConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/MyyamlConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/MyyamlConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Myyaml)

    set(MYYAML_PC_LIBS "-lmyyaml")
    if(MYYAML_BUILD_C_API)
        set(MYYAML_PC_LIBS "-lmyyamlc -lmyyaml")
    endif()
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Myyaml.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Myyaml.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Myyaml.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

    install(FILES LICENSE README.md
            DESTINATION ${CMAKE_INSTALL_DOCDIR})

    if(MYYAML_INSTALL_CPACK)
        set(CPACK_PACKAGE_NAME "Myyaml")
        set(CPACK_PACKAGE_CONTACT "djoezeke")
        set(CPACK_PACKAGE_VENDOR "Myyaml Contributors")
        set(CPACK_PACKAGE_FILE_NAME "myyaml-${PROJECT_VERSION}")
        set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
        set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
        set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
        set(CPACK_GENERATOR "ZIP;TGZ")
        include(CPack)
    endif()
endif()

#--------------------------------------------------------------------
# Convenience targets
#--------------------------------------------------------------------

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if(NOT TARGET uninstall)
        configure_file(cmake/cmake_uninstall.cmake.in
                       ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
                       IMMEDIATE @ONLY)
        add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P
                    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    endif()
endif()
