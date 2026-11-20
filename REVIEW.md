# Review: Limitations and Known Issues

This document describes the current state of myyaml as of version 0.2.0. It
focuses on behavioural limitations and known issues that users should consider
before relying on the library for a particular YAML workload.

The library is intentionally small and does not aim to implement every feature
of the YAML 1.2 specification. Where the implementation deviates from the
specification or from its own documentation, that deviation is listed here.

## 1. YAML feature coverage

### 1.1 Only a single document per parse call

`yaml::parse()` and the C `myyaml_parse*()` functions parse exactly one YAML
document. A stream containing `---`-separated documents is rejected with
`parse_error`. Applications that need multi-document streams must split the
input themselves and call the parser once per document.

There is no event/SAX API, no incremental parser and no way to iterate over a
stream of documents without buffering each document.

### 1.2 Complex mapping keys are not supported

Mapping keys must be scalars. A key that is itself a mapping or a sequence is
not supported, even though YAML permits it. Empty keys are rejected as well.

### 1.3 Tags are limited to the YAML core scalar tags

Only `!!str`, `!!int`, `!!float`, `!!bool` and `!!null` are interpreted.
`!<tag:yaml.org,2002:...>` forms of those tags are accepted, but arbitrary
application tags, `%TAG` handles, `!!timestamp`, `!!binary`, `!!set`, `!!omap`
and custom tag resolution are not supported. Unknown tags produce a
`parse_error`.

`%YAML` directives are checked for version 1.1/1.2, but `%YAML 1.1` input is
still parsed with YAML 1.2 core-schema rules. YAML 1.1-only booleans such as
`yes`, `on` and `y` remain strings. `%TAG` directives are currently ignored
rather than resolved. The header comment that advertises `%TAG` directives
should therefore be read as "tolerated, not resolved".

### 1.4 Anchors and aliases are value-based copies

Aliases are resolved by making a deep copy of the anchored value. This means:

- Object identity is not preserved. Two aliases of the same anchor become
  independent values, and emission loses the anchor/alias relationship.
- Forward aliases (an alias before its anchor) are rejected.
- Recursive or mutually recursive aliases are rejected.
- Alias expansion increases both memory use and the node count. `max_nodes`
  mitigates alias bombs, but a large alias used many times can still create
  substantial temporary memory pressure.

### 1.5 Timestamps and other YAML scalar types

Timestamps remain strings. There is no timestamp value type in the API, and a
`!!timestamp` tag is not resolved. Other application-specific scalar types are
also left as strings or rejected, depending on the tag.

### 1.6 Dialect details and scalar inference

- Leading-zero decimal integers such as `0123` are treated as strings. The
  YAML 1.2 core schema's integer rule technically permits leading zeros; this
  implementation deliberately rejects them.
- Only `true` and `false` (case-insensitive) are booleans. `yes`, `no`, `on`,
  `off` and similar spellings are strings.
- Merge keys (`<<`) are not implemented. They are parsed as ordinary keys.
- Duplicate mapping keys are rejected by default. This is configurable with
  `parse_options::allow_duplicate_keys`, but accepting duplicates preserves
  both entries and lookup semantics then depend on insertion order.
- Empty input, whitespace-only input and comment-only input produce a null
  root value rather than an empty mapping.

## 2. Parser behaviour

### 2.1 Line-oriented block parsing

Block parsing is line-oriented. This keeps the implementation small, but it
means some valid YAML constructs are not accepted or are handled more strictly
than the specification requires. In particular:

- Tabs are rejected as indentation. Tabs inside scalar content are allowed.
- Quoted scalars and plain scalars have limited support for spanning multiple
  lines compared with a full YAML parser.
- Some indentation/compact-block combinations that a complete YAML parser
  accepts may be rejected.
- Tabs after a sequence indicator or mapping colon are accepted as separation
  only in the places explicitly implemented by the parser.

### 2.2 Input encoding

