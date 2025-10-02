cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

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
# Sample Functions
#--------------------------------------------------------------------

function(add_c_sample NAME)
  add_executable(${NAME} ${NAME}.c)
  set_property(TARGET ${NAME} PROPERTY C_STANDARD 17)
  target_link_libraries(${NAME} ${MYYAML_EXAMPLE_LIB_NAME})
  target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
  set_target_properties(${NAME} PROPERTIES DEFINE_SYMBOL MYYAML_IMPORT)
endfunction()

function(add_cpp_sample NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
  target_link_libraries(${NAME} ${MYYAML_EXAMPLE_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
  set_target_properties(${NAME} PROPERTIES DEFINE_SYMBOL MYYAML_IMPORT)
endfunction()

#--------------------------------------------------------------------
# Recursively add Samples
#--------------------------------------------------------------------

# Automatically add all .c samples in this folder
file(GLOB C_SAMPLE_SOURCES "*.c")
foreach(SAMPLE_FILE ${C_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
  add_c_sample(${SAMPLE_NAME})
endforeach()


# Automatically add all .cpp samples in this folder
file(GLOB CPP_SAMPLE_SOURCES "*.cpp")
foreach(SAMPLE_FILE ${CPP_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
  add_cpp_sample(${SAMPLE_NAME})
endforeach()
