# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if myyaml_build_tests
	catch2_include_dirs = include_directories('vendor/Catch2/src')

	catch2_dep = dependency(
		'catch2-with-main',
        fallback: ['Catch2', 'catch2_with_main_dep'],
		required: true
	)

    #===========================
    # C++ Library Tests        #
    #===========================

	myyaml_test_sources = [
		'cxx/Main.test.cpp',
	]

	myyaml_test_exe = executable(
		'myyaml_cpp_tests', 
		myyaml_test_sources,
		include_directories: [myyaml_include_dirs, catch2_include_dirs],
		dependencies: [myyaml_dep, catch2_dep], 
		install: false
	)

	test('myyaml_cpp_tests', myyaml_test_exe)

    #===========================
    # C Library Tests          #
    #===========================

	myyamlc_test_sources = [
		'c/Main.test.cpp'
	]

	myyamlc_test_exe = executable(
		'myyaml_c_tests', 
		myyamlc_test_sources,
		include_directories: [myyaml_include_dirs, catch2_include_dirs],
		dependencies: [myyamlc_dep, catch2_dep], 
		install: false
	)

	test('myyaml_c_tests', myyamlc_test_exe)

endif

# ===================================================================
