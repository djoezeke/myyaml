# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if myyaml_build_tests

    #===========================
    # C++ Library Tests        #
    #===========================

	myyaml_test_sources = [
		'cxx/Version.test.cpp'
	]

	myyaml_test_exe = executable(
		myyaml_test, 
		myyaml_test_sources,
		include_directories: myyaml_include_dirs,
		link_with: [myyaml_lib], 
		install: false
	)

	test(myyaml_test, myyaml_test_exe)

    #===========================
    # C Library Tests          #
    #===========================

	myyamlc_test_sources = [
		'c/Version.test.cpp'
	]

	myyamlc_test_exe = executable(
		myyamlc_test, 
		myyamlc_test_sources,
		include_directories: myyaml_include_dirs,
		link_with: [myyamlc_lib], 
		install: false
	)

	test(myyamlc_test, myyamlc_test_exe)

endif

# ===================================================================
