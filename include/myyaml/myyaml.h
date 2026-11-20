/**
 * @file myyaml.h
 * @brief C API for the myyaml YAML library.
 * @details This header exposes an opaque, exception-free C interface for
 * parsing, reading, building and emitting YAML documents.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.2.0
 * @see https://www.github.com/djoezeke/myyaml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * The C API is a thin wrapper around the C++ implementation. All functions
 * report failures by returning NULL/0 or a status code and store a
 * human-readable message that can be retrieved with myyaml_last_error().
 * The error state is thread-local.
 *
 * Handles returned by myyaml_root(), myyaml_node_get(), myyaml_node_at() and
 * myyaml_node_set_key() remain valid until the document is modified or
 * released. Calling myyaml_free() invalidates every handle belonging to the
 * document.
 *
 * Example:
 * @code
 * myyaml_document *doc = myyaml_parse("name: demo\nitems: [1, 2]\n");
 * if (doc == NULL) {
 *     fprintf(stderr, "%s\n", myyaml_last_error());
 *     return 1;
 * }
 * const myyaml_node *items = myyaml_node_get(myyaml_root(doc), "items");
 * printf("items: %zu\n", myyaml_node_size(items));
 * char *text = myyaml_dump(doc);
 * puts(text);
 * myyaml_free_string(text);
 * myyaml_free(doc);
 * @endcode
 */

#ifndef DJOEZEKE_MYYAMLC_H
#define DJOEZEKE_MYYAMLC_H

#include <stddef.h>

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

//-----------------------------------------------------------------------------
// [SECTION] Version
//-----------------------------------------------------------------------------

/** @brief Major version number of the C API. */
#define MYYAMLC_VERSION_MAJOR 0
/** @brief Minor version number of the C API. */
#define MYYAMLC_VERSION_MINOR 2
/** @brief Patch version number of the C API. */
#define MYYAMLC_VERSION_PATCH 0
/** @brief Version string of the C API ("X.Y.Z"). */
#define MYYAMLC_VERSION "0.2.0"

//-----------------------------------------------------------------------------
// [SECTION] Export
//-----------------------------------------------------------------------------

#if defined(_WIN32)
#define MYYAMLC_API_EXPORT __declspec(dllexport)
#define MYYAMLC_API_IMPORT __declspec(dllimport)
#else
#define MYYAMLC_API_EXPORT __attribute__((visibility("default")))
#define MYYAMLC_API_IMPORT __attribute__((visibility("default")))
#endif

#if defined(MYYAMLC_BUILD_STATIC)
#define MYYAMLC_API
#elif defined(MYYAMLC_BUILD_SHARED) || defined(MYYAMLC_EXPORTS)
#define MYYAMLC_API MYYAMLC_API_EXPORT
#elif defined(MYYAMLC_LOAD_SHARED) || defined(MYYAMLC_IMPORTS)
#define MYYAMLC_API MYYAMLC_API_IMPORT
#else
#define MYYAMLC_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------
// [SECTION] Types
//-----------------------------------------------------------------------------

/** @brief Opaque YAML document handle. */
typedef struct myyaml_document myyaml_document;

/** @brief Opaque YAML node handle. */
typedef struct myyaml_node myyaml_node;

/**
 * @brief Status code returned by operations that can fail.
 */
typedef enum myyaml_status
{
    MYYAML_STATUS_OK = 0,              /**< Operation succeeded. */
    MYYAML_STATUS_PARSE_ERROR,         /**< Invalid YAML syntax. */
    MYYAML_STATUS_TYPE_ERROR,          /**< A value has an unexpected type. */
    MYYAML_STATUS_LOOKUP_ERROR,        /**< A mapping key or index is missing. */
    MYYAML_STATUS_ENCODING_ERROR,      /**< Invalid Unicode input or output. */
    MYYAML_STATUS_IO_ERROR,            /**< File or stream I/O failed. */
    MYYAML_STATUS_INVALID_ARGUMENT,    /**< A NULL or otherwise invalid argument. */
    MYYAML_STATUS_OUT_OF_MEMORY,       /**< Allocation failed. */
    MYYAML_STATUS_UNKNOWN              /**< An unexpected error occurred. */
} myyaml_status;

/**
 * @brief Type of a YAML node.
 */
typedef enum myyaml_type
{
    MYYAML_TYPE_NULL = 0, /**< Null value. */
    MYYAML_TYPE_BOOLEAN,  /**< Boolean value. */
    MYYAML_TYPE_INTEGER,  /**< Signed 64-bit integer. */
    MYYAML_TYPE_FLOATING, /**< Double precision floating-point value. */
    MYYAML_TYPE_STRING,   /**< UTF-8 string. */
    MYYAML_TYPE_SEQUENCE, /**< Ordered sequence. */
    MYYAML_TYPE_MAPPING   /**< Ordered mapping (insertion order is preserved). */
} myyaml_type;

