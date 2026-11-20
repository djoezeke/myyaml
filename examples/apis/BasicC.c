/**
 * @file BasicC.c
 * @brief Parse, inspect, build and emit YAML with the C API.
 */

#include <myyaml/myyaml.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
    myyaml_document *document = myyaml_parse(
        "name: myyaml\n"
        "enabled: true\n"
        "items: [one, two, three]\n");
    if (document == NULL)
    {
        fprintf(stderr, "parse failed: %s\n", myyaml_last_error());
        return 1;
    }

    const myyaml_node *root = myyaml_const_root(document);
    const myyaml_node *name = myyaml_node_get(root, "name");
    const myyaml_node *items = myyaml_node_get(root, "items");
    printf("name: %s\n", myyaml_node_get_string(name));
    printf("items: %zu\n", myyaml_node_size(items));

    /* Build a new document from scratch. */
    myyaml_document *created = myyaml_create();
    if (created == NULL)
    {
        myyaml_free(document);
        fprintf(stderr, "create failed: %s\n", myyaml_last_error());
        return 1;
    }

    myyaml_node *created_root = myyaml_root(created);

    /* Mutating a mapping can invalidate its child handles, so assign each
     * value before inserting the next key. */
    myyaml_node *message = myyaml_node_set_key(created_root, "message");
    if (message == NULL)
    {
        fprintf(stderr, "build failed: %s\n", myyaml_last_error());
        myyaml_free(created);
        myyaml_free(document);
        return 1;
    }
    myyaml_node_set_string(message, "hello from C");

    myyaml_node *count = myyaml_node_set_key(created_root, "count");
    if (count == NULL)
    {
        fprintf(stderr, "build failed: %s\n", myyaml_last_error());
        myyaml_free(created);
        myyaml_free(document);
        return 1;
    }
    myyaml_node_set_integer(count, 3);

    /* Emit with deterministic, sorted keys. */
    myyaml_dump_options options;
    memset(&options, 0, sizeof(options));
    options.sort_keys = 1;
    options.trailing_newline = 1;

    char *text = myyaml_dump_with_options(created, &options);
    if (text == NULL)
    {
        fprintf(stderr, "dump failed: %s\n", myyaml_last_error());
        myyaml_free(created);
        myyaml_free(document);
        return 1;
    }

    printf("\n--- created YAML ---\n%s", text);

    myyaml_free_string(text);
    myyaml_free(created);
    myyaml_free(document);
    return 0;
}
