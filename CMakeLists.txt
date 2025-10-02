cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

set (MYYAML_VERSION_MAJOR 0)
set (MYYAML_VERSION_MINOR 1)
set (MYYAML_VERSION_PATCH 1)
set (MYYAML_VERSION_STRING ${MYYAML_VERSION_MAJOR}.${MYYAML_VERSION_MINOR}.${MYYAML_VERSION_PATCH})

project(Myyaml
        VERSION ${MYYAML_VERSION_STRING} 
        DESCRIPTION "Yaml parser for C/C++"
        HOMEPAGE_URL "https://github.com/djoezeke/myyaml"
        LANGUAGES  C
)

#--------------------------------------------------------------------
# Preparations depending on the specified build options  
#--------------------------------------------------------------------

# Check if this project is the main project
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MYYAML_IS_TOP_LEVEL ON)
else()
  set(MYYAML_IS_TOP_LEVEL OFF)
endif()

#--------------------------------------------------------------------
# Configure variables for the library   
#--------------------------------------------------------------------
include(GNUInstallDirs)

set(MYYAML_LIB_NAME "myyaml" CACHE STRING "Base name of library output name")

set(MYYAML_SOURCES myyaml.c)
set(MYYAML_INCLUDES myyaml.h)
set(MYYAML_TARGET_NAME  ${MYYAML_LIB_NAME})

# Set required C/C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

option(MYYAML_BUILD_SHARED "Build shared library" OFF)
option(MYYAML_BUILD_EXAMPLES "Build the ${PROJECT_NAME} example applications" ${MYYAML_IS_TOP_LEVEL})
option(MYYAML_BUILD_TESTS "Build the ${PROJECT_NAME} test programs" ${MYYAML_IS_TOP_LEVEL})
option(MYYAML_INSTALL "Generate installation target" ${MYYAML_IS_TOP_LEVEL})

set(MYYAML_CMAKE_CONFIG_NAME "${PROJECT_NAME}Config")
set(MYYAML_CMAKE_TARGET_NAME "${PROJECT_NAME}Target")

set(MYYAML_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MYYAML_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(MYYAML_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "Install directory path for config files.")

set(MYYAML_CMAKE_CONFIG_TEMPLATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MYYAML_CMAKE_CONFIG_TEMPLATE "${MYYAML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYYAML_CMAKE_CONFIG_NAME}.cmake.in")
set(MYYAML_CMAKE_VERSION_CONFIG_TEMPLATE "${MYYAML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYYAML_CMAKE_CONFIG_NAME}Version.cmake.in")
set(MYYAML_CMAKE_PACKAGE_CONFIG_TEMPLATE "${MYYAML_CMAKE_CONFIG_TEMPLATE_DIR}/pkg-config.pc.in")

set(MYYAML_CMAKE_CONFIG_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set(MYYAML_CMAKE_CONFIG_FILE  ${MYYAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake)
set(MYYAML_CMAKE_TARGETS_FILE  ${MYYAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake)
set(MYYAML_CMAKE_PACKAGE_FILE  ${MYYAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}.pc)
set(MYYAML_CMAKE_VERSION_CONFIG_FILE  ${MYYAML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake)

#--------------------------------------------------------------------
# Build library targets
#--------------------------------------------------------------------

if(MYYAML_BUILD_SHARED)
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
endif()

add_library(${MYYAML_LIB_NAME} ${MYYAML_SOURCES})

target_include_directories(${MYYAML_LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${MYYAML_INCLUDE_BUILD_DIR}>
        $<INSTALL_INTERFACE:${MYYAML_INCLUDE_INSTALL_DIR}>
)

#--------------------------------------------------------------------
# Configurations
#--------------------------------------------------------------------

# Default to build type "Release" unless tests are being built
if(NOT CMAKE_BUILD_TYPE)
	if (NOT MYYAML_BUILD_TESTS)
		message(STATUS "No build type selected, default to Release")
		set(CMAKE_BUILD_TYPE "Release")
	else()
		message(STATUS "No build type selected but tests are being built, default to Debug")
		set(CMAKE_BUILD_TYPE "Debug")
	endif()
endif()

if(BUILD_SHARED_LIBS)
    set_target_properties(${MYYAML_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYYAML_BUILD_SHARED
    )
else()
    set_target_properties(${MYYAML_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYYAML_BUILD_STATIC
    )
endif()

# Output directories for a build tree
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

#--------------------------------------------------------------------
# Add Project subdirectories
#--------------------------------------------------------------------

# Build the example apps   
if(MYYAML_BUILD_EXAMPLES)
    add_subdirectory(samples)
endif()

# Build the test apps   
if(MYYAML_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)

    # Set the unit test app project as the Visual Studio startup project
    # if the target compiler is some version of Microsoft Visual C++ and
    # if this project is the main project.
    if(MSVC AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyYamlUnitTest)
    endif()
endif()

#--------------------------------------------------------------------
# Install a pkg-config file
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Configure package.
#--------------------------------------------------------------------
include(CMakePackageConfigHelpers)

configure_file(
    ${MYYAML_CMAKE_CONFIG_TEMPLATE}
    ${MYYAML_CMAKE_CONFIG_FILE}
    @ONLY
)

configure_file(
    ${MYYAML_CMAKE_VERSION_CONFIG_TEMPLATE}
    ${MYYAML_CMAKE_VERSION_CONFIG_FILE}
    @ONLY
)

configure_file(
    "${MYYAML_CMAKE_PACKAGE_CONFIG_TEMPLATE}"
    "${MYYAML_CMAKE_PACKAGE_FILE}"
)

#--------------------------------------------------------------------
# Install package.
#--------------------------------------------------------------------
if(MYYAML_INSTALL)
    install(
        DIRECTORY ${MYYAML_INCLUDE_BUILD_DIR}
        DESTINATION ${MYYAML_INCLUDE_INSTALL_DIR}
    )

    install(
        FILES ${MYYAML_CMAKE_CONFIG_FILE} ${MYYAML_CMAKE_VERSION_CONFIG_FILE}
        DESTINATION ${MYYAML_CONFIG_INSTALL_DIR}
    )

    export(
        TARGETS ${MYYAML_TARGET_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${MYYAML_CMAKE_TARGETS_FILE}
    )

    install(
        TARGETS ${MYYAML_TARGET_NAME}
        EXPORT ${MYYAML_CMAKE_TARGET_NAME}
    )

    install(
        EXPORT ${MYYAML_CMAKE_TARGET_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${MYYAML_CONFIG_INSTALL_DIR}
    )

    install(
        FILES "${MYYAML_CMAKE_PACKAGE_FILE}"
        DESTINATION ${MYYAML_CONFIG_INSTALL_DIR}
    )

endif()
