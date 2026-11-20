/**
 * @file myyaml.hpp
 * @brief YAML for modern C++.
 * @details Public API for parsing, manipulating and emitting YAML documents.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.2.0
 * @see https://www.github.com/djoezeke/myyaml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Developed by Sackey Ezekiel Etrue and every direct or indirect contributor
 * to the GitHub project. See LICENSE for copyright and licensing details.
 *
 * MYYAML
 * - A small, dependency-free YAML 1.2 implementation for C++17 and C11.
 * - Human friendly output, deterministic round trips, strict validation and
 *   clear parse errors with source positions.
 *
 * SUPPORTED YAML
 * - Block mappings and sequences, nesting and compact forms.
 * - Flow mappings and sequences, including multi-line flow collections.
 * - Plain, single-quoted and double-quoted scalars.
 * - Literal (|) and folded (>) block scalars with chomping and indentation
 *   indicators.
 * - YAML 1.2 core-schema scalar inference: null, bool, int, float and string.
 * - Comments, document start/end markers and %YAML/%TAG directives.
 * - Local anchors and aliases (resolved by value), plus scalar tags.
 *
 * NOT SUPPORTED
 * - Multi-document streams through parse(); call parse() once per document.
 * - Complex keys (non-scalar mapping keys).
 * - Recursive aliases and arbitrary tag resolution. Tags are limited to
 *   !!str, !!int, !!float, !!bool and !!null.
 * - Timestamps are intentionally kept as strings until a timestamp value type
 *   is added to the API.
 */

#ifndef DJOEZEKE_MYYAML_HPP
#define DJOEZEKE_MYYAML_HPP

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

//-----------------------------------------------------------------------------
// [SECTION] Version
//-----------------------------------------------------------------------------

/**
 * @def MYYAML_VERSION_MAJOR
 * @brief Major version number of the library.
 */
#ifndef MYYAML_VERSION_MAJOR
#define MYYAML_VERSION_MAJOR 0
#endif

/**
 * @def MYYAML_VERSION_MINOR
 * @brief Minor version number of the library.
 */
#ifndef MYYAML_VERSION_MINOR
#define MYYAML_VERSION_MINOR 2
#endif

/**
 * @def MYYAML_VERSION_PATCH
 * @brief Patch version number of the library.
 */
#ifndef MYYAML_VERSION_PATCH
#define MYYAML_VERSION_PATCH 0
#endif

/**
 * @def MYYAML_VERSION
 * @brief Library version string in the form "X.Y.Z".
 */
#ifndef MYYAML_VERSION
#define MYYAML_VERSION "0.2.0"
#endif

#ifndef MYYAML_SKIP_VERSION_CHECK
#if defined(MYYAML_VERSION_EXPECTED_MAJOR) && defined(MYYAML_VERSION_EXPECTED_MINOR) && \
    defined(MYYAML_VERSION_EXPECTED_PATCH)
#if MYYAML_VERSION_MAJOR != MYYAML_VERSION_EXPECTED_MAJOR || \
    MYYAML_VERSION_MINOR != MYYAML_VERSION_EXPECTED_MINOR || \
    MYYAML_VERSION_PATCH != MYYAML_VERSION_EXPECTED_PATCH
#warning "Already included a different version of the library!"
#endif
#endif
#endif

//-----------------------------------------------------------------------------
// [SECTION] Export
//-----------------------------------------------------------------------------

#if defined(_WIN32)
#define MYYAML_API_EXPORT __declspec(dllexport)
#define MYYAML_API_IMPORT __declspec(dllimport)
#define MYYAML_NO_EXPORT
#else
#define MYYAML_API_EXPORT __attribute__((visibility("default")))
#define MYYAML_API_IMPORT __attribute__((visibility("default")))
#define MYYAML_NO_EXPORT __attribute__((visibility("hidden")))
#endif

