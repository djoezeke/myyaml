# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Recursively Add Examples
#--------------------------------------------------------------------

# Automatically add all .c samples in this folder
file(GLOB C_SAMPLE_SOURCES "*.c")
foreach(EXAMPLE_FILE ${C_SAMPLE_SOURCES})
  get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)
  myyaml_add_example("${EXAMPLE_NAME}" "${EXAMPLE_FILE}" "")
endforeach()


# Automatically add all .cpp samples in this folder
file(GLOB CPP_SAMPLE_SOURCES "*.cpp")
foreach(EXAMPLE_FILE ${CPP_SAMPLE_SOURCES})
  get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)
  myyaml_add_example("${EXAMPLE_NAME}" "${EXAMPLE_FILE}" "")
endforeach()
