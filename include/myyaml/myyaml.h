#ifndef DJOEZEKE_MYYAMLC_H
#define DJOEZEKE_MYYAMLC_H

// clang-format off

#ifndef MYYAMLC_SKIP_VERSION_CHECK
    #if defined(MYYAMLC_VERSION_MAJOR) && defined(MYYAMLC_VERSION_MINOR) && defined(MYYAMLC_VERSION_PATCH)
        #if MYYAMLC_VERSION_MAJOR != 0 || MYYAMLC_VERSION_MINOR != 1 || MYYAMLC_VERSION_PATCH != 0
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif  // MYYAMLC_SKIP_VERSION_CHECK

/**
 * @defgroup version Version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYYAMLC_VERSION_MAJOR
 * @brief Major version number of the library.
 * @note If this were version 1.2.3, this value would be 1.
 * @since This macro is available since 0.1.0.
 */
#define MYYAMLC_VERSION_MAJOR 0

/**
 * @def MYYAMLC_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were version 1.2.3, this value would be 2.
 * @since This macro is available since 0.1.0.
 */
#define MYYAMLC_VERSION_MINOR 1

/**
 * @def MYYAMLC_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were version 1.2.3, this value would be 3.
 * @since This macro is available since 0.1.0.
 */
#define MYYAMLC_VERSION_PATCH 0

/**
 * @def MYYAMLC_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 */
#define MYYAMLC_VERSION "0.1.0"

/** @} */

// clang-format on

//-----------------------------------------------------------------------------
// [SECTION] Structures
//-----------------------------------------------------------------------------

/**
 * @defgroup basic Basic Types
 * @brief Core types and data structures.
 * @{
 */

typedef struct myVersion {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
} myVersion;

/**
 * @brief Definition of Unicode encoding types
 */
typedef enum myEncoding {
    myUnspecifiedEncoding, /** Let the parser choose the encoding. */
    myUTF8Encoding,        /** The default UTF-8 encoding. */
    myUTF16Encoding,       /** The UTF-16-LE encoding with native endianness. */
    myUTF16LEEncoding,     /** The UTF-16-LE encoding with BOM. */
    myUTF16BEEncoding,     /** The UTF-16-BE encoding with BOM. */
    myUTF32Encoding,       /** The UTF-32 encoding with native endianness. */
    myUTF32LEEncoding,     /** The UTF-32-LE encoding with BOM. */
    myUTF32BEEncoding,     /** The UTF-32-BE encoding with BOM. */
} myEncoding;

/** @} */

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifdef __cplusplus
};
#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifdef __cplusplus
};
#endif  //__cplusplus

#endif  // DJOEZEKE_MYYAMLC_H

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