/**
 * @brief Parser limits. A zero field selects the library default.
 */
typedef struct myyaml_parse_options
{
    size_t max_depth;   /**< Maximum nesting depth (default 128). */
    size_t max_input_size; /**< Maximum input size in bytes (default 16 MiB). */
    size_t max_nodes;   /**< Maximum node count (default 1000000). */
    int allow_duplicate_keys; /**< Non-zero to accept duplicate mapping keys. */
} myyaml_parse_options;

/**
 * @brief Emitter options.
 *
 * Numeric fields use zero to select the library default. Boolean fields are
 * interpreted literally, so zero-initialize the structure and explicitly set
 * the flags that should be enabled (in particular trailing_newline).
 */
typedef struct myyaml_dump_options
{
    int indent;            /**< Spaces per level, 1..8 (default 2). */
    int flow;              /**< Non-zero to emit flow-style collections. */
    int sort_keys;         /**< Non-zero to sort mapping keys. */
    int explicit_start;    /**< Non-zero to emit a "---" marker. */
    int explicit_end;      /**< Non-zero to emit a "..." marker. */
    int trailing_newline;  /**< Non-zero to append a trailing newline. */
} myyaml_dump_options;

//-----------------------------------------------------------------------------
// [SECTION] Version and error state
//-----------------------------------------------------------------------------

/** @brief Return the version string, for example "0.2.0". */
MYYAMLC_API const char *myyaml_version_string(void);

/** @brief Store the version components in the given pointers (any may be NULL). */
MYYAMLC_API void myyaml_version(unsigned int *major, unsigned int *minor,
                                unsigned int *patch);

/** @brief Return the status of the most recent failed operation on this thread. */
MYYAMLC_API myyaml_status myyaml_last_status(void);

/** @brief Return the error message from the most recent failure on this thread. */
MYYAMLC_API const char *myyaml_last_error(void);

/** @brief Return the 1-based line of the most recent parse error, or 0. */
MYYAMLC_API size_t myyaml_last_error_line(void);

/** @brief Return the 1-based column of the most recent parse error, or 0. */
MYYAMLC_API size_t myyaml_last_error_column(void);

/** @brief Return the 0-based byte offset of the most recent parse error, or 0. */
MYYAMLC_API size_t myyaml_last_error_index(void);

//-----------------------------------------------------------------------------
// [SECTION] Parsing
//-----------------------------------------------------------------------------

/**
 * @brief Parse a NUL-terminated UTF-8 YAML document.
 * @return A new document, or NULL on failure (see myyaml_last_error()).
 */
MYYAMLC_API myyaml_document *myyaml_parse(const char *text);

/**
 * @brief Parse a UTF-8 YAML document of exactly @p length bytes.
 * @return A new document, or NULL on failure (see myyaml_last_error()).
 */
MYYAMLC_API myyaml_document *myyaml_parse_n(const char *text, size_t length);

/**
 * @brief Parse a YAML document with custom limits.
 * @return A new document, or NULL on failure (see myyaml_last_error()).
 */
MYYAMLC_API myyaml_document *myyaml_parse_with_options(const char *text,
                                                       size_t length,
                                                       const myyaml_parse_options *options);

/**
 * @brief Parse a YAML file.
 * @return A new document, or NULL on failure (see myyaml_last_error()).
 */
MYYAMLC_API myyaml_document *myyaml_parse_file(const char *path);

/** @brief Release a document and all node handles belonging to it. */
MYYAMLC_API void myyaml_free(myyaml_document *document);

//-----------------------------------------------------------------------------
// [SECTION] Document construction
//-----------------------------------------------------------------------------

/** @brief Create an empty document whose root is null. */
MYYAMLC_API myyaml_document *myyaml_create(void);

/** @brief Return the root node of a document (never NULL for a valid document). */
MYYAMLC_API myyaml_node *myyaml_root(myyaml_document *document);

/** @brief Return the root node of a read-only document. */
MYYAMLC_API const myyaml_node *myyaml_const_root(const myyaml_document *document);

/** @brief Replace a node with null. */
MYYAMLC_API void myyaml_node_set_null(myyaml_node *node);

/** @brief Replace a node with a boolean. */
MYYAMLC_API void myyaml_node_set_bool(myyaml_node *node, int value);

/** @brief Replace a node with a signed 64-bit integer. */
MYYAMLC_API void myyaml_node_set_integer(myyaml_node *node, long long value);

/** @brief Replace a node with a double. */
MYYAMLC_API void myyaml_node_set_double(myyaml_node *node, double value);

/** @brief Replace a node with a UTF-8 string (copied). Returns 0 on failure. */
MYYAMLC_API int myyaml_node_set_string(myyaml_node *node, const char *value);

/**
 * @brief Ensure the node is a mapping and return the child for @p key.
 *
 * A missing key is inserted with a null value. Returns NULL when the node is
 * not a mapping or can no longer be converted to one.
 */
