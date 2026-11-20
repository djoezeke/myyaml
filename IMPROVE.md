# Suggested Improvements and Future Work

This roadmap describes improvements that would make myyaml more complete,
faster and easier to integrate. Items are grouped by theme and roughly ordered
by impact. Each item includes a concrete direction and, where useful, an
acceptance criterion.

## 1. YAML conformance

### 1.1 Run the official YAML test suite

Add the [YAML Test Suite](https://github.com/yaml/yaml-test-suite) as a test
fixture and run every applicable case in CI. Track pass/fail counts per
feature (block scalars, flow collections, directives, tags, anchors, Unicode)
so progress is measurable.

**Acceptance:** a machine-readable conformance report is generated for each
commit; regressions fail CI.

### 1.2 Multi-document streams

Add a stream API that yields one document at a time:

```cpp
myyaml::document_stream stream(text);
for (myyaml::yaml document : stream)
{
    // ...
}
```

Support `---` and `...` separators, `%YAML`/`%TAG` directives scoped to a
document, and lazy/incremental reading from files and streams. Also expose the
equivalent in the C API.

**Acceptance:** parse and emit multi-document files; explicit and implicit
document starts; correct directive scoping.

### 1.3 Directives and tag resolution

- Resolve `%TAG` handles instead of ignoring them.
- Reject or correctly implement `%YAML 1.1` semantics. Do not accept 1.1 and
  silently apply 1.2 rules.
- Add a configurable tag registry for application tags, including
  `!!timestamp`, `!!binary`, `!!set` and `!!omap`.
- Decide whether merge keys (`<<`) are supported; if so, implement them as a
  parser feature with cycle protection.

**Acceptance:** `%TAG !e! tag:example.com,2025:` can be used in keys and
values; unknown tags fail with a clear diagnostic unless a resolver is
registered.

### 1.4 Complex keys

Support mappings, sequences and aliases as mapping keys while keeping the
current string-key convenience API. At minimum, model keys as `yaml` values
internally and add a canonical key encoding for duplicate detection.

**Acceptance:** `? [a, b]\n: value\n` parses and round-trips, and duplicate
complex keys are detected.

### 1.5 Timestamps and scalar types

Introduce a timestamp value type (or a dedicated `timestamp` node kind) with
timezone-aware parsing and deterministic formatting. Consider `binary` and
other core scalar types if the YAML test suite or users require them.

**Acceptance:** `2025-01-02T03:04:05Z` becomes a timestamp value, not a
string, when the caller opts in.

## 2. Parser and diagnostics

### 2.1 Incremental/event parser

Add a pull parser or SAX-style callback API so large documents can be processed
without building the whole tree:

```cpp
myyaml::event_reader reader(input);
while (auto event = reader.next())
{
    // mapping_start, scalar, sequence_end, ...
}
```

This also enables streaming validation, conversion to other formats and
lower peak memory use.

**Acceptance:** a multi-megabyte document can be validated with memory
proportional to nesting depth rather than document size.

### 2.2 Better error diagnostics

- Report a source excerpt with a caret under the failing token.
- Keep error categories stable (syntax, encoding, duplicate key, unsupported
  feature, limit exceeded) in both C++ and C APIs.
- Point semantic errors at the duplicate key or alias definition rather than
  the end of the document.
- Add error recovery for tools such as formatters and linters.

**Acceptance:** duplicate keys report the line of the second key; `try_parse()`
fills a stable error category and range.

### 2.3 Streaming input and limits

- Add `max_raw_input_size` in addition to decoded-size limits.
- Support bounded streaming reads for files and sockets.
- Make parser limits account for raw bytes as well as decoded UTF-8.

**Acceptance:** a UTF-16 document exceeding the raw limit is rejected before
being fully decoded.

### 2.4 Encoding control

Allow the caller to force an input encoding (`utf8`, `utf16le`, ...) rather
than relying solely on BOM detection. Add UTF-16/UTF-32 output options if user
demand justifies the extra emitter complexity.

**Acceptance:** BOM-less UTF-16 input can be parsed with an explicit option.

## 3. Performance

### 3.1 Index mappings

Keep the ordered vector for iteration, but add an optional index (hash map from
key to entry or an order-statistic structure) for O(1) average lookup. The
index must be invalidated or updated consistently on mutation and copies.

**Acceptance:** lookup and duplicate detection on a 10,000-key mapping are
significantly faster in the existing 256-key benchmark and a new 10,000-key
benchmark, with no regression for small mappings.

### 3.2 Avoid emitter copies

`emit_block_mapping()` currently copies the whole mapping even when
`sort_keys` is false. Iterate the mapping by reference in the common case and
copy only when sorting is requested.

**Acceptance:** large-mapping emission allocates less memory and improves
throughput in `[emitter]` benchmarks.

### 3.3 Reduce parse-time allocations

- Reserve vectors based on simple heuristics or input size.
- Reuse scalar buffers and avoid repeated `std::string` temporaries.
- Consider `std::string_view` for keys/scalars during parsing with a final
  ownership pass.
- Investigate arena allocation for nodes (API-compatible via a custom
  allocator or an opt-in parser arena).

**Acceptance:** fewer allocations per node and higher `[parser]` throughput on
the large configuration benchmark.

### 3.4 Alias handling

Store anchors as shared immutable subtrees or introduce a node graph with
cycle detection. This would reduce alias-expansion memory and preserve alias
relationships on emission. At minimum, cap expansion with a configurable
budget and report the anchor name in errors.

**Acceptance:** an alias used N times does not duplicate the subtree in memory;
recursive aliases produce a clear cycle error.

### 3.5 Benchmark infrastructure

- Add regression thresholds and a benchmark history (for example a JSON
  artifact uploaded by CI).
- Add allocation-count benchmarks and a large-document memory profile.
- Compare against yaml-cpp, libyaml and rapidyaml on the same corpus.
- Add compiler and standard-library matrices (GCC, Clang, MSVC; libstdc++,
  libc++, MSVC STL).

**Acceptance:** CI reports throughput and peak memory per scenario; a threshold
regression fails or warns.

## 4. Emitter and formatting

### 4.1 Style-preserving round trips

A full CST (concrete syntax tree) or comment-attachment model would allow
formatters and config editors to preserve comments, blank lines and quoting
style. This is a larger project but a common requirement for YAML tooling.

**Acceptance:** parse + dump of a file with comments preserves comments and
their attachment.

### 4.2 More formatting controls

- Line width and folding for long scalars and flow collections.
- Configurable quoting strategy (plain where safe, single-quoted, double-
  quoted, literal/folded block scalars).
- Indentation as a string style (for example tabs) if needed by consumers.
- Newline style (LF/CRLF) and final-newline control.
- ASCII-only output with `\u`/`\U` escapes.

**Acceptance:** options are documented, validated at the API boundary and
covered by unit tests.

### 4.3 Preserve anchors, aliases and tags

If the data model gains node identity, emit anchors and aliases instead of
expanding them. Preserve resolvable tags when a tag registry is available.

**Acceptance:** a document containing an anchored subtree and two aliases
round-trips with one anchor and two aliases.

### 4.4 Emitter performance

Write directly into a caller-provided buffer or stream with a reusable output
adapter, avoid building intermediate strings per scalar, and add a
`dump_to(std::string&)` overload that appends or reuses capacity.

**Acceptance:** emitter benchmarks show reduced allocations and improved
throughput without changing output.

## 5. C++ API

- Add `std::string_view`-friendly parse options and zero-copy scalar access
  where safe.
- Add allocator-aware containers or an opt-in `pmr` mode.
- Consider `std::expected` (C++23) or a project-local `expected` for
  non-throwing APIs across parse, emit and type conversion.
- Distinguish emitter-option errors from value type errors with a dedicated
  exception or error enum.
- Add typed conversion helpers and a visitor API for exhaustive node handling.
- Provide a stable `node_view` type for read-only access without copying.
- Document and test the exact copy/move/noexcept guarantees.

## 6. C API

- Add a versioned options struct (`size_t struct_size`) or explicit flag fields
  so zero-initialization can mean "defaults" without making `trailing_newline`
  impossible to disable.
- Add stable node IDs or generation-counted handles, or explicitly document and
  test invalidation after every mutation.
- Add `myyaml_parse_file_with_options()` and `myyaml_dump_to_file()`.
- Add a non-throwing status/result API for all mutating operations.
- Expose encoding override and multi-document stream APIs.
- Add an ABI/version policy and compile-time checks for header/library
  mismatches.

## 7. Testing, fuzzing and quality

### 7.1 Fuzzing

Add libFuzzer/AFL++ targets for:

- parser input (bytes)
- emitter input (random `yaml` trees)
- C API sequences (parse, mutate, dump, free)
- encoding decoders (UTF-8/16/32)

Run fuzzers for a fixed time in CI and with corpus regression files.

**Acceptance:** no crashes, leaks or UBSan reports in a sustained fuzz run.

### 7.2 Sanitizers and static analysis

Enable AddressSanitizer, UndefinedBehaviorSanitizer, leak detection and
`_GLIBCXX_ASSERTIONS` in at least one CI job. Run clang-tidy and
clang-format checks on changed files.

### 7.3 Property-based tests

Add property tests for:

- parse(dump(value)) == value for generated values
- dump(parse(text)) is idempotent for valid text
- malformed input never crashes
- limits never produce a partially valid tree

Use Catch2 generators or a property-testing dependency.

### 7.4 Coverage

Publish line/branch coverage for the C++ and C APIs and set a minimum
threshold. Track coverage separately for the parser, emitter and C wrapper.

### 7.5 Cross-platform CI

Run on Linux, macOS and Windows with GCC, Clang and MSVC; include 32-bit and
big-endian builds where practical, plus shared/static library variants.

## 8. Build, packaging and documentation

- Add Meson and Bazel benchmark targets that mirror the CMake Catch2 target.
- Add a `make check` / CMake `check` convenience target for tests plus
  benchmarks.
- Add a pkg-config file test that compiles a downstream example.
- Add install/export tests for `find_package(Myyaml)` with both static and
  shared builds.
- Expand the MkDocs documentation with an API reference generated from the
  headers, a cookbook and migration notes.
- Add a changelog and a compatibility policy (YAML version, C++/C standards,
  ABI).
- Add continuous benchmarking to the documentation site with historical plots.

## 9. Suggested milestones

| Milestone | Scope |
| --------- | ----- |
| 0.3 | Emitter copy removal, C API option/ownership fixes, fuzzing and sanitizer CI, YAML test suite harness |
| 0.4 | Multi-document streams, `%TAG` resolution, timestamp type, mapping lookup index |
| 0.5 | Complex keys, event/SAX parser, style-preserving round trips |
| 1.0 | Full YAML 1.2 conformance target, stable C ABI, documented performance envelope and compatibility guarantees |
