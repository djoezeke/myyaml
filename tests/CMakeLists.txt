# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Basic Test Configures   
#--------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include_directories("." "../")

set(MYYAML_TEST_LIB_NAME ${MYYAML_LIB_NAME}-d)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
add_library(${MYYAML_TEST_LIB_NAME} ../${MYYAML_SOURCES})

target_include_directories(${MYYAML_TEST_LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${MYYAML_INCLUDE_BUILD_DIR}>
)

set_target_properties(${MYYAML_TEST_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYYAML_BUILD_SHARED
    )

#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

function(add_c_test NAME)
  add_executable(${NAME} ${NAME}.c)
  target_link_libraries(${NAME} PRIVATE ${MYYAML_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY C_STANDARD 17)
  set_target_properties(${NAME} PROPERTIES DEFINE_SYMBOL MYYAML_IMPORT)
  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

function(add_cpp_test NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE ${MYYAML_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
  set_target_properties(${NAME} PROPERTIES DEFINE_SYMBOL MYYAML_IMPORT)
  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

#--------------------------------------------------------------------
# Recursively add Tests
#--------------------------------------------------------------------

# Automatically add all .c tests in this folder
file(GLOB C_TEST_SOURCES "*.c")
foreach(TEST_FILE ${C_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_c_test(${TEST_NAME})
endforeach()

# Automatically add all .cpp tests in this folder
file(GLOB CPP_TEST_SOURCES "*.cpp")
foreach(TEST_FILE ${CPP_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_cpp_test(${TEST_NAME})
endforeach()
