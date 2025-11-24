# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

# TODO: Use FetchContent if Catch2 submodule is not found
# TODO: Set the target flags to use the appropriate C++ standard library
# TODO: Enable Code Coverage

#--------------------------------------------------------------------
# Basic Test Configures   
#--------------------------------------------------------------------

# Check that all required submodules are there
set(
  GIT_TEST_SUBMODULES
  Catch2
)

foreach (submodule IN ITEMS ${GIT_TEST_SUBMODULES})
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/${submodule}/.git")
  message( FATAL_ERROR "Required CMake submodule `${submodule}' not found. This \
  is most likely caused by an incomplete clone from the main repository. Please run:
  $ git submodule update --init --recursive
to clone all required submodules and run CMake again.")
  endif()
endforeach()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories("." "../")

# Add Catch2 as a subdirectory
add_subdirectory(vendor/Catch2)

# Build Catch2 in C++17 mode to enable C++17 features
target_compile_features(Catch2 PRIVATE cxx_std_17)

# Ensure that Catch2 sources and headers are not analyzed by any tools
set_target_properties(Catch2 PROPERTIES COMPILE_OPTIONS "" EXPORT_COMPILE_COMMANDS OFF UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 256)
set_target_properties(Catch2WithMain PROPERTIES EXPORT_COMPILE_COMMANDS OFF)
set_target_properties(Catch2 Catch2WithMain PROPERTIES FOLDER "Dependencies")
get_target_property(CATCH2_INCLUDE_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(Catch2 SYSTEM INTERFACE ${CATCH2_INCLUDE_DIRS})

# Add the Catch2 CMake module path so CTest integration works
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/Cmake")

# Include Catch2 helper functions
include(Catch)

set(C_TEST_SRC
    c/main.test.cpp
)

set(CPP_TEST_SRC
    cxx/main.test.cpp
)

#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

# add a new target which is a MYYAML test
# example: myyaml_add_test(main-tests main.test.cpp "")
function(myyaml_add_test target SOURCES DEPENDS)

    # set a source group for the source files
    source_group("" FILES ${SOURCES})

    # create the target
    add_executable(${target} ${SOURCES})

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Tests")

    set_target_properties(${target} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # set the Visual Studio startup path for debugging
        VS_DEBUGGER_COMMAND_ARGUMENTS "-b" # Break into debugger

        XCODE_GENERATE_SCHEME ON # Required to set arguments
        XCODE_SCHEME_ARGUMENTS "-b" # Break into debugger
    )

    # link the target to its MYYAML dependencies
    target_link_libraries(${target} PRIVATE ${DEPENDS} Myyaml::Myyaml Catch2::Catch2WithMain)

    # If coverage is enabled for MSVC and we are linking statically, use /WHOLEARCHIVE
    # to make sure the linker doesn't discard unused code sections before coverage can be measured
    if(MYYAML_ENABLE_COVERAGE AND MSVC AND NOT BUILD_SHARED_LIBS)
        foreach(DEPENDENCY ${DEPENDS})
            target_link_options(${target} PRIVATE $<$<CONFIG:DEBUG>:/WHOLEARCHIVE:$<TARGET_LINKER_FILE:${DEPENDENCY}>>)
        endforeach()
    endif()

    # Enable support for UTF-8 characters in source code
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()

    # Add the test
    catch_discover_tests(${target} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endfunction()

#--------------------------------------------------------------------
# Manually or Recursively add Tests
#--------------------------------------------------------------------

myyaml_add_test(myyaml-c-tests "${C_TEST_SRC}" "")
myyaml_add_test(myyaml-cpp-tests "${CPP_TEST_SRC}" "")

# # Automatically add all .c tests in this folder
# file(GLOB C_TEST_SOURCES "*.c")
# foreach(TEST_FILE ${C_TEST_SOURCES})
#   get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
#   myyaml_add_test("${TEST_NAME}" "${TEST_FILE}" "")
# endforeach()

# # Automatically add all .cpp tests in this folder
# file(GLOB CPP_TEST_SOURCES "*.cpp")
# foreach(TEST_FILE ${CPP_TEST_SOURCES})
#   get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
#   myyaml_add_test("${TEST_NAME}" "${TEST_FILE}" "")
# endforeach()

# TODO: Add Convenience for building and running tests in a single command
