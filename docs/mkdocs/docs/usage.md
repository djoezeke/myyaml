# Using myyaml

This page shows the most common ways to parse, inspect, modify and emit YAML
with the C++ API. The C API is covered in [C API](c-api.md).

## Include and link

```cpp
#include <myyaml/myyaml.hpp>
```

With CMake, link the `Myyaml::Myyaml` target:

```cmake
find_package(Myyaml REQUIRED)
target_link_libraries(my_app PRIVATE Myyaml::Myyaml)
```

The C++ implementation requires C++17. The C API is a separate target,
`Myyaml::MyyamlC`, and requires C11 or newer.

## Parse a document

`myyaml::yaml::parse()` accepts a `std::string_view` and returns a value. Parse
errors throw `myyaml::parse_error`, which includes the line, column and byte
offset of the failure.

```cpp
#include <myyaml/myyaml.hpp>

#include <iostream>

int main()
{
    const char *text =
        "name: demo\n"
        "enabled: true\n"
        "retries: 3\n"
        "ratio: 0.75\n"
        "features: [parse, emit]\n"
        "servers:\n"
        "  - host: primary\n"
        "    port: 8080\n"
        "  - host: secondary\n"
        "    port: 8081\n";

    myyaml::yaml config = myyaml::yaml::parse(text);
    std::cout << config["name"].as_string() << '\n';
}
```

The parser applies the YAML 1.2 core schema to plain scalars, so `true`, `42`,
`0x2A`, `3.5` and `null` become the corresponding boolean, integer, floating
point and null values. Quoted scalars are always strings.

## Read values safely

Throwing accessors such as `as_string()` and `as_integer()` raise
`myyaml::type_error` when the node has another type. Non-throwing accessors
return `std::optional` and are convenient when the input is untrusted.

```cpp
const myyaml::yaml retries = config["retries"];
if (auto value = retries.get_integer())
{
    std::cout << "retries: " << *value << '\n';
}

if (config["enabled"].get_boolean().value_or(false))
{
    std::cout << "enabled\n";
}
```

Use `contains()` or `find()` before indexing a mapping when the key may be
missing:

```cpp
if (config.contains("servers"))
{
    const myyaml::yaml &servers = config["servers"];
    for (std::size_t index = 0; index < servers.size(); ++index)
    {
        std::cout << servers[index]["host"].as_string() << '\n';
    }
}
```

`yaml::operator[](std::string_view)` inserts a missing key with a null value,
while `yaml::at()` throws `myyaml::lookup_error` for a missing key.

## Parse without exceptions

`yaml::try_parse()` returns a result object and never throws. This is useful in
libraries that cannot use exceptions or when reporting errors to a user.

```cpp
const myyaml::parse_result result = myyaml::yaml::try_parse("a: [1, 2\n");
if (result)
{
    const myyaml::yaml &document = *result.value;
    // ...
}
else
{
    std::cerr << result.error << " at line " << result.line
              << ", column " << result.column << '\n';
}
```

`try_parse()` also reports encoding errors such as invalid UTF-8 through
`result.error`.

## Parse files and streams

```cpp
myyaml::yaml from_file = myyaml::yaml::parse_file("config.yaml");

std::ifstream input("config.yaml");
myyaml::yaml from_stream = myyaml::yaml::parse(input);

std::FILE *file = std::fopen("config.yaml", "rb");
if (file != nullptr)
{
    myyaml::yaml from_c_stream = myyaml::yaml::parse(file);
    std::fclose(file);
}
```

Exceptions from I/O failures are `myyaml::io_error`; malformed YAML raises
`myyaml::parse_error`. The `std::istream` extraction operator is also
available:

```cpp
std::istringstream input("name: demo\n");
myyaml::yaml document;
input >> document;
```

## Emit YAML

Every value can be serialized with `dump()`:

```cpp
const std::string text = config.dump();
std::cout << text;
```

`myyaml::dump_options` controls formatting:

```cpp
myyaml::dump_options options;
options.indent = 4;              // 1..8, default 2
options.flow = false;            // true emits [a, b] / {k: v}
options.sort_keys = true;        // deterministic key order
options.explicit_start = true;   // emit "---"
options.explicit_end = true;     // emit "..."
options.trailing_newline = true; // default

std::cout << config.dump(options);
```

Emission can also target a stream:

```cpp
config.dump(std::cout, options);
std::cout << config; // default options
```

The emitter quotes strings when plain scalars would change their meaning, and
uses literal block scalars for multi-line text. Invalid UTF-8 is rejected with
`myyaml::encoding_error` rather than producing a corrupt document.

## Build and modify documents

Start from a null value or an empty collection and build the document
programmatically:

```cpp
myyaml::yaml document;
document["name"] = "demo";
document["enabled"] = true;
document["ports"].push_back(8080);
document["ports"].push_back(8081);
document["limits"]["cpu"] = 2;
document["limits"]["memory"] = "4Gi";

myyaml::yaml explicit_sequence =
    myyaml::yaml::make_sequence({1, "two", false});

myyaml::yaml explicit_mapping = myyaml::yaml::make_mapping({
    {"name", "demo"},
    {"values", explicit_sequence},
});

document.erase("enabled");
document.set("updated-by", "example");
```

Assigning to a null node converts it to a mapping (`operator[](key)`) or a
sequence (`push_back()`). Mappings preserve insertion order. Copies are deep:
modifying a copy never changes the original value.

## Limits and untrusted input

Parser limits protect applications that process untrusted YAML:

```cpp
myyaml::parse_options options;
options.max_depth = 64;                    // nesting depth
options.max_input_size = 1024 * 1024;      // 1 MiB
options.max_nodes = 100000;                // nodes including aliases
options.allow_duplicate_keys = false;      // default

myyaml::yaml document = myyaml::yaml::parse(untrusted_text, options);
```

Exceeding a limit raises `myyaml::parse_error`. See
[REVIEW.md](https://github.com/djoezeke/myyaml/blob/main/REVIEW.md) for the
current limitations of the parser and emitter.

## Complete example

`examples/apis/Basic.cpp` is a compilable version of the parse, read, modify
and emit workflow:

```sh
cmake -S . -B build -DMYYAML_BUILD_EXAMPLES=ON
cmake --build build --target Basic
./build/examples/apis/Basic
```

The C equivalent is in `examples/apis/BasicC.c`.
