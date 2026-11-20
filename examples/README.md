# [MYYAML](https://djoezeke.github.io/myyaml) [ EXAMPLES ]

This folder contains compilable examples for the C and C++ APIs.

> [!NOTE]
> Myyaml is still in development.

The purpose of the examples is to show typical parsing, mutation and emission
workflows and to provide a starting point for integrating Myyaml into an
application.

## Getting started

Include the C++ header:

```cpp
#include <myyaml/myyaml.hpp>
```

or the C header:

```c
#include <myyaml/myyaml.h>
```

## Build the examples

From the repository root:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DMYYAML_BUILD_EXAMPLES=ON
cmake --build build
```

The CMake target names are derived from the file names:

```sh
./build/examples/apis/Basic
./build/examples/apis/BasicC
```

## Available examples

| Example        | API | Demonstrates                                           |
| -------------- | --- | ------------------------------------------------------ |
| `apis/Basic.cpp`  | C++ | Parse, read, mutate, dump with block/flow style and error handling |
| `apis/BasicC.c`   | C   | Parse, read, build a document, dump with options and free resources |

More usage snippets are included in the
[documentation](../docs/mkdocs/docs/usage.md), including parsing from files and
streams, non-throwing parse results, parser limits and the C API.

Some examples missing? As always, contributions are welcome, feel free to send
new examples!

## License

Myyaml is licensed under the MIT License, see [LICENSE](../LICENSE) for more
information.
