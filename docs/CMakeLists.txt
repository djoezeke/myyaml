# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

# Generate doxygen documentation.

find_package(Doxygen)

IF(NOT DOXYGEN_FOUND)
    MESSAGE(STATUS "No Doxygen found. Documentation won't be built")
ELSE()
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
    
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ${DOC_INSTALL_DIR}
        COMPONENT doc)
ENDIF()
