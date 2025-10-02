#include "../myyaml.h"

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
}
