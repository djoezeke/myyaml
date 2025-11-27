# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

# Generate doxygen documentation.

if(NOT DEFINED MYYAML_USE_DOXYGEN)
    set(MYYAML_USE_DOXYGEN ON)
endif()

if(MYYAML_USE_DOXYGEN)
    find_package(Doxygen QUIET)

    if(NOT DOXYGEN_FOUND)
        message(STATUS "No Doxygen found. Doxygen documentation won't be built")
    else()
        file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR}/../include/*)
        set(README_DOC ${CMAKE_CURRENT_LIST_DIR}/../README.md)
        list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/../src/myyaml.c)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/doxygen/Doxyfile)

        message(STATUS "Wrote ${CMAKE_CURRENT_BINARY_DIR}/doxygen/Doxyfile")
    
        add_custom_command(OUTPUT html
                COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen/Doxyfile
                COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/html
                DEPENDS ${README_DOC} ${SOURCES} ${DOXYFILES}
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../
                )

        add_custom_target(doc ALL DEPENDS html)
    
        # TODO: Add documentation install 

    endif()
endif()

if(MYYAML_USE_MKDOCS)
    find_program(PYTHON_EXECUTABLE NAMES py python python3)
    if(PYTHON_EXECUTABLE)
        add_custom_target(mkdocs ALL
            COMMAND ${PYTHON_EXECUTABLE} -m mkdocs build -f ${CMAKE_CURRENT_SOURCE_DIR}/mkdocs.yml
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mkdocs
            COMMENT "Build MkDocs documentation"
        )
    else()
        message(WARNING "MkDocs requested but python not found on PATH")
    endif()
endif()
