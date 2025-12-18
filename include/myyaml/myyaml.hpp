/**
 * @file myyaml.hpp
 * @brief YAML parser library header for C/C++.
 * This is a  C/C++ Yaml Parser Library Header @c myyaml.c
 * @details This header provides all public API, types, macros, and
 * configuration for YAML and supports both C and C++ usage.
 * @author Sackey Ezekiel -Etrue (djoezeke)
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

// clang-format off

#ifndef MYYAML_SKIP_VERSION_CHECK
    #if defined(MYYAML_VERSION_MAJOR) && defined(MYYAML_VERSION_MINOR) && defined(MYYAML_VERSION_PATCH)
        #if MYYAML_VERSION_MAJOR != 0 || MYYAML_VERSION_MINOR != 1 || MYYAML_VERSION_PATCH != 0
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif  // MYYAML_SKIP_VERSION_CHECK

/**
 * @defgroup version Version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYYAML_VERSION_MAJOR
 * @brief Major version number of the library.
 * @note If this were Myyaml version 1.2.3, this value would be 1.
 * @since This macro is available since Myyaml 0.1.0.
 */
#define MYYAML_VERSION_MAJOR 0

/**
 * @def MYYAML_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were Myyaml version 1.2.3, this value would be 2.
 * @since This macro is available since Myyaml 0.1.0.
 */
#define MYYAML_VERSION_MINOR 1

/**
 * @def MYYAML_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were Myyaml version 1.2.3, this value would be 3.
 * @since This macro is available since Myyaml 0.1.0.
 */
#define MYYAML_VERSION_PATCH 0

/**
 * @def MYYAML_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 * @sa MyGetVersion
 */
#define MYYAML_VERSION "0.1.0"

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>  // FILE 
#include <stdint.h>

#ifdef __cplusplus

    /** C++ Exclusive headers. */
    #include <exception>

#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] Configurable Macros
//-----------------------------------------------------------------------------

/**
 * @def MYYAML_DISABLE_READER
 * @brief Exclude Reading/Deserialization YAML methods.
 * Define as 1 to diable YAML reader if parsing is not required.
 *
 * @warning This will disable these function at compile-time.
 *
 * @note This will reduce the binary size by about 60%.
 */
#ifndef MYYAML_DISABLE_READER
#endif

/**
 * @def MYYAML_DISABLE_WRITER
 * @brief Exclude Writing/Serialization YAML methods.
 * Define as 1 to disable YAML writer if YAML serialization is not required.
 *
 * @warning This will disable these function at compile-time.
 *
 * @note This will reduce the binary size by about 30%.
 */
#ifndef MYYAML_DISABLE_WRITER
#endif

/**
 * @def MYYAML_DISABLE_ENCODING
 * @brief Exclude Encoding/UTF-8 YAML methods.
 * Define as 1 to disable UTF-8 validation at compile time.
 *
 * @warning This will disable these function at compile-time.
 *
 * @note This will reduce the binary size by about 7%.
 */
#ifndef MYYAML_DISABLE_ENCODING
#endif


//-----------------------------------------------------------------------------
// [SECTION] Yaml Tags
//-----------------------------------------------------------------------------

/**
 * @defgroup tags Yaml Tags
 * @{
 */

/** The tag @c !!str for string values. */
#define MYYAML_YAML_STR_TAG "tag:yaml.org,2002:str"

/**
 * !!str: String (text data).
 */
#define YAML_STR_TAG MYYAML_YAML_STR_TAG

/** The tag @c !!int for integer values. */
#define MYYAML_YAML_INT_TAG "tag:yaml.org,2002:int"

/**
 * !!int: Integer (whole numbers).
 */
#define YAML_INT_TAG MYYAML_YAML_INT_TAG

/** The tag @c !!seq is used to denote sequences. */
#define MYYAML_YAML_SEQ_TAG "tag:yaml.org,2002:seq"

/**
 * !!seq: Sequence (ordered collection, similar to a list or array).
 */
#define YAML_SEQ_TAG MYYAML_YAML_SEQ_TAG

/** The tag @c !!set is used to denote set. */
#define MYYAML_YAML_SET_TAG "tag:yaml.org,2002:set"

/**
 * !!set: Set (unordered collection of unique items).
 */
#define YAML_SET_TAG MYYAML_YAML_SET_TAG

/** The tag @c !!map is used to denote mapping. */
#define MYYAML_YAML_MAP_TAG "tag:yaml.org,2002:map"

