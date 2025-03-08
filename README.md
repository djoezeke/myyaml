# MyYAML 📄

MyYAML is a simple C library for creating, manipulating, and printing YAML data structures. It supports scalar values, sequences, and mappings.

## Features ✨

- Create YAML scalar values
- Create YAML sequences
- Create YAML mappings
- Add items to sequences
- Add key-value pairs to mappings
- Print YAML values
- Pretty print YAML values to a string
- Read YAML files into `YamlValue` structures
- Write `YamlValue` structures to YAML files
- Free YAML values

## Files 📂

- `yaml.c`: Implementation of the YAML library

## Usage 🚀

### Creating and Printing YAML Values

To create and print YAML values, you can use the functions provided in `yaml.c`. Here is an example:

````c
#include "yaml.c"

int main()
{
    // Create a YAML mapping
    Yaml *root = yaml_create_mapping();
    yaml_mapping_add(root, "name", yaml_create_scalar("John Doe"));
    yaml_mapping_add(root, "age", yaml_create_scalar("30"));

    // Create a YAML sequence and add it to the mapping
    Yaml *hobbies = yaml_create_sequence();
    yaml_sequence_add(hobbies, yaml_create_scalar("reading"));
    yaml_sequence_add(hobbies, yaml_create_scalar("swimming"));
    yaml_sequence_add(hobbies, yaml_create_scalar("coding"));
    yaml_mapping_add(root, "hobbies", hobbies);

    // Print the YAML value
    yaml_print(root, 0);

    // Pretty print the YAML value
    yaml_pretty_print(root, 0);

    // Free the YAML value
    yaml_free(root);

    return 0;
}# MyYAML 📄

MyYAML is a simple C library for creating, manipulating, and printing YAML data structures. It supports scalar values, sequences, and mappings.

## Features ✨

- Create YAML scalar values
- Create YAML sequences
- Create YAML mappings
- Add items to sequences
- Add key-value pairs to mappings
- Print YAML values
- Free YAML values

## Files 📂

- `yaml.c`: Implementation of the YAML library
- `csv.cpp`: Implementation of a CSV parser
- `mycsv.hpp`: Header file for CSV and YAML related classes and functions
- `csvconfig.hpp`: Configuration header for CSV parser (currently empty)
- `yaml.cpp`: C++ implementation of the YAML library

## Usage 🚀

### Creating and Printing YAML Values

To create and print YAML values, you can use the functions provided in `yaml.c`. Here is an example:

```c
#include "yaml.c"

int main()
{
    // Create a YAML mapping
    YamlValue *root = yaml_create_mapping();
    yaml_mapping_add(root, "name", yaml_create_scalar("John Doe"));
    yaml_mapping_add(root, "age", yaml_create_scalar("30"));

    // Create a YAML sequence and add it to the mapping
    YamlValue *hobbies = yaml_create_sequence();
    yaml_sequence_add(hobbies, yaml_create_scalar("reading"));
    yaml_sequence_add(hobbies, yaml_create_scalar("swimming"));
    yaml_sequence_add(hobbies, yaml_create_scalar("coding"));
    yaml_mapping_add(root, "hobbies", hobbies);

    // Print the YAML value
    yaml_print(root, 0);

    // Free the YAML value
    yaml_free(root);

    return 0;
}
````

## License 📜

This project is licensed under the MIT License.
