# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Basic Example Configures   
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

# add a new target which is a MYYAML example
# example: myyaml_add_example(main-examples main.example.cpp "")
function(myyaml_add_example target SOURCES DEPENDS)

    # set a source group for the source files
    source_group("" FILES ${SOURCES})

    # create the target
    add_executable(${target} ${SOURCES})

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Examples")

    # set the Visual Studio startup path for debugging
    set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # link the target to its MYYAML dependencies
    target_link_libraries(${target} PRIVATE ${DEPENDS} Myyaml::Myyaml)

    # Enable support for UTF-8 characters in source code
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()
endfunction()

#--------------------------------------------------------------------
# Build Api Example Projects
#--------------------------------------------------------------------
add_subdirectory(apis)

#--------------------------------------------------------------------
# Build Usage Tutorial Projects
#--------------------------------------------------------------------
add_subdirectory(tutorials)

