# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Basic Test Configures   
#--------------------------------------------------------------------

# Check that all required submodules are there
set(
  GIT_TEST_SUBMODULES
  Catch2
)

# Try to use vendor submodule if present otherwise fall back to FetchContent to obtain Catch2
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/CMakeLists.txt")
  	add_subdirectory(vendor/Catch2)
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
endif()

# Build Catch2 in C++17 mode to enable C++17 features
if(TARGET Catch2)
  target_compile_features(Catch2 PRIVATE cxx_std_17)
endif()

# Add the Catch2 CMake module path so CTest integration works
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Catch2/Cmake")

# Include Catch2 helper functions
include(Catch)

# Ensure that Catch2 sources and headers are not analyzed by any tools
set_target_properties(Catch2 PROPERTIES COMPILE_OPTIONS "" EXPORT_COMPILE_COMMANDS OFF UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 256)
set_target_properties(Catch2WithMain PROPERTIES EXPORT_COMPILE_COMMANDS OFF)
# set_target_properties(Catch2 Catch2WithMain PROPERTIES FOLDER "Dependencies")
get_target_property(CATCH2_INCLUDE_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(Catch2 SYSTEM INTERFACE ${CATCH2_INCLUDE_DIRS})

#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

# add a new target which is a MYTOML test
# example: myyaml_add_test(main-tests main.test.cpp ... Myyaml::Myyaml )
function(myyaml_add_test target SOURCES DEPENDS)

    # set a source group for the source files
    source_group("" FILES ${SOURCES})

    # create the target
    add_executable(${target} ${SOURCES})

    target_include_directories(${target} PRIVATE ${MYTOML_INCLUDE_BUILD_DIR})

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Tests")

    set_target_properties(${target} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # set the Visual Studio startup path for debugging
        VS_DEBUGGER_COMMAND_ARGUMENTS "-b" # Break into debugger

        XCODE_GENERATE_SCHEME ON # Required to set arguments
        XCODE_SCHEME_ARGUMENTS "-b" # Break into debugger
    )

    # link the target to its dependencies
    target_link_libraries(${target} PRIVATE ${DEPENDS} Catch2::Catch2WithMain)

    # If coverage is enabled for MSVC and we are linking statically, use /WHOLEARCHIVE
    # to make sure the linker doesn't discard unused code sections before coverage can be measured
    if(MYTOML_ENABLE_COVERAGE AND MSVC AND NOT BUILD_SHARED_LIBS)
        foreach(DEPENDENCY ${DEPENDS})
            target_link_options(${target} PRIVATE $<$<CONFIG:DEBUG>:/WHOLEARCHIVE:$<TARGET_LINKER_FILE:${DEPENDENCY}>>)
        endforeach()
    endif()

    # Delay test registration when cross compiling to avoid running crosscompiled app on host OS
    if(CMAKE_CROSSCOMPILING)
        set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
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

file(GLOB C_TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/c/*.test.cpp")
myyaml_add_test(myyaml-c-tests "${C_TEST_SRC}" "Myyaml::MyyamlC")

file(GLOB CPP_TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/cxx/*.test.cpp")
myyaml_add_test(myyaml-cpp-tests "${CPP_TEST_SRC}" "Myyaml::Myyaml")
