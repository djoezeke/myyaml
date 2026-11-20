# C API

The C API is a thin, exception-free wrapper around the C++ implementation. It
uses opaque handles and reports failures through status codes and a
thread-local error state.

Include the header and link `Myyaml::MyyamlC`:

```c
#include <myyaml/myyaml.h>
```

```cmake
find_package(Myyaml REQUIRED)
target_link_libraries(my_app PRIVATE Myyaml::MyyamlC)
```

## Parse and read

```c
#include <myyaml/myyaml.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    myyaml_document *document = myyaml_parse(
        "name: demo\n"
        "enabled: true\n"
        "items:\n"
        "  - one\n"
        "  - 2\n");
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
    printf("first: %s\n", myyaml_node_get_string(myyaml_node_at(items, 0)));

    myyaml_free(document);
    return 0;
}
```

Use `myyaml_parse_n()` when the input is not NUL-terminated or may contain NUL
bytes, and `myyaml_parse_with_options()` to apply parser limits:

```c
myyaml_parse_options options;
memset(&options, 0, sizeof(options));
options.max_depth = 64;
options.max_input_size = 1024 * 1024;
options.max_nodes = 100000;

myyaml_document *document =
    myyaml_parse_with_options(text, text_length, &options);
```

Zero numeric fields select the library defaults. Boolean fields are
interpreted literally.

## Read typed values

Type queries and getters never throw:

```c
int enabled = 0;
if (myyaml_node_get_bool(myyaml_node_get(root, "enabled"), &enabled))
{
    printf("enabled: %d\n", enabled);
}

long long count = 0;
if (myyaml_node_get_integer(myyaml_node_get(root, "count"), &count))
{
    printf("count: %lld\n", count);
}

const char *text = myyaml_node_get_string(node);
size_t length = myyaml_node_string_length(node); /* keeps embedded NUL bytes */
```

A getter returns `0` on a type mismatch. `myyaml_last_status()` then remains
`MYYAML_STATUS_OK`, because a mismatch is not an operation failure.

## Build a document

```c
myyaml_document *document = myyaml_create();
myyaml_node *root = myyaml_root(document);

myyaml_node *name = myyaml_node_set_key(root, "name");
myyaml_node_set_string(name, "demo");

myyaml_node *ports = myyaml_node_set_key(root, "ports");
myyaml_node_set_integer(myyaml_node_append(ports), 8080);
myyaml_node_set_integer(myyaml_node_append(ports), 8081);

myyaml_node *features = myyaml_node_set_key(root, "features");
myyaml_node_set_string(myyaml_node_append(features), "parse");
myyaml_node_set_string(myyaml_node_append(features), "emit");
```

`myyaml_node_set_key()` converts a null node to a mapping and inserts the key
with a null value if it is missing. `myyaml_node_append()` converts a null node
to a sequence and appends a null element.

## Emit YAML

```c
myyaml_dump_options options;
memset(&options, 0, sizeof(options));
options.indent = 2;
options.sort_keys = 1;
options.trailing_newline = 1;

char *text = myyaml_dump_with_options(document, &options);
if (text == NULL)
{
    fprintf(stderr, "dump failed: %s\n", myyaml_last_error());
}
else
{
    puts(text);
    myyaml_free_string(text);
}
myyaml_free(document);
```

The caller owns the returned string and must release it with
`myyaml_free_string()`. The caller also owns documents created by
`myyaml_parse*()` and `myyaml_create()`; release them with `myyaml_free()`.

## Error handling

On failure, parse and build functions return `NULL` or `0` and store an error
message. The error state is thread-local.

```c
myyaml_document *document = myyaml_parse("a: [1, 2\n");
if (document == NULL)
{
    fprintf(stderr, "status: %d\n", (int)myyaml_last_status());
    fprintf(stderr, "%s\n", myyaml_last_error());
    fprintf(stderr, "at line %zu, column %zu, byte %zu\n",
            myyaml_last_error_line(),
            myyaml_last_error_column(),
            myyaml_last_error_index());
}
```

Status values are declared in `myyaml_status`. Parse failures use
`MYYAML_STATUS_PARSE_ERROR`; invalid arguments use
`MYYAML_STATUS_INVALID_ARGUMENT`; type mismatches during mutation use
`MYYAML_STATUS_TYPE_ERROR`; file failures use `MYYAML_STATUS_IO_ERROR`.

## Handle lifetime

Node handles point into the owning document. A handle remains valid until the
document is modified or released. Operations that grow or shrink a collection
can invalidate handles to its children, so obtain a fresh handle after a
mutation. `myyaml_free()` invalidates every handle belonging to the document.

`NULL` is accepted by accessors such as `myyaml_node_size()` and
`myyaml_node_type()` and is reported as a null node. A `NULL` document handle
must still not be passed to mutating functions such as `myyaml_node_set_key()`;
those functions report `MYYAML_STATUS_INVALID_ARGUMENT`.

A complete example is available in `examples/apis/BasicC.c`.
