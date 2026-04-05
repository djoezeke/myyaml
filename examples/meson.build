# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================


if myyaml_build_examples

    #===========================
    # C++ Library API Examples #
    #===========================

	myyaml_examples = []

    foreach example : myyaml_examples

		exe = executable(
                        example,
                        'apis/' + example,
                        include_directories: myyaml_include_dirs,
                        dependencies: [myyaml_dep],
            install: false
        )
        
	endforeach

    #===========================
    # C Library API Examples   #
    #===========================

	myyamlc_examples = []

    foreach example : myyamlc_examples

		exe = executable(
                        example + 'c',
            'apis/' + example,
                        include_directories: myyaml_include_dirs,
                        dependencies: [myyamlc_dep],
            install: false
        )

	endforeach

endif

# ===================================================================
