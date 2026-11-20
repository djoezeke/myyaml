# Myyaml

Myyaml is a small, dependency-free YAML 1.2 implementation for C++17 and C11.
It provides deterministic round trips, strict validation and parse errors with
source positions.

## Quick example

```cpp
#include <myyaml/myyaml.hpp>

#include <iostream>

int main()
{
    myyaml::yaml config = myyaml::yaml::parse(
        "name: demo\n"
        "enabled: true\n"
        "ports: [8080, 8081]\n");

    std::cout << config["name"].as_string() << '\n';

    myyaml::dump_options options;
    options.sort_keys = true;
    std::cout << config.dump(options);
}
```

## Documentation

- [Usage](usage.md) — parsing, reading, emitting, building documents and limits.
- [C API](c-api.md) — opaque handles, ownership, error state and examples.
- [Tests and benchmarks](benchmarks.md) — Catch2 test and benchmark targets.

## Project status

The implementation covers block and flow collections, quoted and block
scalars, comments, document markers, anchors/aliases and the YAML 1.2 core
schema. Multi-document streams, complex keys, recursive aliases and arbitrary
tag resolution are not supported. See
[REVIEW.md](https://github.com/djoezeke/myyaml/blob/main/REVIEW.md) for known
limitations and
[IMPROVE.md](https://github.com/djoezeke/myyaml/blob/main/IMPROVE.md) for
planned work.