/**
 * !!map: Mapping (unordered collection of key-value pairs, similar to a dictionary or hash map).
 */
#define YAML_MAP_TAG MYYAML_YAML_MAP_TAG

/** The tag @c !!omap is used to denote ordered mapping. */
#define MYYAML_YAML_OMAP_TAG "tag:yaml.org,2002:omap"

/**
 * !!omap: Ordered map (mapping where order is preserved).
 */
#define YAML_OMAP_TAG MYYAML_YAML_OMAP_TAG

/** The tag @c !!null with the only possible value: @c null. */
#define MYYAML_YAML_NULL_TAG "tag:yaml.org,2002:null"

/**
 * !!null: Null value.
 */
#define YAML_NULL_TAG MYYAML_YAML_NULL_TAG

/** The tag @c !!bool with the values: @c true and @c false. */
#define MYYAML_YAML_BOOL_TAG "tag:yaml.org,2002:bool"

/**
 * !!bool: Boolean (true or false). Recommended to use true and false for compatibility.
 */
#define YAML_BOOL_TAG MYYAML_YAML_BOOL_TAG

/** The tag @c !!float for float values. */
#define MYYAML_YAML_FLOAT_TAG "tag:yaml.org,2002:float"

/**
 * !!float: Floating-point number (numbers with fractional parts).
 */
#define YAML_FLOAT_TAG MYYAML_YAML_FLOAT_TAG

/** The tag @c !!binary for binary data. */
#define MYYAML_YAML_BINARY_TAG "tag:yaml.org,2002:binary"

/**
 * !!binary: Binary data (encoded as base64).
 */
#define YAML_BINARY_TAG MYYAML_YAML_BINARY_TAG

/** The tag @c !!timestamp for date and time values. */
#define MYYAML_YAML_TIMESTAMP_TAG "tag:yaml.org,2002:timestamp"

/**
 * !!timestamp: Timestamp (date and time).
 */
#define YAML_TIMESTAMP_TAG MYYAML_YAML_TIMESTAMP_TAG

/** The default scalar tag is @c !!str. */
#define MYYAML_YAML_DEFAULT_SCALAR_TAG MYYAML_YAML_STR_TAG
#define YAML_DEFAULT_SCALAR_TAG MYYAML_YAML_DEFAULT_SCALAR_TAG

/** The default sequence tag is @c !!seq. */
#define MYYAML_YAML_DEFAULT_SEQUENCE_TAG MYYAML_YAML_SEQ_TAG
#define YAML_DEFAULT_SEQUENCE_TAG MYYAML_YAML_DEFAULT_SEQUENCE_TAG

/** The default mapping tag is @c !!map. */
#define MYYAML_YAML_DEFAULT_MAPPING_TAG MYYAML_YAML_MAP_TAG
#define YAML_DEFAULT_MAPPING_TAG MYYAML_YAML_DEFAULT_MAPPING_TAG

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Import/Export
//-----------------------------------------------------------------------------

/**
 * @defgroup export Export Definitions
 * @{
 */

#if defined(_WIN32)
  	#define MYYAML_API_EXPORT __declspec(dllexport)
  	#define MYYAML_API_IMPORT __declspec(dllimport)
  	#define MYYAML_NO_EXPORT
#else  // _WIN32
  	#define MYYAML_API_EXPORT __attribute__((visibility("default")))
  	#define MYYAML_API_IMPORT __attribute__((visibility("default")))
  	#define MYYAML_NO_EXPORT __attribute__((visibility("hidden")))
#endif  // _WIN32

/**
 * @def MYYAML_API
 * @brief Macro for public API symbol export/import.
 * @details Use this macro to annotate all public API functions for correct
 * symbol visibility on all platforms.
 */

#if defined(MYYAML_BUILD_STATIC)
  	#define MYYAML_API
#elif defined(MYYAML_BUILD_SHARED) || defined(MYYAML_EXPORTS)
  	/* We are building this library */
  	#define MYYAML_API MYYAML_API_EXPORT
#elif defined(MYYAML_LOAD_SHARED) || defined(MYYAML_IMPORTS)
  	/* We are using this library */
  	#define MYYAML_API MYYAML_API_IMPORT
#else  // MYYAML_BUILD_STATIC
  	#define MYYAML_API
#endif  // MYYAML_BUILD_STATIC

/** @} */

// clang-format on

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

namespace myyaml {

};  // namespace myyaml

#endif  //__cplusplus

#endif  // DJOEZEKE_MYYAML_H

/**
 * LICENSE: MIT License
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
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
 */