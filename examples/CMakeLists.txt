# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Basic Sample Configures   
#--------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(MYYAML_EXAMPLE_LIB_NAME ${MYYAML_LIB_NAME}-e)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
add_library(${MYYAML_EXAMPLE_LIB_NAME} ../${MYYAML_SOURCES})

target_include_directories(${MYYAML_EXAMPLE_LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${MYYAML_INCLUDE_BUILD_DIR}>
)

set_target_properties(${MYYAML_EXAMPLE_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYYAML_BUILD_SHARED
    )

#--------------------------------------------------------------------
# Build Api Example Projects
#--------------------------------------------------------------------
add_subdirectory(apis)

#--------------------------------------------------------------------
# Build Usage Tutorial Projects
#--------------------------------------------------------------------
add_subdirectory(tutorials)

