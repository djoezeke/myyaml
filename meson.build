# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

project('myyaml',
	'cpp',
	version : '0.1.0',
	license : 'MIT',
	default_options : [
		'buildtype=release',
		'default_library=static',
		'cpp_std=c++17'
		]
)

compiler = meson.get_compiler('cpp')
message('target cpu_family: @0@'.format(host_machine.cpu_family()))
message('target cpu: @0@'.format(host_machine.cpu()))
message('target system: @0@'.format(host_machine.system()))
message('target endian: @0@'.format(host_machine.endian()))

#--------------------------------------------------------------------
# Configure Project Variables.   
#--------------------------------------------------------------------

license = files('LICENSE')

myyaml_is_subproject = meson.is_subproject()
myyaml_include_dirs = include_directories('include')
myyaml_build_tests = get_option('myyaml_build_tests') and not myyaml_is_subproject
myyaml_build_examples = get_option('myyaml_build_examples') and not myyaml_is_subproject

src_files = []

#--------------------------------------------------------------------
# Build Project Libraries.
#--------------------------------------------------------------------

#===========================
# Build C++ Library        #
#===========================

myyaml_lib = static_library(
	'myyaml',
	'src/myyaml.cpp',
	include_directories : myyaml_include_dirs,
	install : true,
)

#===========================
# Build C Library          #
#===========================

myyamlc_lib = static_library(
	'myyamlc', 
	'src/myyamlc.cpp',
	include_directories : myyaml_include_dirs,
	link_with : myyaml_lib,
	install : true,
)

#--------------------------------------------------------------------
# Add Project subdirectories.
#--------------------------------------------------------------------

if myyaml_build_tests
	subdir('tests')
endif

if myyaml_build_examples
	subdir('examples')
endif

#--------------------------------------------------------------------
# Install Project Packages.
#--------------------------------------------------------------------

if not meson.is_subproject()

	install_headers('include/myyaml/myyaml.h', subdir : 'myyaml')
	install_headers('include/myyaml/myyaml.hpp', subdir : 'myyaml')

	pkgc = import('pkgconfig')
	pkgc.generate(
		name : meson.project_name(),
		version: meson.project_version(),
		description: 'YAML for Modern C/C++',
		url: 'https://github.com/djoezeke/myyaml',
	)

endif

# ===================================================================