#if defined(MYYAML_BUILD_STATIC)
#define MYYAML_API
#elif defined(MYYAML_BUILD_SHARED) || defined(MYYAML_EXPORTS)
#define MYYAML_API MYYAML_API_EXPORT
#elif defined(MYYAML_LOAD_SHARED) || defined(MYYAML_IMPORTS)
#define MYYAML_API MYYAML_API_IMPORT
#else
#define MYYAML_API
#endif

//-----------------------------------------------------------------------------
// [SECTION] Feature switches
//-----------------------------------------------------------------------------

/**
 * @def MYYAML_DISABLE_READER
 * @brief Define to 1 to exclude parsing APIs at compile time.
 */
#if defined(MYYAML_DISABLE_READER) && MYYAML_DISABLE_READER
#define MYYAML_READER_ENABLED 0
#else
#define MYYAML_READER_ENABLED 1
#endif

/**
 * @def MYYAML_DISABLE_WRITER
 * @brief Define to 1 to exclude emitting APIs at compile time.
 */
#if defined(MYYAML_DISABLE_WRITER) && MYYAML_DISABLE_WRITER
#define MYYAML_WRITER_ENABLED 0
#else
#define MYYAML_WRITER_ENABLED 1
#endif

//-----------------------------------------------------------------------------
// [SECTION] Includes
//-----------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <initializer_list>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace myyaml
{
    inline namespace v0_2_0
    {
        //---------------------------------------------------------------------
        // [SECTION] Forward declarations
        //---------------------------------------------------------------------

        class yaml;
        class exception;
        class parse_error;
        class encoding_error;
        class io_error;
        class type_error;
        class lookup_error;
        class version;

        struct parse_options;
        struct dump_options;
        struct parse_result;

        namespace detail
        {
            struct mark;
            struct utf8;
            struct utf16;
            struct utf32;
            class iadapter;
            class oadapter;
            class memory_iadapter;
            class file_iadapter;
            class stream_iadapter;
            class memory_oadapter;
            class file_oadapter;
            class stream_oadapter;
        } // namespace detail

        //---------------------------------------------------------------------
        // [SECTION] Enumerations
        //---------------------------------------------------------------------

        /**
         * @brief Text encoding used by an input document.
         */
        enum class encoding : std::uint8_t
        {
            unspecified, /**< Detect from a BOM or assume UTF-8. */
            utf8,        /**< UTF-8. */
            utf16,       /**< UTF-16 with native endianness. */
            utf16le,     /**< UTF-16 little endian. */
            utf16be,     /**< UTF-16 big endian. */
            utf32,       /**< UTF-32 with native endianness. */
            utf32le,     /**< UTF-32 little endian. */
            utf32be,     /**< UTF-32 big endian. */
        };

        /**
         * @brief Type of a YAML node.
         */
        enum class node_t : std::uint8_t
        {
            null,     /**< Null value. */
            boolean,  /**< Boolean value. */
            integer,  /**< Signed 64-bit integer. */
            floating, /**< Double precision floating-point value. */
            string,   /**< UTF-8 string. */
            sequence, /**< Ordered sequence. */
            mapping,  /**< Ordered mapping (insertion order is preserved). */
        };

        //---------------------------------------------------------------------
        // [SECTION] Parse and dump options
        //---------------------------------------------------------------------

        /**
         * @brief Options controlling parser behaviour.
         */
        struct parse_options
        {
            /** Maximum nesting depth. Prevents stack exhaustion. */
            std::size_t max_depth{128};

            /** Maximum accepted input size in bytes. */
            std::size_t max_input_size{16u * 1024u * 1024u};

            /** Maximum number of nodes (including alias expansion). */
            std::size_t max_nodes{1000000u};

            /** Allow duplicate mapping keys. When false a parse_error is thrown. */
            bool allow_duplicate_keys{false};
        };

        /**
         * @brief Options controlling emitter behaviour.
         */
        struct dump_options
        {
            /** Number of spaces used for each indentation level (1..8). */
            int indent{2};

            /** Emit every collection in flow style ([a, b] / {k: v}). */
            bool flow{false};

            /** Sort mapping keys lexicographically before emitting. */
            bool sort_keys{false};

            /** Emit an explicit document start marker (---). */
            bool explicit_start{false};

            /** Emit an explicit document end marker (...). */
            bool explicit_end{false};

            /** Append a trailing newline to the document. */
            bool trailing_newline{true};
        };

        //---------------------------------------------------------------------
        // [SECTION] Exceptions
        //---------------------------------------------------------------------

        /**
         * @brief Base class for all exceptions thrown by myyaml.
         */
        class MYYAML_API exception : public std::exception
        {
        public:
            exception() noexcept = default;
            explicit exception(std::string message) noexcept;
            ~exception() override = default;

            [[nodiscard]] const char *what() const noexcept override;

        private:
            std::string m_message;
        };

        /**
         * @brief Error raised when a document cannot be parsed.
         */
        class MYYAML_API parse_error : public exception
        {
        public:
            parse_error(std::string message, std::size_t line = 0, std::size_t column = 0,
                        std::size_t index = 0);
            ~parse_error() override = default;

            /** @brief 1-based line where the error was detected. */
            [[nodiscard]] std::size_t line() const noexcept { return m_line; }
            /** @brief 1-based column where the error was detected. */
            [[nodiscard]] std::size_t column() const noexcept { return m_column; }
            /** @brief 0-based byte offset where the error was detected. */
            [[nodiscard]] std::size_t index() const noexcept { return m_index; }

        private:
            static std::string build_message(std::string message, std::size_t line,
                                             std::size_t column);

            std::size_t m_line;
            std::size_t m_column;
            std::size_t m_index;
        };

        /**
         * @brief Error raised for invalid byte sequences or bad encodings.
         */
        class MYYAML_API encoding_error : public exception
        {
        public:
            explicit encoding_error(std::string message);
            ~encoding_error() override = default;
        };

        /**
         * @brief Error raised for input/output failures.
         */
        class MYYAML_API io_error : public exception
        {
        public:
            explicit io_error(std::string message);
            ~io_error() override = default;
        };

        /**
         * @brief Error raised when a value is accessed as the wrong type.
         */
        class MYYAML_API type_error : public exception
        {
        public:
            explicit type_error(std::string message);
            ~type_error() override = default;
        };

        /**
         * @brief Error raised when a mapping key or sequence index is missing.
         */
        class MYYAML_API lookup_error : public exception
        {
        public:
            explicit lookup_error(std::string message);
            ~lookup_error() override = default;
        };

        //---------------------------------------------------------------------
        // [SECTION] Version
        //---------------------------------------------------------------------

        /**
         * @brief Semantic version number.
         */
        class MYYAML_API version
        {
        public:
            version() = default;
            version(int major, int minor, int patch) noexcept;

            int major{0};
            int minor{0};
            int patch{0};
        };

        MYYAML_API bool operator<(const version &lhs, const version &rhs) noexcept;
        MYYAML_API bool operator>(const version &lhs, const version &rhs) noexcept;
        MYYAML_API bool operator==(const version &lhs, const version &rhs) noexcept;
        MYYAML_API bool operator!=(const version &lhs, const version &rhs) noexcept;
        MYYAML_API bool operator<=(const version &lhs, const version &rhs) noexcept;
        MYYAML_API bool operator>=(const version &lhs, const version &rhs) noexcept;
        MYYAML_API std::ostream &operator<<(std::ostream &stream, const version &value);

        /** @brief Return the library version as a version object. */
        MYYAML_API version library_version() noexcept;

        /** @brief Return the library version as a string ("X.Y.Z"). */
        MYYAML_API const char *library_version_string() noexcept;

        //---------------------------------------------------------------------
        // [SECTION] YAML document
        //---------------------------------------------------------------------

        /**
         * @brief A YAML value.
         *
         * A yaml object can hold a null, boolean, signed 64-bit integer,
         * double, UTF-8 string, sequence or mapping. Mappings preserve
         * insertion order. Copies are deep, so modifying a copy never
         * modifies the original document.
         *
         * Construction examples:
         * @code
         * myyaml::yaml scalar = "hello";
         * myyaml::yaml number = 42;
         * myyaml::yaml list = myyaml::yaml::sequence{1, "two", true};
         * myyaml::yaml object = myyaml::yaml::mapping{
         *     {"name", "demo"},
         *     {"enabled", true},
         * };
         * @endcode
         */
        class MYYAML_API yaml
        {
        public:
            /** @brief Ordered sequence container. */
            using sequence = std::vector<yaml>;

            /** @brief Ordered mapping container. */
            using mapping = std::vector<std::pair<std::string, yaml>>;

            //-----------------------------------------------------------------
            // Construction
            //-----------------------------------------------------------------

            yaml();
            yaml(std::nullptr_t) noexcept;
            yaml(bool value);
            yaml(int value);
            yaml(long value);
            yaml(long long value);
            yaml(unsigned int value);
            yaml(unsigned long value);
            yaml(unsigned long long value);
            yaml(float value);
            yaml(double value);
            yaml(const char *value);
            yaml(const std::string &value);
            yaml(std::string &&value);
            yaml(std::string_view value);
            yaml(sequence value);
            yaml(mapping value);

            yaml(const yaml &other);
            yaml(yaml &&other) noexcept;
            ~yaml();

            yaml &operator=(const yaml &other);
            yaml &operator=(yaml &&other) noexcept;

            /** @brief Create an empty sequence. */
            static yaml make_sequence();
            /** @brief Create a sequence from an initializer list. */
            static yaml make_sequence(std::initializer_list<yaml> values);
            /** @brief Create an empty mapping. */
            static yaml make_mapping();
            /** @brief Create a mapping from an initializer list. */
            static yaml make_mapping(std::initializer_list<std::pair<std::string, yaml>> values);

            /** @brief Swap two values in constant time. */
            void swap(yaml &other) noexcept;

            //-----------------------------------------------------------------
            // Type queries
            //-----------------------------------------------------------------

            [[nodiscard]] node_t type() const noexcept;
            [[nodiscard]] const char *type_name() const noexcept;
            [[nodiscard]] bool is_null() const noexcept;
            [[nodiscard]] bool is_boolean() const noexcept;
            [[nodiscard]] bool is_bool() const noexcept { return is_boolean(); }
            [[nodiscard]] bool is_integer() const noexcept;
            [[nodiscard]] bool is_int() const noexcept { return is_integer(); }
            [[nodiscard]] bool is_floating() const noexcept;
            [[nodiscard]] bool is_double() const noexcept { return is_floating(); }
            [[nodiscard]] bool is_number() const noexcept;
            [[nodiscard]] bool is_string() const noexcept;
            [[nodiscard]] bool is_sequence() const noexcept;
            [[nodiscard]] bool is_array() const noexcept { return is_sequence(); }
            [[nodiscard]] bool is_mapping() const noexcept;
            [[nodiscard]] bool is_object() const noexcept { return is_mapping(); }
            [[nodiscard]] bool is_scalar() const noexcept;

            //-----------------------------------------------------------------
            // Value access (throwing)
            //-----------------------------------------------------------------

            [[nodiscard]] bool as_boolean() const;
            [[nodiscard]] bool as_bool() const { return as_boolean(); }
            [[nodiscard]] std::int64_t as_integer() const;
            [[nodiscard]] std::int64_t as_int() const { return as_integer(); }
            [[nodiscard]] double as_floating() const;
            [[nodiscard]] double as_double() const { return as_floating(); }
            [[nodiscard]] const std::string &as_string() const;
            [[nodiscard]] std::string &as_string();
            [[nodiscard]] const char *as_c_str() const;
            [[nodiscard]] sequence &as_sequence();
            [[nodiscard]] const sequence &as_sequence() const;
            [[nodiscard]] sequence &as_array() { return as_sequence(); }
            [[nodiscard]] const sequence &as_array() const { return as_sequence(); }
            [[nodiscard]] mapping &as_mapping();
            [[nodiscard]] const mapping &as_mapping() const;
            [[nodiscard]] mapping &as_object() { return as_mapping(); }
            [[nodiscard]] const mapping &as_object() const { return as_mapping(); }

            //-----------------------------------------------------------------
            // Value access (non-throwing)
            //-----------------------------------------------------------------

            [[nodiscard]] std::optional<bool> get_boolean() const noexcept;
            [[nodiscard]] std::optional<std::int64_t> get_integer() const noexcept;
            [[nodiscard]] std::optional<double> get_floating() const noexcept;
            [[nodiscard]] std::optional<std::string> get_string() const;
            [[nodiscard]] std::optional<std::string_view> get_string_view() const noexcept;

            /** @brief Return the value converted to double, if numeric. */
            [[nodiscard]] std::optional<double> get_number() const noexcept;

            //-----------------------------------------------------------------
            // Conversion
            //-----------------------------------------------------------------

            /** @brief Return a string representation (scalars converted). */
            [[nodiscard]] std::string to_string() const;

            /**
             * @brief Serialize this value as a YAML document.
             * @param options Emitter options.
             */
            [[nodiscard]] std::string dump(const dump_options &options = {}) const;

            /**
             * @brief Serialize this value into an output stream.
             * @param stream Destination stream.
             * @param options Emitter options.
             */
            void dump(std::ostream &stream, const dump_options &options = {}) const;

            //-----------------------------------------------------------------
            // Mapping and sequence access
            //-----------------------------------------------------------------

            /**
             * @brief Access or insert a mapping value.
             *
             * A null value is converted to an empty mapping. A missing key is
             * inserted with a null value. Accessing a non-mapping value throws
             * type_error.
             */
            yaml &operator[](std::string_view key);

            /** @brief Access a mapping value; throws lookup_error if missing. */
            const yaml &operator[](std::string_view key) const;

            /**
             * @brief Access or insert a sequence element.
             *
             * A null value is converted to an empty sequence. Index must be
             * within the current size. Non-sequence values throw type_error.
             */
            yaml &operator[](std::size_t index);
            const yaml &operator[](std::size_t index) const;

            yaml &at(std::string_view key);
            const yaml &at(std::string_view key) const;
            yaml &at(std::size_t index);
            const yaml &at(std::size_t index) const;

            [[nodiscard]] yaml *find(std::string_view key) noexcept;
            [[nodiscard]] const yaml *find(std::string_view key) const noexcept;
            [[nodiscard]] bool contains(std::string_view key) const noexcept;

            /** @brief Append a value to a sequence. Converts null to sequence. */
            yaml &push_back(yaml value);

            /** @brief Construct a value in-place at the end of a sequence. */
            template <typename... Args>
            yaml &emplace_back(Args &&...args)
            {
                return push_back(yaml(std::forward<Args>(args)...));
            }

            /** @brief Erase a mapping key. Returns true when the key existed. */
            bool erase(std::string_view key);
            /** @brief Erase a sequence element. Returns true when the index existed. */
            bool erase(std::size_t index);

            /** @brief Remove all elements; the value becomes an empty collection. */
            void clear();

            /** @brief Number of elements in a collection. */
            [[nodiscard]] std::size_t size() const noexcept;
            /** @brief True when the value has no elements (null is empty). */
            [[nodiscard]] bool empty() const noexcept;

            /** @brief Mapping keys in insertion order. */
            [[nodiscard]] std::vector<std::string> keys() const;

            /** @brief Insert or assign a mapping key. */
            yaml &set(std::string_view key, yaml value);

            //-----------------------------------------------------------------
            // Parsing
            //-----------------------------------------------------------------

#if MYYAML_READER_ENABLED
            /**
             * @brief Parse a YAML document from memory.
             * @throws parse_error when the document is invalid.
             */
            static yaml parse(std::string_view text, const parse_options &options = {});

            /**
             * @brief Parse a YAML document from an input stream.
             * @throws io_error on stream failure and parse_error on invalid YAML.
             */
            static yaml parse(std::istream &stream, const parse_options &options = {});

            /**
             * @brief Parse a YAML document from a C FILE stream.
             * @throws io_error on stream failure and parse_error on invalid YAML.
             */
            static yaml parse(std::FILE *file, const parse_options &options = {});

            /**
             * @brief Parse a YAML document from a file path.
             * @throws io_error when the file cannot be opened.
             */
            static yaml parse_file(const std::string &path, const parse_options &options = {});

            /**
             * @brief Parse without throwing.
             *
             * On failure `error`, `line`, `column` and `index` describe the
             * problem and `value` is empty.
             */
            static parse_result try_parse(std::string_view text,
                                          const parse_options &options = {}) noexcept;
#endif

            //-----------------------------------------------------------------
            // Operators
            //-----------------------------------------------------------------

            friend bool operator==(const yaml &lhs, const yaml &rhs) noexcept;
            friend bool operator!=(const yaml &lhs, const yaml &rhs) noexcept;

            friend std::ostream &operator<<(std::ostream &stream, const yaml &value);

#if MYYAML_READER_ENABLED
            friend std::istream &operator>>(std::istream &stream, yaml &value);
#endif

        private:
            static yaml from_int64(std::int64_t value);
            struct impl;
            std::unique_ptr<impl> m_impl;
        };

        MYYAML_API bool operator==(const yaml &lhs, const yaml &rhs) noexcept;
        MYYAML_API bool operator!=(const yaml &lhs, const yaml &rhs) noexcept;

        MYYAML_API void swap(yaml &lhs, yaml &rhs) noexcept;

        MYYAML_API std::ostream &operator<<(std::ostream &stream, const yaml &value);

#if MYYAML_READER_ENABLED
        MYYAML_API std::istream &operator>>(std::istream &stream, yaml &value);
#endif

        /**
         * @brief Non-throwing parse result returned by yaml::try_parse().
         */
        struct parse_result
        {
            std::optional<yaml> value; /**< Parsed document when successful. */
            std::string error;         /**< Error message when parsing failed. */
            std::size_t line{0};       /**< Error line (1-based), 0 if unknown. */
            std::size_t column{0};     /**< Error column (1-based), 0 if unknown. */
            std::size_t index{0};      /**< Error byte offset (0-based). */

            /** @brief True when parsing succeeded. */
            explicit operator bool() const noexcept { return value.has_value(); }
        };

        //---------------------------------------------------------------------
        // [SECTION] Stringification
        //---------------------------------------------------------------------

        MYYAML_API const char *string(encoding value) noexcept;
        MYYAML_API const char *string(node_t value) noexcept;
        MYYAML_API std::ostream &operator<<(std::ostream &stream, const encoding &value);
        MYYAML_API std::ostream &operator<<(std::ostream &stream, const node_t &value);

        //---------------------------------------------------------------------
        // [SECTION] Detail helpers
        //---------------------------------------------------------------------

        namespace detail
        {
            /**
             * @brief A source position.
             */
            struct mark
            {
                std::size_t line{1};
                std::size_t column{1};
                std::size_t index{0};
            };

            /** @brief Detect an encoding from a byte-order mark. */
            MYYAML_API encoding determine_encoding(const void *data, std::size_t size) noexcept;

            /**
             * @brief Strict UTF-8 helpers.
             *
             * decode() rejects overlong encodings, surrogate code points,
             * invalid continuation bytes and values above U+10FFFF.
             * encode() rejects lone surrogates and out-of-range values.
             */
            struct MYYAML_API utf8
            {
                static int decode(const char *data, std::size_t size, unsigned int &value) noexcept;
                static int encode(unsigned int codepoint, char *output, std::size_t size) noexcept;
                static std::string from_codepoint(unsigned int codepoint);
                static bool validate(const std::string &text) noexcept;
            };

            /** @brief UTF-16 helpers (byte oriented, explicit endianness). */
            struct MYYAML_API utf16
            {
                static int decode(const char *data, std::size_t size, unsigned int &value,
                                  bool big_endian = false) noexcept;
                static int encode(unsigned int codepoint, char *output, std::size_t size,
                                  bool big_endian = false) noexcept;
                static std::string to_utf8(const std::vector<unsigned char> &bytes,
                                           bool big_endian = false);
            };

            /** @brief UTF-32 helpers (byte oriented, explicit endianness). */
            struct MYYAML_API utf32
            {
                static int decode(const char *data, std::size_t size, unsigned int &value,
                                  bool big_endian = false) noexcept;
                static int encode(unsigned int codepoint, char *output, std::size_t size,
                                  bool big_endian = false) noexcept;
                static std::string to_utf8(const std::vector<unsigned char> &bytes,
                                           bool big_endian = false);
            };

            /**
             * @brief Decode a byte buffer to UTF-8, stripping a BOM.
             * @throws encoding_error on invalid input.
             */
            MYYAML_API std::string decode_to_utf8(const std::string &bytes,
                                                  encoding detected = encoding::unspecified);

            /** @brief Abstract input adapter used by the parser. */
            class MYYAML_API iadapter
            {
            public:
                iadapter() = default;
                iadapter(const iadapter &) = default;
                iadapter(iadapter &&) noexcept = default;
                iadapter &operator=(const iadapter &) = default;
                iadapter &operator=(iadapter &&) noexcept = default;
                virtual ~iadapter() = default;
                virtual std::size_t read(void *data, std::size_t size) = 0;
            };

            /** @brief Input adapter over a memory buffer. */
            class MYYAML_API memory_iadapter : public iadapter
            {
            public:
                memory_iadapter(const void *data, std::size_t size);
                std::size_t read(void *data, std::size_t size) override;

            private:
                const unsigned char *m_data;
                std::size_t m_size;
                std::size_t m_pos{0};
            };

            /** @brief Input adapter over a C FILE. */
            class MYYAML_API file_iadapter : public iadapter
            {
            public:
                explicit file_iadapter(std::FILE *file);
                std::size_t read(void *data, std::size_t size) override;

            private:
                std::FILE *m_file;
            };

            /** @brief Input adapter over an std::istream. */
            class MYYAML_API stream_iadapter : public iadapter
            {
            public:
                explicit stream_iadapter(std::istream &stream);
                std::size_t read(void *data, std::size_t size) override;

            private:
                std::istream *m_stream;
            };

            /** @brief Read an entire adapter into a string. */
            MYYAML_API std::string read_all(iadapter &adapter);

            /** @brief Abstract output adapter used by the emitter. */
            class MYYAML_API oadapter
            {
            public:
                oadapter() = default;
                oadapter(const oadapter &) = default;
                oadapter(oadapter &&) noexcept = default;
                oadapter &operator=(const oadapter &) = default;
                oadapter &operator=(oadapter &&) noexcept = default;
                virtual ~oadapter() = default;
                virtual std::size_t write(const void *data, std::size_t size) = 0;
            };

            /** @brief Output adapter over a memory buffer. */
            class MYYAML_API memory_oadapter : public oadapter
            {
            public:
                memory_oadapter(void *data, std::size_t size);
                std::size_t write(const void *data, std::size_t size) override;

            private:
                unsigned char *m_data;
                std::size_t m_size;
                std::size_t m_pos{0};
            };

            /** @brief Output adapter over a C FILE. */
            class MYYAML_API file_oadapter : public oadapter
            {
            public:
                explicit file_oadapter(std::FILE *file);
                std::size_t write(const void *data, std::size_t size) override;

            private:
                std::FILE *m_file;
            };

            /** @brief Output adapter over an std::ostream. */
            class MYYAML_API stream_oadapter : public oadapter
            {
            public:
                explicit stream_oadapter(std::ostream &stream);
                std::size_t write(const void *data, std::size_t size) override;

            private:
                std::ostream *m_stream;
            };
        } // namespace detail
    } // namespace v0_2_0
} // namespace myyaml

#endif // DJOEZEKE_MYYAML_HPP
