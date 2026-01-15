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
            example, example,
            include_directories: incdir,
            link_with: [myyaml_lib],
            install: false
        )
        
	endforeach

    #===========================
    # C Library API Examples   #
    #===========================

	myyamlc_examples = []

    foreach example : myyamlc_examples

		exe = executable(example + 'c',
            'apis/' + example,
            include_directories: incdir,
            link_with: [myyamlc_lib], 
            install: false
        )

	endforeach

endif

# ===================================================================