MYYAMLC_API myyaml_node *myyaml_node_set_key(myyaml_node *node, const char *key);

/** @brief Ensure the node is a sequence and append a null element. */
MYYAMLC_API myyaml_node *myyaml_node_append(myyaml_node *node);

/** @brief Append a deep copy of @p value to a sequence node. */
MYYAMLC_API myyaml_node *myyaml_node_append_copy(myyaml_node *node,
                                                 const myyaml_node *value);

/** @brief Erase a mapping key. Returns 1 when the key existed. */
MYYAMLC_API int myyaml_node_erase_key(myyaml_node *node, const char *key);

/** @brief Erase a sequence element. Returns 1 when the index existed. */
MYYAMLC_API int myyaml_node_erase_index(myyaml_node *node, size_t index);

/** @brief Clear a container or string; other values become null. */
MYYAMLC_API void myyaml_node_clear(myyaml_node *node);

//-----------------------------------------------------------------------------
// [SECTION] Reading
//-----------------------------------------------------------------------------

/** @brief Return the node type. A NULL node is reported as null. */
MYYAMLC_API myyaml_type myyaml_node_type(const myyaml_node *node);

/** @brief Return a human-readable name for a node type. */
MYYAMLC_API const char *myyaml_type_name(myyaml_type type);

/** @brief Return non-zero when the node holds a null value. */
MYYAMLC_API int myyaml_node_is_null(const myyaml_node *node);
/** @brief Return non-zero when the node holds a boolean. */
MYYAMLC_API int myyaml_node_is_bool(const myyaml_node *node);
/** @brief Return non-zero when the node holds an integer. */
MYYAMLC_API int myyaml_node_is_integer(const myyaml_node *node);
/** @brief Return non-zero when the node holds a floating-point value. */
MYYAMLC_API int myyaml_node_is_double(const myyaml_node *node);
/** @brief Return non-zero when the node holds a string. */
MYYAMLC_API int myyaml_node_is_string(const myyaml_node *node);
/** @brief Return non-zero when the node holds a sequence. */
MYYAMLC_API int myyaml_node_is_sequence(const myyaml_node *node);
/** @brief Return non-zero when the node holds a mapping. */
MYYAMLC_API int myyaml_node_is_mapping(const myyaml_node *node);

/**
 * @brief Read a boolean. Returns 1 on success and 0 on type mismatch.
 */
MYYAMLC_API int myyaml_node_get_bool(const myyaml_node *node, int *value);

/**
 * @brief Read an integer. Returns 1 on success and 0 on type mismatch.
 */
MYYAMLC_API int myyaml_node_get_integer(const myyaml_node *node, long long *value);

/**
 * @brief Read a double. Returns 1 on success and 0 on type mismatch.
 */
MYYAMLC_API int myyaml_node_get_double(const myyaml_node *node, double *value);

/**
 * @brief Return a NUL-terminated UTF-8 string, or NULL on type mismatch.
 *
 * Use myyaml_node_string_length() when the value may contain NUL bytes.
 * The pointer is owned by the document and must not be freed.
 */
MYYAMLC_API const char *myyaml_node_get_string(const myyaml_node *node);

/** @brief Return the byte length of a string node, or 0 on type mismatch. */
MYYAMLC_API size_t myyaml_node_string_length(const myyaml_node *node);

/** @brief Number of elements in a collection, bytes in a string, or 0. */
MYYAMLC_API size_t myyaml_node_size(const myyaml_node *node);

/** @brief Return the sequence element at @p index, or NULL. */
MYYAMLC_API const myyaml_node *myyaml_node_at(const myyaml_node *node, size_t index);

/** @brief Return the mapping key at @p index in insertion order, or NULL. */
MYYAMLC_API const char *myyaml_node_key_at(const myyaml_node *node, size_t index);

/** @brief Return the mapping value for @p key, or NULL when it is missing. */
MYYAMLC_API const myyaml_node *myyaml_node_get(const myyaml_node *node, const char *key);

/** @brief Return non-zero when a mapping contains @p key. */
MYYAMLC_API int myyaml_node_has(const myyaml_node *node, const char *key);

//-----------------------------------------------------------------------------
// [SECTION] Emitting
//-----------------------------------------------------------------------------

/**
 * @brief Serialize a document with default options.
 * @return A NUL-terminated string allocated with malloc, or NULL on failure.
 *         Release it with myyaml_free_string().
 */
MYYAMLC_API char *myyaml_dump(const myyaml_document *document);

/**
 * @brief Serialize a document with custom options.
 * @return A NUL-terminated string allocated with malloc, or NULL on failure.
 */
MYYAMLC_API char *myyaml_dump_with_options(const myyaml_document *document,
                                           const myyaml_dump_options *options);

/** @brief Release a string returned by myyaml_dump() or myyaml_dump_with_options(). */
MYYAMLC_API void myyaml_free_string(char *text);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* DJOEZEKE_MYYAMLC_H */