Encoding is detected from a UTF-8, UTF-16 or UTF-32 byte-order mark. Without a
BOM, input is assumed to be UTF-8. There is no public parse option to force an
encoding; the lower-level `detail::decode_to_utf8()` helper accepts an encoding
but is an internal API.

The emitter always writes UTF-8 and never writes a BOM. UTF-16 and UTF-32
output are not supported. NUL bytes in the input are rejected before parsing,
including inside quoted scalars.

### 2.3 Input size accounting

`max_input_size` is checked against the decoded UTF-8 buffer, not the raw byte
count. A UTF-16 or UTF-32 document can therefore have an on-disk size larger
than the configured limit before the limit is applied. Callers that need a hard
cap on raw bytes should check the size before calling the parser.

### 2.4 Parser limits are conservative

`max_depth` is enforced at internal recursion points and is not a direct count
of user-visible mapping/sequence nesting. In practice a limit may reject a
document earlier than the nominal nesting depth suggests. Applications that
need a precise bound should test with representative documents.

### 2.5 Error positions are approximate for semantic errors

Syntax errors generally report a useful line, column and byte offset.
Semantic failures such as duplicate keys, unsupported tags or alias errors can
report the end of the document or the start of the next token rather than the
exact duplicate key or alias definition. Diagnostics are human-readable but
there is no error code per failure reason beyond the exception type.

## 3. Emitter behaviour

### 3.1 Semantic, not textual, round trips

Emission does not preserve comments, blank lines, original quoting style,
anchors, aliases, tags or exact indentation. A parse/dump round trip preserves
the data model, not the original text. Comments and anchor relationships are
lost.

### 3.2 Formatting controls are intentionally limited

- Indentation is restricted to 1–8 spaces. Zero or values above 8 raise
  `type_error`.
- Key sorting is lexicographic and uses `std::stable_sort`.
- There is no line-width/wrapping control. Long flow or plain scalars are
  emitted on one line.
- `explicit_start` and `explicit_end` add document markers, but the parser is
  still single-document.
- Multi-line strings are emitted as literal block scalars (`|`) when possible,
  otherwise as double-quoted strings. Folded scalars (`>`) are parsed but not
  reproduced by the emitter.
- Unicode is emitted as UTF-8; there is no option for ASCII-only escaping or
  escaped non-printable Unicode.

### 3.3 Emission performance

The block emitter copies a mapping before iterating over it, even when
`sort_keys` is false. For large mappings this doubles the mapping's memory
traffic and allocator work. Sequence emission iterates the source directly.
Aliases were already expanded into copies during parsing, so emitting an
alias-heavy document can be significantly more expensive than the original
input suggests.

### 3.4 Invalid UTF-8 is rejected, not repaired

The emitter validates string data and throws `encoding_error` for invalid
UTF-8. Bad string data cannot be silently emitted. This is safer than writing
corrupt output, but callers that obtain strings from untrusted or legacy
sources must validate or sanitize them beforehand.

## 4. Data model and API limitations

### 4.1 Mapping representation and lookup cost

Mappings are stored as an ordered vector of key/value pairs. This gives
deterministic insertion-order iteration and stable simple semantics, but:

- Lookup, insertion and erase are O(n).
- Duplicate-key detection is O(n) per key.
- There is no hash index or tree index.
- Memory overhead per entry is larger than a compact hash map.

Mappings with thousands of keys may become a performance bottleneck. The
benchmarks include a 256-key mapping to make this cost visible.

### 4.2 No allocator or storage customization

Values own `std::string` objects and a heap allocation per node
(`std::unique_ptr<impl>`). There is no allocator parameter, arena, small-string
optimisation hook, or string interning. Zero-copy parsing is not supported;
scalars are copied into owned strings.

### 4.3 `operator[]` inserts missing keys

`yaml::operator[](std::string_view)` on a mapping inserts a missing key with a
null value, so a typo can silently modify a document. Use `find()`,
`contains()` or `at()` when that behaviour is not wanted. `at()` throws
`lookup_error` for missing keys.

