# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# myyaml Catch2 benchmarks
#--------------------------------------------------------------------

add_executable(myyaml-benchmarks Main.cpp)
target_link_libraries(myyaml-benchmarks PRIVATE
    Myyaml::Myyaml
    Catch2::Catch2WithMain
)
set_target_properties(myyaml-benchmarks PROPERTIES
    FOLDER "Benchmarks"
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(TARGET Myyaml::MyyamlC)
    target_link_libraries(myyaml-benchmarks PRIVATE Myyaml::MyyamlC)
    target_compile_definitions(myyaml-benchmarks PRIVATE MYYAML_BENCHMARKS_HAVE_C_API=1)
endif()

if(MSVC)
    target_compile_options(myyaml-benchmarks PRIVATE /utf-8)
endif()

# Support running the full benchmark suite through CTest. The sample count is
# deliberately small so that CI stays fast; run the executable directly with
# default settings (100 samples) for publication-quality measurements.
add_test(NAME myyaml-benchmarks
         COMMAND myyaml-benchmarks
                 --benchmark-samples 10
                 --benchmark-resamples 1000
                 --benchmark-warmup-time 20
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(myyaml-benchmarks PROPERTIES
    LABELS "benchmark"
    TIMEOUT 600
)
