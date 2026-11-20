# [MYYAML](https://djoezeke.github.io/myyaml) [ TESTS ]

This folder contains the Catch2 unit tests for the C and C++ APIs.

> [!NOTE]
> Myyaml is still in development.

## Building the tests

Catch2 is vendored in `tests/vendor/Catch2`. If the submodule is not present,
CMake fetches Catch2 automatically with `FetchContent`.

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DMYYAML_BUILD_TESTS=ON
cmake --build build
```

From the repository root, run the suite with:

```sh
ctest --test-dir build --output-on-failure
```

The two test executables can also be invoked directly:

```sh
./build/tests/myyaml-cpp-tests
./build/tests/myyaml-c-tests
```

Catch2 filters select individual tests or tags:

```sh
./build/tests/myyaml-cpp-tests "[emit]"
./build/tests/myyaml-cpp-tests "block scalars"
./build/tests/myyaml-c-tests "[c][errors]"
```

## Layout

| Path             | Contents                                                |
| ---------------- | ------------------------------------------------------- |
| `cxx/Main.test.cpp` | C++ API tests: parsing, emitting, mutation, options, I/O, encoding and errors |
| `c/Main.test.cpp`   | C API tests: parsing, node access, mutation, ownership, limits and errors     |
| `vendor/Catch2`     | Catch2 submodule (unit tests and benchmarks)            |

The tests cover:

- Valid block and flow documents, scalar inference, quoted and block scalars,
  comments, directives, document markers, anchors/aliases and tags.
- Emitter options, round trips, tricky strings, invalid UTF-8 and stream
  failures.
- Invalid arguments, malformed YAML, duplicate keys, parser limits, aliases
  and non-throwing error reporting.
- C API handle ownership, invalid arguments, type mismatches, dump options and
  embedded NUL bytes.

Microbenchmarks live in [`../benchmarks`](../benchmarks) and use the same
Catch2 dependency.

Some tests missing? As always, contributions are welcome, feel free to add new
tests!

## License

Myyaml is licensed under the MIT License, see [LICENSE](../LICENSE) for more
information.
