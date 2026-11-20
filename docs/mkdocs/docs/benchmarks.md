# Tests and benchmarks

Both the unit tests and the microbenchmarks use
[Catch2](https://github.com/catchorg/Catch2). Catch2 is vendored as a
submodule under `tests/vendor/Catch2`; if it is missing, CMake fetches it with
`FetchContent`.

## Build

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DMYYAML_BUILD_TESTS=ON \
  -DMYYAML_BUILD_BENCHMARKS=ON
cmake --build build
```

## Run the unit tests

```sh
ctest --test-dir build --output-on-failure
```

The C++ and C suites are separate executables, so individual tests can also be
selected through Catch2:

```sh
./build/tests/myyaml-cpp-tests "block scalars"
./build/tests/myyaml-c-tests "[c][parse]"
```

## Run the benchmarks

```sh
./build/benchmarks/myyaml-benchmarks
```

Catch2 prints the mean, confidence interval and standard deviation for each
benchmark. The executable accepts the standard Catch2 benchmark options:

```sh
# Faster iteration while developing
./build/benchmarks/myyaml-benchmarks \
  --benchmark-samples 10 \
  --benchmark-resamples 1000 \
  --benchmark-warmup-time 20

# Run only parser, emitter, round-trip or C API benchmarks
./build/benchmarks/myyaml-benchmarks "[parser]"
./build/benchmarks/myyaml-benchmarks "[emitter]"
./build/benchmarks/myyaml-benchmarks "[roundtrip]"
./build/benchmarks/myyaml-benchmarks "[c-api]"

# Disable statistical analysis for the raw sample mean
./build/benchmarks/myyaml-benchmarks --benchmark-no-analysis
```

`ctest --test-dir build -L benchmark` runs the benchmark suite with a reduced
sample count for CI.

## Scenarios

The benchmarks use a small hand-written configuration, a flow-style document,
an anchor/alias document, a 1000-element sequence, a 256-key mapping and a
generated large nested service configuration.

| Group       | Scenarios                                                        |
| ----------- | ---------------------------------------------------------------- |
| `[parser]`  | scalar, small block, flow, anchors/aliases, long sequence, mapping, large config |
| `[emitter]` | scalar, block, flow, sorted/indented, large config block and flow  |
| `[roundtrip]` | parse + dump, lookup, mapping insert, sequence append          |
| `[c-api]`   | parse and parse + dump through the C wrapper                      |

The numbers are comparative rather than absolute. Run benchmarks in a Release
build on an otherwise idle machine, and compare results from the same machine
and compiler. Benchmarking a Debug build measures debug STL and assertion
overhead, not the library's typical performance.
