/**
 * @file myyaml.h
 * @brief YAML parser library header for C/C++.
 * This is a  C/C++ Yaml Parser Library Header @c myyaml.c.
 * @details This header provides all public API, types, macros, and configuration for YAML and supports both C and C++ usage.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @date Thur 02 11:08:15 Oct GMT 2025
 * @version 0.1.0
 * @see https://www.github.com/djoezeke/myyaml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Usage:
 * @code
 *  #include <myyaml.h>
 * @endcode
 *
 * Index of this file:
 *  [SECTION] Header mess
 *  [SECTION] Configurable macros
 *  [SECTION] Function Macros
 *  [SECTION] Imports/Exports
 *  [SECTION] Data Structures
 *  [SECTION] C Only Functions
 *  [SECTION] C++ Only Classes
 *
 *
 * Resources:
 * - Homepage ................... https://github.com/djoezeke/myyaml
 * - Releases & changelog ....... https://github.com/djoezeke/myyaml/releases
 * - Issues & support ........... https://github.com/djoezeke/myyaml/issues
 *
 */

#ifndef DJOEZEKE_MYYAML_H
#define DJOEZEKE_MYYAML_H

/**
 * @defgroup version Version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYYAML_VERSION_MAJOR
 * @brief Major version number of the library.
 */
#define MYYAML_VERSION_MAJOR 0

/**
 * @def MYYAML_VERSION_MINOR
 * @brief Minor version number of the library.
 */
#define MYYAML_VERSION_MINOR 1

/**
 * @def MYYAML_VERSION_PATCH
 * @brief Patch version number of the library.
 */
#define MYYAML_VERSION_PATCH 0

/**
 * @def MYYAML_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 */
#define MYYAML_VERSION "0.1.0"

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef __cplusplus

/** C++ Exclusive headers. */
#include <exception>
#include <iostream>

#endif //__cplusplus

#define MYYAML_DEBUG

#ifdef MYYAML_DEBUG
#endif // MYYAML_DEBUG

//-----------------------------------------------------------------------------
// [SECTION] Configurable Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Function Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Import/Export
//-----------------------------------------------------------------------------

/**
 * @defgroup export Export Definitions
 * @{
 */

#if defined(_WIN32)
#define MYYAML_NO_EXPORT_ATTR
#define MYYAML_API_EXPORT __declspec(dllexport)
#define MYYAML_API_IMPORT __declspec(dllimport)
#define MYYAML_DEPRECATED_ATTR __declspec(deprecated)
#else // _WIN32
#define MYYAML_API_EXPORT __attribute__((visibility("default")))
#define MYYAML_API_IMPORT __attribute__((visibility("default")))
#define MYYAML_NO_EXPORT_ATTR __attribute__((visibility("hidden")))
#define MYYAML_DEPRECATED_ATTR __attribute__((__deprecated__))
#endif // _WIN32

/**
 * @def MYYAML_API
 * @brief Macro for public API symbol export/import.
 * @details Use this macro to annotate all public API functions for correct symbol visibility on all platforms.
 */

#if defined(MYYAML_BUILD_STATIC)
#define MYYAML_API
#elif defined(MYYAML_BUILD_SHARED)
/* We are building this library */
#define MYYAML_API MYYAML_API_EXPORT
#elif defined(MYYAML_IMPORT)
/* We are using this library */
#define MYYAML_API MYYAML_API_IMPORT
#else // MYYAML_BUILD_STATIC
#define MYYAML_API
#endif // MYYAML_BUILD_STATIC

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

/**
 * @defgroup basic Basic Types
 * @brief Core types and data structures for YAML.
 * @{
 */

// Define YAML value types
typedef enum
{
    SCALAR,
    SEQUENCE,
    MAPPING,
    BOOLEAN,
    NIL,
    INTEGER,
    DOUBLE,
    STRING,
} YamlType;

// Forward declaration of Yaml
typedef struct Yaml Yaml;

// Define a YAML key-value pair for mappings
typedef struct
{
    char *key;
    Yaml *value;
} YamlKeyValuePair;

// Define a YAML value
struct Yaml
{
    YamlType type;
    union
    {
        char *scalar_value;
        struct
        {
            Yaml **items;
            size_t size;
        } sequence_value;
        struct
        {
            YamlKeyValuePair **items;
            size_t size;
        } mapping_value;
    } data;
};

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    Yaml *parse_scalar(const char **str);
    Yaml *parse_sequence(const char **str);
    Yaml *parse_mapping(const char **str);
    Yaml *parse_yaml(const char **str);
    Yaml *read_yaml_file(const char *filename);

    bool yaml_to_bool(Yaml *yaml);
    double yaml_to_double(Yaml *yaml);
    int yaml_to_int(Yaml *yaml);
    char *yaml_to_string(Yaml *yaml);
    void *yaml_to_null(Yaml *yaml);

    Yaml *yaml_create_scalar(const char *scalar_value);
    Yaml *yaml_create_sequence();
    void yaml_sequence_add(Yaml *sequence, Yaml *item);
    Yaml *yaml_create_mapping();
    void yaml_mapping_add(Yaml *mapping, const char *key, Yaml *value);

    void yaml_print(Yaml *value, int indent);
    void yaml_pretty_print(Yaml *value, int indent);

    void yaml_free(Yaml *value);

#ifdef __cplusplus
}
#endif //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif //__cplusplus

#endif // DJOEZEKE_MYYAML_H
