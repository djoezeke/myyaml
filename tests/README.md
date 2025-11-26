# [MYYAML](https://djoezeke.github.io/myyaml) [ TESTS ]

This folder contains code tests for the C and C++ APIs.

> [!NOTE]
> Myyaml is still in development.

<!-- ## Building the Tests

The examples assume you have already built the `myjson` library in `../src`.

### With Make

```bash
make # builds all api examples and tests.
make examples  # builds all examples.
make examples/loading # builds one examples (e.g `make examples/loading`
```

### With CMake

To build the examples with cmake, run the following commands from the root of the repository:

```bash
cd examples/c
mkdir build
cd build
cmake ..
cmake --build .
```

This will produce binaries for each example: one that links with a shared Myyaml library, and one that is linked with static Myyaml library. The binaries linked with the static library have the postfix **\_s**.

To run a single example, execute it from the `build` directory:

```bash
./loading
```

## API EXAMPLES COLLECTION [TOTAL: 1]

Tests showing myjson functionality, like standalone usage or examples integrating external libraries.

| Example                  | Version created | Version updated | Original Developer                                  |
| ------------------------ | :-------------: | :-------------: | :-------------------------------------------------- |
| [loading](api/loading.c) |      0.1.0      |      0.1.0      | [Sackey Ezekiel Etrue](https://github.com/djoezeke) | -->

Some tests missing? As always, contributions are welcome, feel free to add new tests!

## License

Myyaml is licensed under the MIT License, see [LICENSE](../LICENSE) for more information.
