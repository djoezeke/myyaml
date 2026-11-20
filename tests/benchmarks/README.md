# Benchmarks

The benchmark suite uses [Catch2](https://github.com/catchorg/Catch2)'s
`BENCHMARK` macro. This keeps the microbenchmarks in the same framework as the
unit tests and gives statistical summaries (mean, confidence interval and
standard deviation) out of the box.

## Build

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DMYYAML_BUILD_BENCHMARKS=ON

cmake --build build --target myyaml-benchmarks
```

Catch2 is taken from `tests/vendor/Catch2` when the submodule is present. If it
is missing, CMake fetches it automatically. Benchmarks can be built without
enabling the unit tests.

## Run

```sh
# Catch2's default: 100 samples, 100000 bootstrap resamples
./build/benchmarks/myyaml-benchmarks

# Faster iteration while developing
./build/benchmarks/myyaml-benchmarks \
  --benchmark-samples 10 \
  --benchmark-resamples 1000 \
  --benchmark-warmup-time 20

# Select a group with Catch2 tags
./build/benchmarks/myyaml-benchmarks "[parser]"
./build/benchmarks/myyaml-benchmarks "[emitter]"
./build/benchmarks/myyaml-benchmarks "[roundtrip]"
./build/benchmarks/myyaml-benchmarks "[c-api]"

# Raw sample means without bootstrapping
./build/benchmarks/myyaml-benchmarks --benchmark-no-analysis
```

`ctest --test-dir build -L benchmark` runs the suite through CTest with a
smaller sample count so CI remains fast.

## Scenarios

The executable parses a small hand-written configuration, a flow document, an
anchor/alias document, a 1000-element sequence, a 256-key mapping and a
generated large nested service configuration.

| Group         | What is measured                                                  |
| ------------- | ----------------------------------------------------------------- |
| `[parser]`    | Scalar, small block, flow, anchors/aliases, long sequence, wide mapping, large config |
| `[emitter]`   | Scalar, block and flow output, sorted/indented output, large config |
| `[roundtrip]` | Parse + dump, mapping lookup, mapping insertion, sequence appends |
| `[c-api]`     | Parse and parse + dump through the C wrapper                      |

## Interpreting results

Catch2 reports the mean, lower/upper confidence bounds and standard deviation
for each benchmark. Compare results from the same machine, compiler and build
mode. Always benchmark a Release build: Debug builds include STL debug checks
and unoptimised library code, so they are not representative of production
performance.

Benchmarks use pre-generated inputs and reuse parsed documents for emitter
tests, so the reported numbers isolate parsing or emission rather than input
construction. The C API comparison includes document allocation and release,
matching how the wrapper is used by callers.

## Adding a benchmark

Add a `BENCHMARK` block inside a test case tagged `[benchmark]`:

```cpp
TEST_CASE("my scenario", "[benchmark][my-tag]")
{
    BENCHMARK("parse: my document")
    {
        return myyaml::yaml::parse(document);
    };
}
```

Return a value from the body so the optimiser cannot remove the measured work.
Keep benchmark bodies repeatable and side-effect free; Catch2 runs them many
times during estimation and measurement.
