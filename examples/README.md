# [MYYAML](https://djoezeke.github.io/myyaml) [ EXAMPLES ]

This folder contains code examples for the C and C++ APIs.

> [!NOTE]
> Myyaml is still in development.

The purpose of Examples is to showcase integration, let you try Myyaml, and guide you toward
integrating Myyaml in your own application.

## Getting Started

Integration in a typical existing application, should take no time.

```cpp
#include <myyaml/myyaml>
```

Main resource:

- Read **[Documentation](https://djoezeke.github.io/myyaml/)** for detailed on how to integrate Myyaml.

Additional resources:

<!-- ## Building the Examples

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

Examples showing myjson functionality, like standalone usage or examples integrating external libraries.

| Example                  | Version created | Version updated | Original Developer                                  |
| ------------------------ | :-------------: | :-------------: | :-------------------------------------------------- |
| [loading](api/loading.c) |      0.1.0      |      0.1.0      | [Sackey Ezekiel Etrue](https://github.com/djoezeke) | -->

Some example missing? As always, contributions are welcome, feel free to send new examples!

<!-- Here is an [examples template](template.c) with instructions to start with! -->

## License

Myyaml is licensed under the MIT License, see [LICENSE](../LICENSE) for more information.