### 4.4 Exception and error API

The C++ API uses exceptions (`parse_error`, `encoding_error`, `io_error`,
`type_error`, `lookup_error`). `try_parse()` covers parsing only; other
operations such as emission, file I/O and type conversion still throw. There is
no `expected`-style result type for the whole API.

`type_error` is also used for invalid emitter options and numeric formatting
failures, not only for value type mismatches. Callers should not rely on the
exception class to distinguish every failure category.

### 4.5 C API handle lifetime

C API node handles are pointers into the owning document. Any mutation that
grows, shrinks or reorders a container can invalidate handles to that
container's children. `myyaml_free()` invalidates every handle in the document.
This is documented but differs from APIs that guarantee stable handles.

The C API exposes no way to detach a node, no ownership-transfer operation and
no streaming/file emission. `myyaml_free_string()` must be used for strings
returned by `myyaml_dump*()`; mixing allocators is undefined.

### 4.6 C API option semantics

Numeric zero fields in `myyaml_parse_options` and `myyaml_dump_options` select
library defaults where documented. Boolean fields are literal: a
zero-initialized `myyaml_dump_options` disables `trailing_newline`, so callers
that want a trailing newline must set it explicitly. There is no way to
disable a default-true field while still using "zero means default" for the
same structure; a future options version/size field would remove this
ambiguity.

### 4.7 Concurrency

Separate documents can be used concurrently. Error state in the C API is
thread-local. A single document or node handle must not be mutated from
multiple threads without external synchronization. There is no shared-
immutable-document mode, so concurrent readers of the same document are only
safe if no writer exists.

## 5. Known bugs and surprising behaviour

The following items are known at the time of writing. They are covered by
tests where the current behaviour is intentional, or documented here where the
behaviour is likely to change.

1. `%TAG` directives are accepted but not resolved, despite the public header
   describing `%TAG` support.
2. `%YAML 1.1` is accepted but YAML 1.1 boolean spellings are not recognized.
3. Duplicate-key errors may report a line at or after the duplicate rather than
   the duplicate itself.
4. A zero-initialized C `myyaml_dump_options` omits the trailing newline.
5. Emission of a mapping copies the entire mapping before writing it, even
   when keys are not sorted.
6. Recursive aliases are rejected with a generic `parse_error`; there is no
   dedicated error type or cycle diagnostic.
7. `myyaml_node_get_double()` accepts integers and converts them to `double`,
   while `myyaml_node_get_integer()` is strict. This asymmetry is convenient
   but should be considered when validating input types.
8. `max_depth` may reject documents that appear to be shallower than the
   configured limit because it counts internal parse recursion.

## 6. Testing and project gaps

- Unit tests are extensive for the implemented subset but do not run the
  official YAML test suite or a differential fuzzer against another parser.
- The benchmark suite is not gated by performance thresholds, so regressions
  are visible only when someone runs and compares benchmarks.
- The repository does not currently run AddressSanitizer/UBSan or fuzzing in
  CI by default, although CMake has options for sanitizers.
- The Meson and Bazel builds cover the library and tests but do not yet have
  benchmark targets equivalent to the CMake suite.
- Documentation and examples are intentionally small; API reference coverage
  is incomplete for some advanced helpers in the `detail` namespace.

## 7. Security considerations

- Alias expansion is value-based and can duplicate large structures. Keep
  `max_nodes` and `max_input_size` low for untrusted input, and consider
  `max_depth` as a secondary protection.
- The parser buffers the whole document in memory. It is not suitable for
  unbounded streams without an external size limit.
- Invalid encodings and invalid UTF-8 are rejected, but callers must still
  treat parsed values as untrusted data when using them for paths, shell
  commands, SQL or other sensitive contexts.
- The C API returns borrowed pointers from documents. Use-after-free is
  possible if a handle outlives its document; this is inherent to the opaque
  handle design and cannot be detected by the library.
