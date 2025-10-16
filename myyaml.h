/**
 * @file myyaml.h
 * @brief YAML parser library header for C/C++.
 * This is a  C/C++ Yaml Parser Library Header @c myyaml.c.
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

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>   //
#include <stdlib.h>  //
#include <string.h>  //

#ifdef __cplusplus

  /** C++ Exclusive headers. */
  #include <algorithm>
  #include <cctype>
  #include <exception>
  #include <fstream>
  #include <iostream>
  #include <map>
  #include <sstream>
  #include <string>
  #include <type_traits>
  #include <vector>

#endif  //__cplusplus

#ifdef MYYAML_DEBUG
#endif  // MYYAML_DEBUG

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

/**
 * @def MYYAML_ASSERT
 * @brief Apply the default assert.
 */
#ifndef MYYAML_ASSERT
	#ifndef NDEBUG
    	#include <assert.h>
    	#define MYYAML_ASSERT(x) assert(x)
	#else
		#define MYYAML_ASSERT(x)
	#endif
#endif

#define MYYAML_SUCCESS 1
#define MYYAML_FAILURE 0

/** The tag @c !!null with the only possible value: @c null. */
#define MYYAML_YAML_NULL_TAG "tag:yaml.org,2002:null"
#define YAML_NULL_TAG MYYAML_YAML_NULL_TAG

/** The tag @c !!bool with the values: @c true and @c false. */
#define MYYAML_YAML_BOOL_TAG "tag:yaml.org,2002:bool"
#define YAML_BOOL_TAG MYYAML_YAML_BOOL_TAG

/** The tag @c !!str for string values. */
#define MYYAML_YAML_STR_TAG "tag:yaml.org,2002:str"
#define YAML_STR_TAG MYYAML_YAML_STR_TAG

/** The tag @c !!int for integer values. */
#define MYYAML_YAML_INT_TAG "tag:yaml.org,2002:int"
#define YAML_INT_TAG MYYAML_YAML_INT_TAG

/** The tag @c !!float for float values. */
#define MYYAML_YAML_FLOAT_TAG "tag:yaml.org,2002:float"
#define YAML_FLOAT_TAG MYYAML_YAML_FLOAT_TAG

/** The tag @c !!timestamp for date and time values. */
#define MYYAML_YAML_TIMESTAMP_TAG "tag:yaml.org,2002:timestamp"
#define YAML_TIMESTAMP_TAG MYYAML_YAML_TIMESTAMP_TAG

/** The tag @c !!seq is used to denote sequences. */
#define MYYAML_YAML_SEQ_TAG "tag:yaml.org,2002:seq"
#define YAML_SEQ_TAG MYYAML_YAML_SEQ_TAG

/** The tag @c !!map is used to denote mapping. */
#define MYYAML_YAML_MAP_TAG "tag:yaml.org,2002:map"
#define YAML_MAP_TAG MYYAML_YAML_MAP_TAG

/** The default scalar tag is @c !!str. */
#define MYYAML_YAML_DEFAULT_SCALAR_TAG MYYAML_YAML_STR_TAG
#define YAML_DEFAULT_SCALAR_TAG MYYAML_YAML_DEFAULT_SCALAR_TAG

/** The default sequence tag is @c !!seq. */
#define MYYAML_YAML_DEFAULT_SEQUENCE_TAG MYYAML_YAML_SEQ_TAG
#define YAML_DEFAULT_SEQUENCE_TAG MYYAML_YAML_DEFAULT_SEQUENCE_TAG

/** The default mapping tag is @c !!map. */
#define MYYAML_YAML_DEFAULT_MAPPING_TAG MYYAML_YAML_MAP_TAG
#define YAML_DEFAULT_MAPPING_TAG MYYAML_YAML_DEFAULT_MAPPING_TAG

//-----------------------------------------------------------------------------
// [SECTION] Platform
//-----------------------------------------------------------------------------

/**
 * @defgroup platform Platform Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler is of given brand.
 * @param   name Platform, like `APPLE`.
 * @retval  true   It is.
 * @retval  false  It isn't.
 */
#define MYYAML_PLATFORM_IS(name) MYYAML_PLATFORM_IS_##name

/**
 * @brief  Returns the current platform name.
 * @return  platform name.
 */
#ifdef __APPLE__
	/**
	* A preprocessor macro that is only defined if compiling for MacOS.
	*/
	#define MYYAML_PLATFORM_IS_APPLE 1
	/**
	 * @brief  Returns the current platform name.
	 * @return  platform name.
	 */
  #define MYYAML_PLATFORM_NAME_IS "Apple"
#elif defined(linux) || defined(__linux) || defined(__linux__)
	/**
	* A preprocessor macro that is only defined if compiling for Linux.
	*/
	#define MYYAML_PLATFORM_IS_LINUX 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYYAML_PLATFORM_NAME_IS "Linux"
#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
  	/**
   	* A preprocessor macro that is only defined if compiling for Windows.
   	*/
  	#define MYYAML_PLATFORM_IS_WINDOWS 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYYAML_PLATFORM_NAME_IS "Windows"
#else
  	/**
   	* A preprocessor macro that is only defined if compiling for others.
   	*/
  	#define MYYAML_PLATFORM_IS_OTHERS 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYYAML_PLATFORM_NAME_IS "Others"
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler is of given brand.
 * @param   name  Compiler brand, like `MSVC`.
 * @retval  true   It is.
 * @retval  false  It isn't.
 */
#define MYYAML_COMPILER_IS(name) MYYAML_COMPILER_IS_##name

/// Compiler is apple
#if !defined(__clang__)
	#define MYYAML_COMPILER_IS_APPLE 0
#elif !defined(__apple_build_version__)
	#define MYYAML_COMPILER_IS_APPLE 0
#else
	#define MYYAML_COMPILER_IS_APPLE 1
	#define MYYAML_COMPILER_VERSION_MAJOR __clang_major__
	#define MYYAML_COMPILER_VERSION_MINOR __clang_minor__
	#define MYYAML_COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

/// Compiler is clang
#if !defined(__clang__)
	#define MYYAML_COMPILER_IS_CLANG 0
#elif MYYAML_COMPILER_IS(APPLE)
	#define MYYAML_COMPILER_IS_CLANG 0
#else
	#define MYYAML_COMPILER_IS_CLANG 1
	#define MYYAML_COMPILER_VERSION_MAJOR __clang_major__
	#define MYYAML_COMPILER_VERSION_MINOR __clang_minor__
	#define MYYAML_COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

/// Compiler is intel
#if !defined(__INTEL_COMPILER)
	#define MYYAML_COMPILER_IS_INTEL 0
#elif !defined(__INTEL_COMPILER_UPDATE)
	#define MYYAML_COMPILER_IS_INTEL 1
	/* __INTEL_COMPILER = XXYZ */
	#define MYYAML_COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
	#define MYYAML_COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100 / 10)
	#define MYYAML_COMPILER_VERSION_PATCH (__INTEL_COMPILER % 10)
#else
	#define MYYAML_COMPILER_IS_INTEL 1
	/* __INTEL_COMPILER = XXYZ */
	#define MYYAML_COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
	#define MYYAML_COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100 / 10)
	#define MYYAML_COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#endif

/// Compiler is msc
#if !defined(_MSC_VER)
	#define MYYAML_COMPILER_IS_MSVC 0
#elif MYYAML_COMPILER_IS(CLANG)
	#define MYYAML_COMPILER_IS_MSVC 0
#elif MYYAML_COMPILER_IS(INTEL)
	#define MYYAML_COMPILER_IS_MSVC 0
#elif _MSC_VER >= 1400
	#define MYYAML_COMPILER_IS_MSVC 1
	/* _MSC_FULL_VER = XXYYZZZZZ */
	#define MYYAML_COMPILER_VERSION_MAJOR (_MSC_FULL_VER / 10000000)
	#define MYYAML_COMPILER_VERSION_MINOR (_MSC_FULL_VER % 10000000 / 100000)
	#define MYYAML_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#elif defined(_MSC_FULL_VER)
	#define MYYAML_COMPILER_IS_MSVC 1
	/* _MSC_FULL_VER = XXYYZZZZ */
	#define MYYAML_COMPILER_VERSION_MAJOR (_MSC_FULL_VER / 1000000)
	#define MYYAML_COMPILER_VERSION_MINOR (_MSC_FULL_VER % 1000000 / 10000)
	#define MYYAML_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
#else
	#define MYYAML_COMPILER_IS_MSVC 1
	/* _MSC_VER = XXYY */
	#define MYYAML_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
	#define MYYAML_COMPILER_VERSION_MINOR (_MSC_VER % 100)
	#define MYYAML_COMPILER_VERSION_PATCH 0
#endif

/// Compiler is gcc
#if !defined(__GNUC__)
	#define MYYAML_COMPILER_IS_GCC 0
#elif MYYAML_COMPILER_IS(APPLE)
	#define MYYAML_COMPILER_IS_GCC 0
#elif MYYAML_COMPILER_IS(CLANG)
	#define MYYAML_COMPILER_IS_GCC 0
#elif MYYAML_COMPILER_IS(INTEL)
	#define MYYAML_COMPILER_IS_GCC 0
#else
	#define MYYAML_COMPILER_IS_GCC 1
	#define MYYAML_COMPILER_VERSION_MAJOR __GNUC__
	#define MYYAML_COMPILER_VERSION_MINOR __GNUC_MINOR__
	#define MYYAML_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#endif

// Compiler Checks

/**
 * @brief   Checks if the compiler is of given brand and is newer than or equal
 *          to the passed version.
 * @param   name     Compiler brand, like `MSVC`.
 * @param   x      Major version.
 * @param   y      Minor version.
 * @param   z      Patchlevel.
 * @retval  true   name >= x.y.z.
 * @retval  false  otherwise.
 */
#define MYYAML_COMPILER_SINCE(name, x, y, z)                                 \
  (MYYAML_COMPILER_IS(name) && ((MYYAML_COMPILER_VERSION_MAJOR > (x)) ||     \
                                ((MYYAML_COMPILER_VERSION_MAJOR == (x)) &&   \
                                 ((MYYAML_COMPILER_VERSION_MINOR > (y)) ||   \
                                  ((MYYAML_COMPILER_VERSION_MINOR == (y)) && \
                                   (MYYAML_COMPILER_VERSION_PATCH >= (z)))))))

/**
 * @brief   Checks if  the compiler  is of  given brand and  is older  than the
 *          passed version.
 * @param   name     Compiler brand, like `MSVC`.
 * @param   x      Major version.
 * @param   y      Minor version.
 * @param   z      Patchlevel.
 * @retval  true   name < x.y.z.
 * @retval  false  otherwise.
 */
#define MYYAML_COMPILER_BEFORE(name, x, y, z)                                \
  (MYYAML_COMPILER_IS(name) && ((MYYAML_COMPILER_VERSION_MAJOR < (x)) ||     \
                                ((MYYAML_COMPILER_VERSION_MAJOR == (x)) &&   \
                                 ((MYYAML_COMPILER_VERSION_MINOR < (y)) ||   \
                                  ((MYYAML_COMPILER_VERSION_MINOR == (y)) && \
                                   (MYYAML_COMPILER_VERSION_PATCH < (z)))))))

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Warnings
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Warnings
 * @{
 */

#if MYYAML_COMPILER_IS(CLANG)
	#define MYYAML_PRAGMA_TO_STR(x) _Pragma(#x)
	#define MYYAML_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
	#define MYYAML_CLANG_SUPPRESS_WARNING(w) MYYAML_PRAGMA_TO_STR(clang diagnostic ignored w)
	#define MYYAML_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
	#define MYYAML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w) MYYAML_CLANG_SUPPRESS_WARNING_PUSH MYYAML_CLANG_SUPPRESS_WARNING(w)
#else  // MYYAML_CLANG
	#define MYYAML_CLANG_SUPPRESS_WARNING_PUSH
	#define MYYAML_CLANG_SUPPRESS_WARNING(w)
	#define MYYAML_CLANG_SUPPRESS_WARNING_POP
	#define MYYAML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif  // MYYAML_CLANG

#if MYYAML_COMPILER_IS(GCC)
	#define MYYAML_PRAGMA_TO_STR(x) _Pragma(#x)
	#define MYYAML_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
	#define MYYAML_GCC_SUPPRESS_WARNING(w) MYYAML_PRAGMA_TO_STR(GCC diagnostic ignored w)
	#define MYYAML_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
	#define MYYAML_GCC_SUPPRESS_WARNING_WITH_PUSH(w) MYYAML_GCC_SUPPRESS_WARNING_PUSH MYYAML_GCC_SUPPRESS_WARNING(w)
#else  // MYYAML_GCC
	#define MYYAML_GCC_SUPPRESS_WARNING_PUSH
	#define MYYAML_GCC_SUPPRESS_WARNING(w)
	#define MYYAML_GCC_SUPPRESS_WARNING_POP
	#define MYYAML_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif  // MYYAML_GCC

#if MYYAML_COMPILER_IS(MSVC)
	#define MYYAML_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
	#define MYYAML_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
	#define MYYAML_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
	#define MYYAML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w) MYYAML_MSVC_SUPPRESS_WARNING_PUSH MYYAML_MSVC_SUPPRESS_WARNING(w)
#else  // MYYAML_MSVC
	#define MYYAML_MSVC_SUPPRESS_WARNING_PUSH
	#define MYYAML_MSVC_SUPPRESS_WARNING(w)
	#define MYYAML_MSVC_SUPPRESS_WARNING_POP
	#define MYYAML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif  // MYYAML_MSVC

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Checks
//-----------------------------------------------------------------------------

/**
 * @defgroup check Compiler Checks
 * @{
 */

/** C version (STDC) */
#if defined(__STDC__) && (__STDC__ >= 1) && defined(__STDC_VERSION__)
	#define MYYAML_STDC_VERSION __STDC_VERSION__
#else
	#define MYYAML_STDC_VERSION 0
#endif

/** C++ version */
#if defined(__cplusplus)
	#define MYYAML_CPP_VERSION __cplusplus
#else
  #define MYYAML_CPP_VERSION 0
#endif

/** compiler builtin check */
#ifndef MYYAML_HAS_BUILTIN
	#ifdef __has_builtin
    	#define MYYAML_HAS_BUILTIN(x) __has_builtin(x)
  	#else
    	#define MYYAML_HAS_BUILTIN(x) 0
  	#endif
#endif

/** compiler attribute check */
#ifndef MYYAML_HAS_ATTRIBUTE
  	#ifdef __has_attribute
    	#define MYYAML_HAS_ATTRIBUTE(x) __has_attribute(x)
  	#else
    	#define MYYAML_HAS_ATTRIBUTE(x) 0
  	#endif
#endif

/** compiler feature check */
#ifndef MYYAML_HAS_FEATURE
  	#ifdef __has_feature
    	#define MYYAML_HAS_FEATURE(x) __has_feature(x)
  	#else
    	#define MYYAML_HAS_FEATURE(x) 0
  	#endif
#endif

/** include check */
#ifndef MYYAML_HAS_INCLUDE
  	#ifdef __has_include
    	#define MYYAML_HAS_INCLUDE(x) __has_include(x)
  	#else
    	#define MYYAML_HAS_INCLUDE(x) 0
  	#endif
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Attributes
//-----------------------------------------------------------------------------

/**
 * @defgroup attribute Compiler Attributes
 * @{
 */

/** inline for compiler */
#ifndef MYYAML_INLINE
  	#if MYYAML_COMPILER_SINCE(MSVC, 12, 0, 0)
    	#define MYYAML_INLINE __forceinline
  	#elif MYYAML_COMPILER_IS(INTEL)
    	#define MYYAML_INLINE __inline
  	#elif MYYAML_HAS_ATTRIBUTE(always_inline) || MYYAML_COMPILER_SINCE(GCC, 4, 0, 0)
    	#define MYYAML_INLINE __inline__ __attribute__((always_inline))
  	#elif MYYAML_COMPILER_IS(CLANG) || MYYAML_COMPILER_IS(GCC)
    	#define MYYAML_INLINE __inline__
  	#elif defined(__cplusplus) || MYYAML_STDC_VERSION >= 199901L
    	#define MYYAML_INLINE inline
  	#else
    	#define MYYAML_INLINE
  	#endif
#endif

/** noinline for compiler */
#ifndef MYYAML_NOINLINE
  	#if MYYAML_COMPILER_SINCE(MSVC, 14, 0, 0)
    	#define MYYAML_NOINLINE __declspec(noinline)
  	#elif MYYAML_HAS_ATTRIBUTE(noinline) || (MYYAML_COMPILER_SINCE(GCC, 4, 0, 0))
    	#define MYYAML_NOINLINE __attribute__((noinline))
  	#else
    	#define MYYAML_NOINLINE
  	#endif
#endif

/** align for compiler */
#ifndef MYYAML_ALIGN
  	#if MYYAML_COMPILER_SINCE(MSVC, 13, 0, 0)
    	#define MYYAML_ALIGN(x) __declspec(align(x))
  	#elif MYYAML_HAS_ATTRIBUTE(aligned) || defined(__GNUC__)
    	#define MYYAML_ALIGN(x) __attribute__((aligned(x)))
  	#elif MYYAML_CPP_VERSION >= 201103L
    	#define MYYAML_ALIGN(x) alignas(x)
  	#else
    	#define MYYAML_ALIGN(x)
  	#endif
#endif

/** likely for compiler */
#ifndef MYYAML_LIKELY
  	#if MYYAML_HAS_BUILTIN(__builtin_expect) || (MYYAML_COMPILER_SINCE(GCC, 4, 0, 0) && MYYAML_COMPILER_VERSION_MAJOR != 5)
    	#define MYYAML_LIKELY(expr) __builtin_expect(!!(expr), 1)
  	#else
    	#define MYYAML_LIKELY(expr) (expr)
  	#endif
#endif

/** unlikely for compiler */
#ifndef MYYAML_UNLIKELY
  	#if MYYAML_HAS_BUILTIN(__builtin_expect) || (MYYAML_COMPILER_SINCE(GCC, 4, 0, 0) && MYYAML_COMPILER_VERSION_MAJOR != 5)
    	#define MYYAML_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
  	#else
    	#define MYYAML_UNLIKELY(expr) (expr)
  	#endif
#endif

/** compile-time constant check for compiler */
#ifndef MYYAML_CONSTANT_P
  	#if MYYAML_HAS_BUILTIN(__builtin_constant_p) || (MYYAML_COMPILER_SINCE(GCC, 3, 0, 0))
    	#define MYYAML_HAS_CONSTANT_P 1
    	#define MYYAML_CONSTANT_P(value) __builtin_constant_p(value)
  	#else
    	#define MYYAML_HAS_CONSTANT_P 0
    	#define MYYAML_CONSTANT_P(value) 0
  	#endif
#endif

/** deprecate warning */
#ifndef MYYAML_DEPRECATED
  	#if MYYAML_COMPILER_SINCE(MSVC, 14, 0, 0)
    	#define MYYAML_DEPRECATED(msg) __declspec(deprecated(msg))
  	#elif MYYAML_HAS_FEATURE(attribute_deprecated_with_message) || \
    (MYYAML_COMPILER_SINCE(GCC, 4, 0, 0) ||                    \
     (MYYAML_COMPILER_VERSION_MAJOR == 5 &&                    \
      MYYAML_COMPILER_VERSION_MINOR >= 5))
    	#define MYYAML_DEPRECATED(msg) __attribute__((deprecated(msg)))
  	#elif MYYAML_COMPILER_SINCE(MSVC, 3, 0, 0)
    	#define MYYAML_DEPRECATED(msg) __attribute__((deprecated))
  	#else
    	#define MYYAML_DEPRECATED(msg)
  	#endif
#endif

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

/** inline function export */
#ifndef MYYAML_API_INLINE
  	#define MYYAML_API_INLINE static MYYAML_INLINE
#endif

/** char bit check */
#if defined(CHAR_BIT)
  	#if CHAR_BIT != 8
    	#error non 8-bit char is not supported
  	#endif
#endif

/** @} */

/* Strict C compiler warning helpers */

#if MYYAML_COMPILER_IS(CLANG) || MYYAML_COMPILER_IS(GCC)
  	#define HASATTRIBUTE_UNUSED
#endif

#ifdef HASATTRIBUTE_UNUSED
  	#define MYYAML_UNUSED __attribute__((__unused__))
#else
  	#define MYYAML_UNUSED
#endif

/* Shim arguments are arguments that must be included in your function,
 * but serve no purpose inside.  Silence compiler warnings. */
#define SHIM(a) /*@unused@*/ a MYYAML_UNUSED

/* UNUSED_PARAM() marks a shim argument in the body to silence compiler warnings
 */
#ifdef __clang__
  	#define UNUSED_PARAM(a) (void)(a);
#else
  	#define UNUSED_PARAM(a) if (0) (void)(a);
#endif

// clang-format on

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

/**
 * @defgroup basic Basic Types
 * @brief Core types and data structures for YAML.
 * @{
 */

/** The character type (UTF-8 octet). */
typedef unsigned char YamlChar_t;

/** An element of a sequence node. */
typedef int YamlNodeItem;

/** The version directive data. */
typedef struct YamlVersionDirective
{
	int major; /** The major version number. */
	int minor; /** The minor version number. */

} YamlVersionDirective;

/** The tag directive data. */
typedef struct YamlTagDirective
{
	YamlChar_t *handle; /** The tag handle. */
	YamlChar_t *prefix; /** The tag prefix. */

} YamlTagDirective;

/** The stream encoding. */
typedef enum YamlEncoding
{
	YAML_ANY_ENCODING,	   /** Let the parser choose the encoding. */
	YAML_UTF8_ENCODING,	   /** The default UTF-8 encoding. */
	YAML_UTF16LE_ENCODING, /** The UTF-16-LE encoding with BOM. */
	YAML_UTF16BE_ENCODING  /** The UTF-16-BE encoding with BOM. */

} YamlEncoding;

/** Line break types. */

typedef enum YamlBreakType
{
	YAML_ANY_BREAK, /** Let the parser choose the break type. */
	YAML_CR_BREAK,	/** Use CR for line breaks (Mac style). */
	YAML_LN_BREAK,	/** Use LN for line breaks (Unix style). */
	YAML_CRLN_BREAK /** Use CR LN for line breaks (DOS style). */

} YamlBreakType;

/** Many bad things could happen with the parser and emitter. */
typedef enum YamlErrorType
{
	YAML_NO_ERROR,		 /** No error is produced. */
	YAML_MEMORY_ERROR,	 /** Cannot allocate or reallocate a block of memory. */
	YAML_READER_ERROR,	 /** Cannot read or decode the input stream. */
	YAML_SCANNER_ERROR,	 /** Cannot scan the input stream. */
	YAML_PARSER_ERROR,	 /** Cannot parse the input stream. */
	YAML_COMPOSER_ERROR, /** Cannot compose a YAML document. */
	YAML_WRITER_ERROR,	 /** Cannot write to the output stream. */
	YAML_EMITTER_ERROR,	 /** Cannot emit a YAML stream. */
	YAML_ENCODING_ERROR,
	YAML_TYPE_ERROR,
	YAML_TAG_ERROR,

} YamlErrorType;

typedef struct YamlError_t
{
	YamlErrorType type;	 /**< Type of error. */
	const char *message; /**< Error message string. */
} YamlError_t;

/** The pointer position. */
typedef struct YamlMark
{
	size_t column; /** The position column. */
	size_t index;  /** The position index. */
	size_t line;   /** The position line. */

} YamlMark;

/**
 * @defgroup styles Node Styles
 * @{
 */

/** Scalar styles. */
typedef enum YamlScalarStyle
{
	YAML_ANY_SCALAR_STYLE,			 /** Let the emitter choose the style. */
	YAML_PLAIN_SCALAR_STYLE,		 /** The plain scalar style. */
	YAML_SINGLE_QUOTED_SCALAR_STYLE, /** The single-quoted scalar style. */
	YAML_DOUBLE_QUOTED_SCALAR_STYLE, /** The double-quoted scalar style. */
	YAML_LITERAL_SCALAR_STYLE,		 /** The literal scalar style. */
	YAML_FOLDED_SCALAR_STYLE		 /** The folded scalar style. */

} YamlScalarStyle;

/** Sequence styles. */
typedef enum YamlSequenceStyle
{
	YAML_ANY_SEQUENCE_STYLE,   /** Let the emitter choose the style. */
	YAML_BLOCK_SEQUENCE_STYLE, /** The block sequence style. */
	YAML_FLOW_SEQUENCE_STYLE   /** The flow sequence style. */

} YamlSequenceStyle;

/** Mapping styles. */
typedef enum YamlMappingStyle
{
	YAML_BLOCK_MAPPING_STYLE, /** The block mapping style. */
	YAML_FLOW_MAPPING_STYLE,  /** The flow mapping style. */
	YAML_ANY_MAPPING_STYLE,	  /** Let the emitter choose the style. */

} YamlMappingStyle;

/** @} */

/**
 * @defgroup tokens Tokens
 * @{
 */

/** Token types. */
typedef enum YamlTokenType
{
	YAML_NO_TOKEN,					 /** An empty token. */
	YAML_STREAM_START_TOKEN,		 /** A STREAM-START token. */
	YAML_STREAM_END_TOKEN,			 /** A STREAM-END token. */
	YAML_VERSION_DIRECTIVE_TOKEN,	 /** A VERSION-DIRECTIVE token. */
	YAML_TAG_DIRECTIVE_TOKEN,		 /** A TAG-DIRECTIVE token. */
	YAML_DOCUMENT_START_TOKEN,		 /** A DOCUMENT-START token. */
	YAML_DOCUMENT_END_TOKEN,		 /** A DOCUMENT-END token. */
	YAML_BLOCK_SEQUENCE_START_TOKEN, /** A BLOCK-SEQUENCE-START token. */
	YAML_BLOCK_MAPPING_START_TOKEN,	 /** A BLOCK-MAPPING-START token. */
	YAML_BLOCK_END_TOKEN,			 /** A BLOCK-END token. */
	YAML_FLOW_SEQUENCE_START_TOKEN,	 /** A FLOW-SEQUENCE-START token. */
	YAML_FLOW_SEQUENCE_END_TOKEN,	 /** A FLOW-SEQUENCE-END token. */
	YAML_FLOW_MAPPING_START_TOKEN,	 /** A FLOW-MAPPING-START token. */
	YAML_FLOW_MAPPING_END_TOKEN,	 /** A FLOW-MAPPING-END token. */
	YAML_BLOCK_ENTRY_TOKEN,			 /** A BLOCK-ENTRY token. */
	YAML_FLOW_ENTRY_TOKEN,			 /** A FLOW-ENTRY token. */
	YAML_KEY_TOKEN,					 /** A KEY token. */
	YAML_VALUE_TOKEN,				 /** A VALUE token. */
	YAML_ALIAS_TOKEN,				 /** An ALIAS token. */
	YAML_ANCHOR_TOKEN,				 /** An ANCHOR token. */
	YAML_TAG_TOKEN,					 /** A TAG token. */
	YAML_SCALAR_TOKEN				 /** A SCALAR token. */

} YamlTokenType;

/** The token structure. */
typedef struct YamlToken
{
	YamlTokenType type; /** The token type. */

	/** The token data. */
	union
	{
		/** The stream start (for @c YAML_STREAM_START_TOKEN). */
		struct
		{
			/** The stream encoding. */
			YamlEncoding encoding;
		} stream_start;

		/** The alias (for @c YAML_ALIAS_TOKEN). */
		struct
		{
			YamlChar_t *value; /** The alias value. */
		} alias;

		/** The anchor (for @c YAML_ANCHOR_TOKEN). */
		struct
		{
			YamlChar_t *value; /** The anchor value. */
		} anchor;

		/** The tag (for @c YAML_TAG_TOKEN). */
		struct
		{
			YamlChar_t *handle; /** The tag handle. */
			YamlChar_t *suffix; /** The tag suffix. */
		} tag;

		/** The scalar value (for @c YAML_SCALAR_TOKEN). */
		struct
		{
			YamlChar_t *value;	   /** The scalar value. */
			size_t length;		   /** The length of the scalar value. */
			YamlScalarStyle style; /** The scalar style. */
		} scalar;

		/** The version directive (for @c YAML_VERSION_DIRECTIVE_TOKEN). */
		struct
		{
			int major; /** The major version number. */
			int minor; /** The minor version number. */

		} version_directive;

		/** The tag directive (for @c YAML_TAG_DIRECTIVE_TOKEN). */
		struct
		{
			YamlChar_t *handle; /** The tag handle. */
			YamlChar_t *prefix; /** The tag prefix. */

		} tag_directive;

	} data;

	YamlMark start_mark; /** The beginning of the token. */
	YamlMark end_mark;	 /** The end of the token. */

} YamlToken;

/**
 * @defgroup events Events
 * @{
 */

/** Event types. */
typedef enum YamlEventType
{
	YAML_NO_EVENT,			   /** An empty event. */
	YAML_STREAM_START_EVENT,   /** A STREAM-START event. */
	YAML_STREAM_END_EVENT,	   /** A STREAM-END event. */
	YAML_DOCUMENT_START_EVENT, /** A DOCUMENT-START event. */
	YAML_DOCUMENT_END_EVENT,   /** A DOCUMENT-END event. */
	YAML_ALIAS_EVENT,		   /** An ALIAS event. */
	YAML_SCALAR_EVENT,		   /** A SCALAR event. */
	YAML_SEQUENCE_START_EVENT, /** A SEQUENCE-START event. */
	YAML_SEQUENCE_END_EVENT,   /** A SEQUENCE-END event. */
	YAML_MAPPING_START_EVENT,  /** A MAPPING-START event. */
	YAML_MAPPING_END_EVENT	   /** A MAPPING-END event. */

} YamlEventType;

/** The event structure. */
typedef struct YamlEvent
{
	YamlEventType type; /** The event type. */

	/** The event data. */
	union
	{
		/** The stream parameters (for @c YAML_STREAM_START_EVENT). */
		struct
		{
			/** The document encoding. */
			YamlEncoding encoding;
		} stream_start;

		/** The document parameters (for @c YAML_DOCUMENT_START_EVENT). */
		struct
		{
			YamlVersionDirective *version_directive; /** The version directive. */

			/** The list of tag directives. */
			struct
			{
				YamlTagDirective
					*start;			   /** The beginning of the tag directives list. */
				YamlTagDirective *end; /** The end of the tag directives list. */

			} tag_directives;

			int implicit; /** Is the document indicator implicit? */

		} document_start;

		/** The document end parameters (for @c YAML_DOCUMENT_END_EVENT). */
		struct
		{
			int implicit; /** Is the document end indicator implicit? */

		} document_end;

		/** The alias parameters (for @c YAML_ALIAS_EVENT). */
		struct
		{
			YamlChar_t *anchor; /** The anchor. */

		} alias;

		/** The scalar parameters (for @c YAML_SCALAR_EVENT). */
		struct
		{
			YamlScalarStyle style; /** The scalar style. */
			int quoted_implicit;   /** Is the tag optional for any non-plain
									  style? */
			int plain_implicit;	   /** Is the tag optional for the plain style? */
			YamlChar_t *anchor;	   /** The anchor. */
			YamlChar_t *value;	   /** The scalar value. */
			YamlChar_t *tag;	   /** The tag. */
			size_t length;		   /** The length of the scalar value. */

		} scalar;

		/** The sequence parameters (for @c YAML_SEQUENCE_START_EVENT). */
		struct
		{
			YamlSequenceStyle style; /** The sequence style. */
			YamlChar_t *anchor;		 /** The anchor. */
			YamlChar_t *tag;		 /** The tag. */
			int implicit;			 /** Is the tag optional? */

		} sequence_start;

		/** The mapping parameters (for @c YAML_MAPPING_START_EVENT). */
		struct
		{
			YamlMappingStyle style; /** The mapping style. */
			YamlChar_t *anchor;		/** The anchor. */
			YamlChar_t *tag;		/** The tag. */
			int implicit;			/** Is the tag optional? */
		} mapping_start;

	} data;

	YamlMark start_mark; /** The beginning of the event. */
	YamlMark end_mark;	 /** The end of the event. */

} YamlEvent;

/** Node types. */
typedef enum YamlNodeType
{
	YAML_NO_NODE,		/** An empty node. */
	YAML_SCALAR_NODE,	/** A scalar node. */
	YAML_SEQUENCE_NODE, /** A sequence node. */
	YAML_MAPPING_NODE	/** A mapping node. */

} YamlNodeType;

/** An element of a mapping node. */
typedef struct YamlNodePair
{
	int value; /** The value of the element. */
	int key;   /** The key of the element. */
} YamlNodePair;

/** The node structure. */
typedef struct YamlNode
{
	YamlNodeType type; /** The node type. */
	YamlChar_t *tag;   /** The node tag. */

	/** The node data. */
	union
	{
		/** The scalar parameters (for @c YAML_SCALAR_NODE). */
		struct
		{
			YamlScalarStyle style; /** The scalar style. */
			YamlChar_t *value;	   /** The scalar value. */
			size_t length;		   /** The length of the scalar value. */

		} scalar;

		/** The sequence parameters (for @c YAML_SEQUENCE_NODE). */
		struct
		{
			/** The stack of sequence items. */
			struct
			{
				YamlNodeItem *start; /** The beginning of the stack. */
				YamlNodeItem *end;	 /** The end of the stack. */
				YamlNodeItem *top;	 /** The top of the stack. */

			} items;
			/** The sequence style. */
			YamlSequenceStyle style;
		} sequence;

		/** The mapping parameters (for @c YAML_MAPPING_NODE). */
		struct
		{
			/** The stack of mapping pairs (key, value). */
			struct
			{
				YamlNodePair *start; /** The beginning of the stack. */
				YamlNodePair *end;	 /** The end of the stack. */
				YamlNodePair *top;	 /** The top of the stack. */

			} pairs;

			YamlMappingStyle style; /** The mapping style. */

		} mapping;

	} data;

	YamlMark start_mark; /** The beginning of the node. */
	YamlMark end_mark;	 /** The end of the node. */

} YamlNode;

/** The document structure. */
typedef struct YamlDocument
{
	YamlVersionDirective *version_directive; /** The version directive. */

	/** The document nodes. */
	struct
	{
		YamlNode *start; /** The beginning of the stack. */
		YamlNode *end;	 /** The end of the stack. */
		YamlNode *top;	 /** The top of the stack. */

	} nodes;

	/** The list of tag directives. */
	struct
	{
		YamlTagDirective *start; /** The beginning of the tag directives list. */
		YamlTagDirective *end;	 /** The end of the tag directives list. */

	} tag_directives;

	int start_implicit; /** Is the document start indicator implicit? */
	int end_implicit;	/** Is the document end indicator implicit? */

	YamlMark start_mark; /** The beginning of the document. */
	YamlMark end_mark;	 /** The end of the document. */

} YamlDocument;

#if !defined(MYYAML_DISABLE_READER) || !MYYAML_DISABLE_READER

typedef int YamlReadHandler(void *data, unsigned char *buffer, size_t size,
							size_t *size_read);

/**
 * This structure holds information about a potential simple key.
 */
typedef struct YamlSimpleKey
{
	size_t token_number; /** The number of the token. */
	YamlMark mark;		 /** The position mark. */
	int possible;		 /** Is a simple key possible? */
	int required;		 /** Is a simple key required? */

} YamlSimpleKey;

/**
 * This structure holds aliases data.
 */
typedef struct YamlAliasData
{
	YamlChar_t *anchor; /** The anchor. */
	YamlMark mark;		/** The anchor mark. */
	int index;			/** The node id. */

} YamlAliasData;

/**
 * The states of the parser.
 */
typedef enum YamlParserState
{
	// clang-format off

	YAML_PARSE_STREAM_START_STATE,						/** Expect STREAM-START. */
	YAML_PARSE_IMPLICIT_DOCUMENT_START_STATE,			/** Expect the beginning of an implicit document. */
	YAML_PARSE_DOCUMENT_START_STATE,					/** Expect DOCUMENT-START. */
	YAML_PARSE_DOCUMENT_CONTENT_STATE,					/** Expect the content of a document. */
	YAML_PARSE_DOCUMENT_END_STATE,						/** Expect DOCUMENT-END. */
	YAML_PARSE_BLOCK_NODE_STATE,						/** Expect a block node. */
	YAML_PARSE_BLOCK_NODE_OR_INDENTLESS_SEQUENCE_STATE, /** Expect a block node or indentless sequence. */
	YAML_PARSE_FLOW_NODE_STATE,							/** Expect a flow node. */
	YAML_PARSE_BLOCK_SEQUENCE_FIRST_ENTRY_STATE,		/** Expect the first entry of a block sequence. */
	YAML_PARSE_BLOCK_SEQUENCE_ENTRY_STATE,				/** Expect an entry of a block sequence. */
	YAML_PARSE_INDENTLESS_SEQUENCE_ENTRY_STATE,			/** Expect an entry of an indentless sequence. */
	YAML_PARSE_BLOCK_MAPPING_FIRST_KEY_STATE,			/** Expect the first key of a block mapping. */
	YAML_PARSE_BLOCK_MAPPING_KEY_STATE,					/** Expect a block mapping key. */
	YAML_PARSE_BLOCK_MAPPING_VALUE_STATE,				/** Expect a block mapping value. */
	YAML_PARSE_FLOW_SEQUENCE_FIRST_ENTRY_STATE,			/** Expect the first entry of a flow sequence. */
	YAML_PARSE_FLOW_SEQUENCE_ENTRY_STATE,				/** Expect an entry of a flow sequence. */
	YAML_PARSE_FLOW_SEQUENCE_ENTRY_MAPPING_KEY_STATE,	/** Expect a key of an ordered mapping. */
	YAML_PARSE_FLOW_SEQUENCE_ENTRY_MAPPING_VALUE_STATE, /** Expect a value of an ordered mapping. */
	YAML_PARSE_FLOW_SEQUENCE_ENTRY_MAPPING_END_STATE,	/** Expect the and of an ordered mapping entry. */
	YAML_PARSE_FLOW_MAPPING_FIRST_KEY_STATE,			/** Expect the first key of a flow mapping. */
	YAML_PARSE_FLOW_MAPPING_KEY_STATE,					/** Expect a key of a flow mapping. */
	YAML_PARSE_FLOW_MAPPING_VALUE_STATE,				/** Expect a value of a flow mapping. */
	YAML_PARSE_FLOW_MAPPING_EMPTY_VALUE_STATE,			/** Expect an empty value of a flow mapping. */
	YAML_PARSE_END_STATE								/** Expect nothing. */

	// clang-format on

} YamlParserState;

/**
 * The parser structure.
 *
 * All members are internal.  Manage the structure using the @c yaml_parser_
 * family of functions.
 */
typedef struct YamlParser
{
	/**
	 * @name Error handling
	 * @{
	 */

	YamlMark problem_mark; /** The problem position. */
	size_t problem_offset; /** The byte about which the problem occurred. */
	YamlMark context_mark; /** The context position. */
	YamlErrorType error;   /** Error type. */
	const char *problem;   /** Error description. */
	const char *context;   /** The error context. */
	int problem_value;	   /** The problematic value (@c -1 is none). */

	/**
	 * @}
	 */

	/**
	 * @name Reader stuff
	 * @{
	 */

	YamlReadHandler *read_handler; /** Read handler. */
	void *read_handler_data;	   /** A pointer for passing to the read handler. */

	/** Standard (string or file) input data. */
	union
	{
		/** String input data. */
		struct
		{
			const unsigned char *current; /** The string current position. */
			const unsigned char *start;	  /** The string start pointer. */
			const unsigned char *end;	  /** The string end pointer. */

		} string;

		FILE *file; /** File input data. */

	} input;

	int eof; /** EOF flag */

	/** The working buffer. */
	struct
	{
		YamlChar_t *pointer; /** The current position of the buffer. */
		YamlChar_t *start;	 /** The beginning of the buffer. */
		YamlChar_t *last;	 /** The last filled position of the buffer. */
		YamlChar_t *end;	 /** The end of the buffer. */

	} buffer;

	size_t unread; /**< The number of unread characters in the buffer. */

	/** The raw buffer. */
	struct
	{
		unsigned char *pointer; /** The current position of the buffer. */
		unsigned char *start;	/** The beginning of the buffer. */
		unsigned char *last;	/** The last filled position of the buffer. */
		unsigned char *end;		/** The end of the buffer. */

	} raw_buffer;

	YamlEncoding encoding; /** The input encoding. */
	size_t offset;		   /** The offset of the current position (in bytes). */
	YamlMark mark;		   /** The mark of the current position. */

	/**
	 * @}
	 */

	/**
	 * @name Scanner stuff
	 * @{
	 */

	int stream_start_produced; /** Have we started to scan the input stream? */
	int stream_end_produced;   /** Have we reached the end of the input stream? */
	int flow_level;			   /** The number of unclosed '[' and '{' indicators. */

	/** The tokens queue. */
	struct
	{
		YamlToken *start; /** The beginning of the tokens queue. */
		YamlToken *head;  /** The head of the tokens queue. */
		YamlToken *tail;  /** The tail of the tokens queue. */
		YamlToken *end;	  /** The end of the tokens queue. */

	} tokens;

	size_t tokens_parsed; /** The number of tokens fetched from the queue. */
	int token_available;  /** Does the tokens queue contain a token ready for
							 dequeueing. */

	/** The indentation levels stack. */
	struct
	{
		int *start; /** The beginning of the stack. */
		int *end;	/** The end of the stack. */
		int *top;	/** The top of the stack. */

	} indents;

	int simple_key_allowed; /** May a simple key occur at the current position?
							 */
	int indent;				/** The current indentation level. */

	/** The stack of simple keys. */
	struct
	{
		YamlSimpleKey *start; /** The beginning of the stack. */
		YamlSimpleKey *end;	  /** The end of the stack. */
		YamlSimpleKey *top;	  /** The top of the stack. */

	} simple_keys;

	/**
	 * @}
	 */

	/**
	 * @name Parser stuff
	 * @{
	 */

	/** The parser states stack. */
	struct
	{
		YamlParserState *start; /** The beginning of the stack. */
		YamlParserState *end;	/** The end of the stack. */
		YamlParserState *top;	/** The top of the stack. */

	} states;

	YamlParserState state; /** The current parser state. */

	/** The stack of marks. */
	struct
	{
		YamlMark *start; /** The beginning of the stack. */
		YamlMark *end;	 /** The end of the stack. */
		YamlMark *top;	 /** The top of the stack. */

	} marks;

	/** The list of TAG directives. */
	struct
	{
		YamlTagDirective *start; /** The beginning of the list. */
		YamlTagDirective *end;	 /** The end of the list. */
		YamlTagDirective *top;	 /** The top of the list. */

	} tag_directives;

	/**
	 * @}
	 */

	/**
	 * @name Dumper stuff
	 * @{
	 */

	/** The alias data. */
	struct
	{
		YamlAliasData *start; /** The beginning of the list. */
		YamlAliasData *end;	  /** The end of the list. */
		YamlAliasData *top;	  /** The top of the list. */

	} aliases;

	YamlDocument *document; /** The currently parsed document. */

	/**
	 * @}
	 */

} YamlParser;

#endif // MYYAML_DISABLE_READER

#if !defined(MYYAML_DISABLE_WRITER) || !MYYAML_DISABLE_WRITER

typedef int YamlWriteHandler(void *data, unsigned char *buffer, size_t size);

/* This is needed for C++ */

typedef struct YamlAnchors
{
	int references; /** The number of references. */
	int serialized; /** If the node has been emitted? */
	int anchor;		/** The anchor id. */

} YamlAnchors;

/** The emitter states. */
typedef enum YamlEmitterState
{

	// clang-format off

	YAML_EMIT_STREAM_START_STATE,				/** Expect STREAM-START. */
	YAML_EMIT_FIRST_DOCUMENT_START_STATE,		/** Expect the first DOCUMENT-START or STREAM-END. */
	YAML_EMIT_DOCUMENT_START_STATE,				/** Expect DOCUMENT-START or STREAM-END. */
	YAML_EMIT_DOCUMENT_CONTENT_STATE,			/** Expect the content of a document. */
	YAML_EMIT_DOCUMENT_END_STATE,				/** Expect DOCUMENT-END. */
	YAML_EMIT_FLOW_SEQUENCE_FIRST_ITEM_STATE,	/** Expect the first item of a flow sequence. */
	YAML_EMIT_FLOW_SEQUENCE_ITEM_STATE,			/** Expect an item of a flow sequence. */
	YAML_EMIT_FLOW_MAPPING_FIRST_KEY_STATE,		/** Expect the first key of a flow mapping. */
	YAML_EMIT_FLOW_MAPPING_KEY_STATE,			/** Expect a key of a flow mapping. */
	YAML_EMIT_FLOW_MAPPING_SIMPLE_VALUE_STATE,	/** Expect a value for a simple key of a flow mapping. */
	YAML_EMIT_FLOW_MAPPING_VALUE_STATE,			/** Expect a value of a flow mapping. */
	YAML_EMIT_BLOCK_SEQUENCE_FIRST_ITEM_STATE,	/** Expect the first item of a block sequence. */
	YAML_EMIT_BLOCK_SEQUENCE_ITEM_STATE,		/** Expect an item of a block sequence. */
	YAML_EMIT_BLOCK_MAPPING_FIRST_KEY_STATE,	/** Expect the first key of a block mapping. */
	YAML_EMIT_BLOCK_MAPPING_KEY_STATE,			/** Expect the key of a block mapping. */
	YAML_EMIT_BLOCK_MAPPING_SIMPLE_VALUE_STATE, /** Expect a value for a simple key of a block mapping. */
	YAML_EMIT_BLOCK_MAPPING_VALUE_STATE,		/** Expect a value of a block mapping. */
	YAML_EMIT_END_STATE							/** Expect nothing. */

	// clang-format on

} YamlEmitterState;

/**
 * The emitter structure.
 *
 * All members are internal.  Manage the structure using the @c yaml_emitter_
 * family of functions.
 */

typedef struct YamlEmitter
{
	/**
	 * @name Error handling
	 * @{
	 */

	YamlErrorType error; /** Error type. */
	const char *problem; /** Error description. */

	/**
	 * @}
	 */

	/**
	 * @name Writer stuff
	 * @{
	 */

	YamlWriteHandler *write_handler; /** Write handler. */
	void *write_handler_data;		 /** A pointer for passing to the write handler. */

	/** Standard (string or file) output data. */
	union
	{
		/** String output data. */
		struct
		{
			unsigned char *buffer; /** The buffer pointer. */
			size_t *size_written;  /** The number of written bytes. */
			size_t size;		   /** The buffer size. */

		} string;
		FILE *file; /** File output data. */

	} output;

	/** The working buffer. */
	struct
	{
		YamlChar_t *pointer; /** The current position of the buffer. */
		YamlChar_t *start;	 /** The beginning of the buffer. */
		YamlChar_t *last;	 /** The last filled position of the buffer. */
		YamlChar_t *end;	 /** The end of the buffer. */

	} buffer;

	/** The raw buffer. */
	struct
	{
		unsigned char *pointer; /** The current position of the buffer. */
		unsigned char *start;	/** The beginning of the buffer. */
		unsigned char *last;	/** The last filled position of the buffer. */
		unsigned char *end;		/** The end of the buffer. */

	} raw_buffer;

	YamlEncoding encoding; /** The stream encoding. */

	/**
	 * @}
	 */

	/**
	 * @name Emitter stuff
	 * @{
	 */

	YamlBreakType line_break; /** The preferred line break. */
	int best_indent;		  /** The number of indentation spaces. */
	int best_width;			  /** The preferred width of the output lines. */
	int canonical;			  /** If the output is in the canonical style? */
	int unicode;			  /** Allow unescaped non-ASCII characters? */

	/** The stack of states. */
	struct
	{
		YamlEmitterState *start; /** The beginning of the stack. */
		YamlEmitterState *end;	 /** The end of the stack. */
		YamlEmitterState *top;	 /** The top of the stack. */

	} states;

	YamlEmitterState state; /** The current emitter state. */

	/** The event queue. */
	struct
	{
		YamlEvent *start; /** The beginning of the event queue. */
		YamlEvent *head;  /** The head of the event queue. */
		YamlEvent *tail;  /** The tail of the event queue. */
		YamlEvent *end;	  /** The end of the event queue. */

	} events;

	/** The stack of indentation levels. */
	struct
	{
		int *start; /** The beginning of the stack. */
		int *end;	/** The end of the stack. */
		int *top;	/** The top of the stack. */

	} indents;

	/** The list of tag directives. */
	struct
	{
		YamlTagDirective *start; /** The beginning of the list. */
		YamlTagDirective *end;	 /** The end of the list. */
		YamlTagDirective *top;	 /** The top of the list. */

	} tag_directives;

	int flow_level;			/** The current flow level. */
	int indent;				/** The current indentation level. */
	int root_context;		/** Is it the document root context? */
	int sequence_context;	/** Is it a sequence context? */
	int mapping_context;	/** Is it a mapping context? */
	int simple_key_context; /** Is it a simple mapping key context? */
	int line;				/** The current line. */
	int column;				/** The current column. */
	int indention;			/** If the last character was an indentation character (' ',
							   '-', '?', ':')? */
	int open_ended;			/** If an explicit document end is required? */
	int whitespace;			/** If the last character was a whitespace? */

	/** Anchor analysis. */
	struct
	{
		size_t anchor_length; /** The anchor length. */
		YamlChar_t *anchor;	  /** The anchor value. */
		int alias;			  /** Is it an alias? */

	} anchor_data;

	/** Tag analysis. */
	struct
	{
		size_t suffix_length; /** The tag suffix length. */
		size_t handle_length; /** The tag handle length. */
		YamlChar_t *suffix;	  /** The tag suffix. */
		YamlChar_t *handle;	  /** The tag handle. */

	} tag_data;

	/** Scalar analysis. */
	struct
	{
		int single_quoted_allowed; /** Can the scalar be expressed in the single
									  quoted style? */
		int block_plain_allowed;   /** Can the scalar be expressed in the block
									  plain   style? */
		int flow_plain_allowed;	   /** Can the scalar be expressed in the flow
									  plain    style? */
		YamlScalarStyle style;	   /** The output style. */
		int block_allowed;		   /** Can the scalar be expressed in the literal or
									  folded styles? */
		YamlChar_t *value;		   /** The scalar value. */
		size_t length;			   /** The scalar length. */
		int multiline;			   /** Does the scalar contain line breaks? */

	} scalar_data;

	/**
	 * @}
	 */

	/**
	 * @name Dumper stuff
	 * @{
	 */

	YamlDocument *document; /** The currently emitted document. */
	YamlAnchors
		*anchors; /** The information associated with the document nodes. */

	int last_anchor_id; /** The last assigned anchor id. */
	int opened;			/** If the stream was already opened? */
	int closed;			/** If the stream was already closed? */

	/**
	 * @}
	 */

} YamlEmitter;

#endif // MYYAML_DISABLE_WRITER

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#pragma region C

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

	/**
	 * Set the maximum depth of nesting.
	 *
	 * Default: 1000
	 *
	 * Each nesting level increases the stack and the number of previous
	 * starting events that the parser has to check.
	 *
	 * @param[in]       max         The maximum number of allowed nested events
	 */
	MYYAML_API void yaml_set_max_nest_level(int max);

	/**
	 * Free any memory allocated for a token object.
	 *
	 * @param[in,out]   token   A token object.
	 */
	MYYAML_API void yaml_token_delete(YamlToken *token);

	/*
	 * Check if a string is a valid UTF-8 sequence.
	 */
	MYYAML_API int yaml_check_utf8(const YamlChar_t *start, size_t length);

#pragma region Event

	/**
	 * Create the STREAM-START event.
	 *
	 * @param[out]      event       An empty event object.
	 * @param[in]       encoding    The stream encoding.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_stream_start(YamlEvent *event,
													  YamlEncoding encoding);

	/**
	 * Create the STREAM-END event.
	 *
	 * @param[out]      event       An empty event object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_stream_end(YamlEvent *event);

	/**
	 * Create the DOCUMENT-START event.
	 *
	 * The @a implicit argument is considered as a stylistic parameter and may be
	 * ignored by the emitter.
	 *
	 * @param[out]      event                   An empty event object.
	 * @param[in]       version_directive       The %YAML directive value or
	 *                                          @c NULL.
	 * @param[in]       tag_directives_start    The beginning of the %TAG
	 *                                          directives list.
	 * @param[in]       tag_directives_end      The end of the %TAG directives
	 *                                          list.
	 * @param[in]       implicit                If the document start indicator is
	 *                                          implicit.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_document_start(
		YamlEvent *event, YamlVersionDirective *version_directive,
		YamlTagDirective *tag_directives_start,
		YamlTagDirective *tag_directives_end, int implicit);

	/**
	 * Create the DOCUMENT-END event.
	 *
	 * The @a implicit argument is considered as a stylistic parameter and may be
	 * ignored by the emitter.
	 *
	 * @param[out]      event       An empty event object.
	 * @param[in]       implicit    If the document end indicator is implicit.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_document_end(YamlEvent *event,
													  int implicit);

	/**
	 * Create an ALIAS event.
	 *
	 * @param[out]      event       An empty event object.
	 * @param[in]       anchor      The anchor value.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_alias(YamlEvent *event,
											   const YamlChar_t *anchor);

	/**
	 * Create a SCALAR event.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * Either the @a tag attribute or one of the @a plain_implicit and
	 * @a quoted_implicit flags must be set.
	 *
	 * @param[out]      event           An empty event object.
	 * @param[in]       anchor          The scalar anchor or @c NULL.
	 * @param[in]       tag             The scalar tag or @c NULL.
	 * @param[in]       value           The scalar value.
	 * @param[in]       length          The length of the scalar value.
	 * @param[in]       plain_implicit  If the tag may be omitted for the plain
	 *                                  style.
	 * @param[in]       quoted_implicit If the tag may be omitted for any
	 *                                  non-plain style.
	 * @param[in]       style           The scalar style.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_scalar(
		YamlEvent *event, const YamlChar_t *anchor, const YamlChar_t *tag,
		const YamlChar_t *value, int length, int plain_implicit,
		int quoted_implicit, YamlScalarStyle style);

	/**
	 * Create a SEQUENCE-START event.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * Either the @a tag attribute or the @a implicit flag must be set.
	 *
	 * @param[out]      event       An empty event object.
	 * @param[in]       anchor      The sequence anchor or @c NULL.
	 * @param[in]       tag         The sequence tag or @c NULL.
	 * @param[in]       implicit    If the tag may be omitted.
	 * @param[in]       style       The sequence style.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_sequence_start(YamlEvent *event,
														const YamlChar_t *anchor,
														const YamlChar_t *tag,
														int implicit,
														YamlSequenceStyle style);

	/**
	 * Create a SEQUENCE-END event.
	 *
	 * @param[out]      event       An empty event object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_sequence_end(YamlEvent *event);

	/**
	 * Create a MAPPING-START event.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * Either the @a tag attribute or the @a implicit flag must be set.
	 *
	 * @param[out]      event       An empty event object.
	 * @param[in]       anchor      The mapping anchor or @c NULL.
	 * @param[in]       tag         The mapping tag or @c NULL.
	 * @param[in]       implicit    If the tag may be omitted.
	 * @param[in]       style       The mapping style.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_mapping_start(YamlEvent *event,
													   const YamlChar_t *anchor,
													   const YamlChar_t *tag,
													   int implicit,
													   YamlMappingStyle style);

	/**
	 * Create a MAPPING-END event.
	 *
	 * @param[out]      event       An empty event object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_event_initialize_mapping_end(YamlEvent *event);

	/**
	 * Free any memory allocated for an event object.
	 *
	 * @param[in,out]   event   An event object.
	 */
	MYYAML_API void yaml_event_delete(YamlEvent *event);

#pragma endregion // Event

#pragma region Document

	/**
	 * Create a YAML document.
	 *
	 * @param[out]      document                An empty document object.
	 * @param[in]       version_directive       The %YAML directive value or
	 *                                          @c NULL.
	 * @param[in]       tag_directives_start    The beginning of the %TAG
	 *                                          directives list.
	 * @param[in]       tag_directives_end      The end of the %TAG directives
	 *                                          list.
	 * @param[in]       start_implicit          If the document start indicator is
	 *                                          implicit.
	 * @param[in]       end_implicit            If the document end indicator is
	 *                                          implicit.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_document_initialize(YamlDocument *document,
											YamlVersionDirective *version_directive,
											YamlTagDirective *tag_directives_start,
											YamlTagDirective *tag_directives_end,
											int start_implicit, int end_implicit);

	/**
	 * Delete a YAML document and all its nodes.
	 *
	 * @param[in,out]   document        A document object.
	 */
	MYYAML_API void yaml_document_delete(YamlDocument *document);

	/**
	 * Get the root of a YAML document node.
	 *
	 * The root object is the first object added to the document.
	 *
	 * The pointer returned by this function is valid until any of the functions
	 * modifying the documents are called.
	 *
	 * An empty document produced by the parser signifies the end of a YAML
	 * stream.
	 *
	 * @param[in]       document        A document object.
	 *
	 * @returns the node object or @c NULL if the document is empty.
	 */
	MYYAML_API YamlNode *yaml_document_get_root_node(YamlDocument *document);

	/**
	 * Get a node of a YAML document.
	 *
	 * The pointer returned by this function is valid until any of the functions
	 * modifying the documents are called.
	 *
	 * @param[in]       document        A document object.
	 * @param[in]       index           The node id.
	 *
	 * @returns the node objct or @c NULL if @c node_id is out of range.
	 */
	MYYAML_API YamlNode *yaml_document_get_node(YamlDocument *document, int index);

	/**
	 * Create a SCALAR node and attach it to the document.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * @param[in,out]   document        A document object.
	 * @param[in]       tag             The scalar tag.
	 * @param[in]       value           The scalar value.
	 * @param[in]       length          The length of the scalar value.
	 * @param[in]       style           The scalar style.
	 *
	 * @returns the node id or @c 0 on error.
	 */
	MYYAML_API int yaml_document_add_scalar(YamlDocument *document,
											const YamlChar_t *tag,
											const YamlChar_t *value, int length,
											YamlScalarStyle style);

	/**
	 * Create a SEQUENCE node and attach it to the document.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * @param[in,out]   document    A document object.
	 * @param[in]       tag         The sequence tag.
	 * @param[in]       style       The sequence style.
	 *
	 * @returns the node id or @c 0 on error.
	 */
	MYYAML_API int yaml_document_add_sequence(YamlDocument *document,
											  const YamlChar_t *tag,
											  YamlSequenceStyle style);

	/**
	 * Create a MAPPING node and attach it to the document.
	 *
	 * The @a style argument may be ignored by the emitter.
	 *
	 * @param[in,out]   document    A document object.
	 * @param[in]       tag         The sequence tag.
	 * @param[in]       style       The sequence style.
	 *
	 * @returns the node id or @c 0 on error.
	 */
	MYYAML_API int yaml_document_add_mapping(YamlDocument *document,
											 const YamlChar_t *tag,
											 YamlMappingStyle style);

	/**
	 * Add an item to a SEQUENCE node.
	 *
	 * @param[in,out]   document    A document object.
	 * @param[in]       sequence    The sequence node id.
	 * @param[in]       item        The item node id.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_document_append_sequence_item(YamlDocument *document,
													  int sequence, int item);

	/**
	 * Add a pair of a key and a value to a MAPPING node.
	 *
	 * @param[in,out]   document    A document object.
	 * @param[in]       mapping     The mapping node id.
	 * @param[in]       key         The key node id.
	 * @param[in]       value       The value node id.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_document_append_mapping_pair(YamlDocument *document,
													 int mapping, int key,
													 int value);

	/**
	 * Convenience: return pointer to scalar value for a node id.
	 * Returns NULL if node is not a scalar or out of range.
	 */
	MYYAML_API const YamlChar_t *yaml_document_get_scalar_value(
		YamlDocument *document, int node_id);

	/**
	 * Convenience: return length of scalar value for a node id.
	 * Returns -1 if node is not a scalar or out of range.
	 */
	MYYAML_API int yaml_document_get_scalar_length(YamlDocument *document,
												   int node_id);

	/**
	 * Convenience: get an item node id from a sequence node by zero-based index.
	 * Returns 0 on error (invalid sequence id or index out of range).
	 */
	MYYAML_API int yaml_document_sequence_get_item(YamlDocument *document,
												   int sequence_node_id, int index);

	/**
	 * Convenience: find a mapping value node id by scalar key string.
	 * The key is matched by exact byte equality. If key_length < 0 the key is
	 * treated as a NUL-terminated string and its length is computed with strlen.
	 * Returns the value node id on success or 0 if not found or on error.
	 */
	MYYAML_API int yaml_document_mapping_get_value(YamlDocument *document,
												   int mapping_node_id,
												   const YamlChar_t *key,
												   int key_length);

	/**
	 * Find a node by a path of keys. Keys are supplied as an array of NUL-
	 * terminated strings. For mapping nodes a key is matched against scalar
	 * keys (exact byte match). For sequence nodes a key that is a decimal
	 * integer ("0", "1", ...) is treated as a zero-based index. The
	 * function returns the node id (>0) on success or 0 on error/not found.
	 */
	MYYAML_API int yaml_document_get_node_by_path(YamlDocument *document,
												  const YamlChar_t **keys,
												  int key_count);

	/**
	 * Convenience: return scalar value pointer for node found by path of keys.
	 * Returns NULL if not found or the final node is not a scalar. Use
	 * yaml_document_get_scalar_length() to get the length.
	 */
	MYYAML_API const YamlChar_t *yaml_document_get_value_by_path(
		YamlDocument *document, const YamlChar_t **keys, int key_count);

	/**
	 * Convenience: return scalar length for node found by path of keys.
	 * Returns -1 if not found or the final node is not a scalar.
	 */
	MYYAML_API int yaml_document_get_value_length_by_path(YamlDocument *document,
														  const YamlChar_t **keys,
														  int key_count);

#pragma endregion // Document

#if !defined(MYYAML_DISABLE_READER) || !MYYAML_DISABLE_READER

#pragma region Reader

	/**
	 * Initialize a parser.
	 *
	 * This function creates a new parser object.  An application is responsible
	 * for destroying the object using the yaml_parser_delete() function.
	 *
	 * @param[out]      parser  An empty parser object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_parser_initialize(YamlParser *parser);

	/**
	 * Parse the input stream and produce the next parsing event.
	 *
	 * Call the function subsequently to produce a sequence of events corresponding
	 * to the input stream.  The initial event has the type
	 * @c YAML_STREAM_START_EVENT while the ending event has the type
	 * @c YAML_STREAM_END_EVENT.
	 *
	 * An application is responsible for freeing any buffers associated with the
	 * produced event object using the yaml_event_delete() function.
	 *
	 * An application must not alternate the calls of yaml_parser_parse() with the
	 * calls of yaml_parser_scan() or yaml_parser_load(). Doing this will break the
	 * parser.
	 *
	 * @param[in,out]   parser      A parser object.
	 * @param[out]      event       An empty event object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_parser_parse(YamlParser *parser, YamlEvent *event);

	/**
	 * Parse the input stream and produce the next YAML document.
	 *
	 * Call this function subsequently to produce a sequence of documents
	 * constituting the input stream.
	 *
	 * If the produced document has no root node, it means that the document
	 * end has been reached.
	 *
	 * An application is responsible for freeing any data associated with the
	 * produced document object using the yaml_document_delete() function.
	 *
	 * An application must not alternate the calls of yaml_parser_load() with the
	 * calls of yaml_parser_scan() or yaml_parser_parse(). Doing this will break
	 * the parser.
	 *
	 * @param[in,out]   parser      A parser object.
	 * @param[out]      document    An empty document object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_parser_load(YamlParser *parser, YamlDocument *document);

	/**
	 * Scan the input stream and produce the next token.
	 *
	 * Call the function subsequently to produce a sequence of tokens corresponding
	 * to the input stream.  The initial token has the type
	 * @c YAML_STREAM_START_TOKEN while the ending token has the type
	 * @c YAML_STREAM_END_TOKEN.
	 *
	 * An application is responsible for freeing any buffers associated with the
	 * produced token object using the @c yaml_token_delete function.
	 *
	 * An application must not alternate the calls of yaml_parser_scan() with the
	 * calls of yaml_parser_parse() or yaml_parser_load(). Doing this will break
	 * the parser.
	 *
	 * @param[in,out]   parser      A parser object.
	 * @param[out]      token       An empty token object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_parser_scan(YamlParser *parser, YamlToken *token);

	/**
	 * Destroy a parser.
	 *
	 * @param[in,out]   parser  A parser object.
	 */
	MYYAML_API void yaml_parser_delete(YamlParser *parser);

	/**
	 * Set a string input.
	 *
	 * Note that the @a input pointer must be valid while the @a parser object
	 * exists.  The application is responsible for destroying @a input after
	 * destroying the @a parser.
	 *
	 * @param[in,out]   parser  A parser object.
	 * @param[in]       input   A source data.
	 * @param[in]       size    The length of the source data in bytes.
	 */
	MYYAML_API void yaml_parser_set_input_string(YamlParser *parser,
												 const unsigned char *input,
												 size_t size);

	/**
	 * Set a file input.
	 *
	 * @a file should be a file object open for reading.  The application is
	 * responsible for closing the @a file.
	 *
	 * @param[in,out]   parser  A parser object.
	 * @param[in]       file    An open file.
	 */
	MYYAML_API void yaml_parser_set_input_file(YamlParser *parser, FILE *file);

	/**
	 * Set a generic input handler.
	 *
	 * @param[in,out]   parser  A parser object.
	 * @param[in]       handler A read handler.
	 * @param[in]       data    Any application data for passing to the read
	 *                          handler.
	 */
	MYYAML_API void yaml_parser_set_input(YamlParser *parser,
										  YamlReadHandler *handler, void *data);

	/**
	 * Set the source encoding.
	 *
	 * @param[in,out]   parser      A parser object.
	 * @param[in]       encoding    The source encoding.
	 */
	MYYAML_API void yaml_parser_set_encoding(YamlParser *parser,
											 YamlEncoding encoding);

#pragma endregion // Reader

#endif // MYYAML_DISABLE_READER

#if !defined(MYYAML_DISABLE_WRITER) || !MYYAML_DISABLE_WRITER

#pragma region Writer

	/**
	 * Initialize an emitter.
	 *
	 * This function creates a new emitter object.  An application is responsible
	 * for destroying the object using the yaml_emitter_delete() function.
	 *
	 * @param[out]      emitter     An empty parser object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_initialize(YamlEmitter *emitter);

	/**
	 * Emit an event.
	 *
	 * The event object may be generated using the yaml_parser_parse() function.
	 * The emitter takes the responsibility for the event object and destroys its
	 * content after it is emitted. The event object is destroyed even if the
	 * function fails.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in,out]   event       An event object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_emit(YamlEmitter *emitter, YamlEvent *event);

	/**
	 * Destroy an emitter.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 */
	MYYAML_API void yaml_emitter_delete(YamlEmitter *emitter);

	/**
	 * Emit a YAML document.
	 *
	 * The document object may be generated using the yaml_parser_load() function
	 * or the yaml_document_initialize() function.  The emitter takes the
	 * responsibility for the document object and destroys its content after
	 * it is emitted. The document object is destroyed even if the function fails.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in,out]   document    A document object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_dump(YamlEmitter *emitter, YamlDocument *document);

	/**
	 * Set a string output.
	 *
	 * The emitter will write the output characters to the @a output buffer of the
	 * size @a size.  The emitter will set @a size_written to the number of written
	 * bytes.  If the buffer is smaller than required, the emitter produces the
	 * YAML_WRITE_ERROR error.
	 *
	 * @param[in,out]   emitter         An emitter object.
	 * @param[in]       output          An output buffer.
	 * @param[in]       size            The buffer size.
	 * @param[in]       size_written    The pointer to save the number of written
	 *                                  bytes.
	 */
	MYYAML_API void yaml_emitter_set_output_string(YamlEmitter *emitter,
												   unsigned char *output,
												   size_t size,
												   size_t *size_written);

	/**
	 * Set a file output.
	 *
	 * @a file should be a file object open for writing.  The application is
	 * responsible for closing the @a file.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       file        An open file.
	 */
	MYYAML_API void yaml_emitter_set_output_file(YamlEmitter *emitter, FILE *file);

	/**
	 * Set a generic output handler.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       handler     A write handler.
	 * @param[in]       data        Any application data for passing to the write
	 *                              handler.
	 */
	MYYAML_API void yaml_emitter_set_output(YamlEmitter *emitter,
											YamlWriteHandler *handler, void *data);

	/**
	 * Set the output encoding.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       encoding    The output encoding.
	 */
	MYYAML_API void yaml_emitter_set_encoding(YamlEmitter *emitter,
											  YamlEncoding encoding);

	/**
	 * Set if the output should be in the "canonical" format as in the YAML
	 * specification.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       canonical   If the output is canonical.
	 */
	MYYAML_API void yaml_emitter_set_canonical(YamlEmitter *emitter, int canonical);

	/**
	 * Set the indentation increment.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       indent      The indentation increment (1 < . < 10).
	 */
	MYYAML_API void yaml_emitter_set_indent(YamlEmitter *emitter, int indent);

	/**
	 * Set the preferred line width. @c -1 means unlimited.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       width       The preferred line width.
	 */
	MYYAML_API void yaml_emitter_set_width(YamlEmitter *emitter, int width);

	/**
	 * Set if unescaped non-ASCII characters are allowed.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       unicode     If unescaped Unicode characters are allowed.
	 */
	MYYAML_API void yaml_emitter_set_unicode(YamlEmitter *emitter, int unicode);

	/**
	 * Set the preferred line break.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 * @param[in]       line_break  The preferred line break.
	 */
	MYYAML_API void yaml_emitter_set_break(YamlEmitter *emitter,
										   YamlBreakType line_break);

	/**
	 * Start a YAML stream.
	 *
	 * This function should be used before yaml_emitter_dump() is called.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_open(YamlEmitter *emitter);

	/**
	 * Finish a YAML stream.
	 *
	 * This function should be used after yaml_emitter_dump() is called.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_close(YamlEmitter *emitter);

	/**
	 * Flush the accumulated characters to the output.
	 *
	 * @param[in,out]   emitter     An emitter object.
	 *
	 * @returns @c 1 if the function succeeded, @c 0 on error.
	 */
	MYYAML_API int yaml_emitter_flush(YamlEmitter *emitter);

#pragma endregion // Writer

#endif // MYYAML_DISABLE_WRITER

#ifdef __cplusplus
};
#endif //__cplusplus

#pragma endregion // C

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#pragma region Cpp

#ifdef __cplusplus

namespace myyaml
{

#pragma region Exception

	/**
	 * @class YamlError
	 * @brief YamlError class for Yaml-related errors.
	 */
	class YamlError : public std::exception
	{
	public:
		/**
		 * @brief Constructs an exception with a specific error.
		 * @param error The type of the error.
		 */
		YamlError(YamlError_t error);

		/**
		 * @brief Gets the error message.
		 * @return The error message.
		 */
		const char *what() const noexcept override;

		/**
		 * @brief Gets the error type.
		 * @return The error type.
		 */
		YamlErrorType type() const noexcept;

	private:
		YamlError_t m_Error; /**< The error */
	};

	/** Errors that occur during usage
	 */
	class EncodingError : public YamlError
	{
	public:
		EncodingError(YamlError_t error) : YamlError(error) {};
	};

	class EmitterError : public YamlError
	{
	public:
		EmitterError(YamlError_t error) : YamlError(error) {};
	};

	class WriterError : public YamlError
	{
	public:
		WriterError(YamlError_t error) : YamlError(error) {};
	};

	class ScannerError : public YamlError
	{
	public:
		ScannerError(YamlError_t error) : YamlError(error) {};
	};

	class ReaderError : public YamlError
	{
	public:
		ReaderError(YamlError_t error) : YamlError(error) {};
	};

	class ParserError : public YamlError
	{
	public:
		ParserError(YamlError_t error) : YamlError(error) {};
	};

	class MemoryError : public YamlError
	{
	public:
		MemoryError(YamlError_t error) : YamlError(error) {};
	};

	class TypeError : public YamlError
	{
	public:
		TypeError(YamlError_t error) : YamlError(error) {};
	};

	class TagError : public YamlError
	{
	public:
		TagError(YamlError_t error) : YamlError(error) {};
	};

#pragma endregion // Exception

#pragma region Yaml

	// Basic type-traits helpers for template dispatch
	template <typename T>
	struct is_std_vector : std::false_type
	{
	};

	template <typename U, typename A>
	struct is_std_vector<std::vector<U, A>> : std::true_type
	{
		using value_type = U;
	};

	template <typename T>
	struct is_std_map : std::false_type
	{
	};

	template <typename K, typename V, typename C, typename A>
	struct is_std_map<std::map<K, V, C, A>> : std::true_type
	{
		using key_type = K;
		using mapped_type = V;
	};

	// A minimal YAML document class (string-backed) for simple use-cases.
	class yaml
	{
	public:
		using ValueType = YamlTokenType;
		using ValueNodeType = YamlNodeType;

	private:
		template <typename ObjectType, typename OutputType>
		friend class myyaml::Writer;

		template <typename ObjectType, typename InputType>
		friend class myyaml::Reader;

		template <typename Node>
		friend class myyaml::Serializer;

		template <typename Node>
		friend class myyaml::Deserializer;

		template <typename InputType>
		using ReaderType = myyaml::Reader<yaml, InputType>;

		template <typename OuputType>
		using WriterType = myyaml::Writer<yaml, OuputType>;

		/**
		 * @brief A type for YAML serializers.
		 */
		using SerializerType = myyaml::Serializer<yaml>;

		/**
		 * @brief A type for YAML deserializers.
		 */
		using DeserializerType = myyaml::Deserializer<yaml>;

	public:
		yaml() = default;
		explicit yaml(const std::string &s) : content_(s) {}

		void loadFromString(const std::string &s) { content_ = s; }
		const std::string &toString() const { return content_; }
		void clear() { content_.clear(); }

		// Append with optional newline
		void append(const std::string &s, bool newline = true)
		{
			content_ += s;
			if (newline)
				content_ += '\n';
		}

	private:
		std::string content_;
	};

#pragma endregion // Yaml

#if !defined(MYYAML_DISABLE_READER) || !MYYAML_DISABLE_READER

#pragma region Reader

	// Deserializer: parses the simple YAML produced by Serializer into C++ objects.
	template <typename Node>
	class MYYAML_API Deserializer
	{
	public:
		Deserializer() = default;

		static void from_yaml(const myyaml::yaml &doc, Node &out);

	private:
		// arithmetic
		template <typename T>
		static auto from_yaml_impl(const std::string &s, T &out, int)
			-> std::enable_if_t<std::is_integral<T>::value>
		{
			out = static_cast<T>(std::stoll(s));
		}

		template <typename T>
		static auto from_yaml_impl(const std::string &s, T &out, long)
			-> std::enable_if_t<std::is_floating_point<T>::value>
		{
			out = static_cast<T>(std::stod(s));
		}

		// bool
		static void from_yaml_impl(const std::string &s, bool &out, int)
		{
			std::string t = s;
			for (auto &c : t)
				c = (char)std::tolower((unsigned char)c);
			out = (t == "true" || t == "1");
		}

		// string
		static void from_yaml_impl(const std::string &s, std::string &out, int)
		{
			out = s;
		}

		// vector<T> expects a sequence where each line starts with '- '
		template <typename Vec>
		static auto from_yaml_impl(const std::string &s, Vec &out, long)
			-> std::enable_if_t<is_std_vector<Vec>::value>
		{
			using Item = typename Vec::value_type;
			out.clear();
			std::istringstream ss(s);
			std::string line;
			while (std::getline(ss, line))
			{
				if (line.size() >= 2 && line[0] == '-' && line[1] == ' ')
				{
					std::string payload = line.substr(2);
					Item item{};
					Deserializer<Item>::from_yaml(myyaml::yaml(payload), item);
					out.push_back(std::move(item));
				}
			}
		}

		// map<string, T> expects lines like 'key: value'
		template <typename Map>
		static auto from_yaml_impl(const std::string &s, Map &out, short)
			-> std::enable_if_t<
				is_std_map<Map>::value &&
				std::is_same<typename Map::key_type, std::string>::value>
		{
			using Mapped = typename Map::mapped_type;
			out.clear();
			std::istringstream ss(s);
			std::string line;
			while (std::getline(ss, line))
			{
				auto pos = line.find(":");
				if (pos != std::string::npos)
				{
					std::string key = line.substr(0, pos);
					// trim
					while (!key.empty() && std::isspace((unsigned char)key.back()))
						key.pop_back();
					std::string val = line.substr(pos + 1);
					// remove leading space
					if (!val.empty() && val[0] == ' ')
						val = val.substr(1);
					Mapped m{};
					Deserializer<Mapped>::from_yaml(myyaml::yaml(val), m);
					out.emplace(key, std::move(m));
				}
			}
		}
	};

	template <typename ObjectType, typename InputType>
	class MYYAML_API Reader
	{
	public:
		Reader() = default;
		explicit Reader(const std::string &path);

		bool open();

		void close();

		// Reads the entire input into an myyaml::yaml document
		bool read(ObjectType &out);

	private:
		std::string path_;
		std::ifstream file_;
	};

#pragma endregion // Reader

#endif // MYYAML_DISABLE_READER

#if !defined(MYYAML_DISABLE_WRITER) || !MYYAML_DISABLE_WRITER

#pragma region Writer

	// Serializer: converts C++ objects into a minimal YAML string stored in
	// myyaml::yaml
	template <class Node>
	class MYYAML_API Serializer
	{
	public:
		Serializer() = default;

		// Serialize value into the provided yaml document.
		static void to_yaml(myyaml::yaml &doc, const Node &value)
		{
			to_yaml_impl(doc, value, (int)0);
		};

	private:
		// arithmetic types (int, float, double, etc.)
		template <typename T>
		static auto to_yaml_impl(myyaml::yaml &doc, const T &v, int)
			-> std::enable_if_t<std::is_arithmetic<T>::value>
		{
			doc.append(std::to_string(v));
		}

		// bool specialization
		static void to_yaml_impl(myyaml::yaml &doc, const bool &b, int)
		{
			doc.append(b ? "true" : "false");
		}

		// std::string
		static void to_yaml_impl(myyaml::yaml &doc, const std::string &s, int)
		{
			doc.append(s);
		}

		// std::vector
		template <typename T>
		static auto to_yaml_impl(myyaml::yaml &doc, const std::vector<T> &vec, long)
			-> std::enable_if_t<is_std_vector<std::vector<T>>::value>
		{
			for (const auto &item : vec)
			{
				// simple sequence item formatting
				myyaml::yaml nested;
				Serializer<T>::to_yaml(nested, item);
				// split nested into single-line if multiple lines
				std::istringstream ss(nested.toString());
				std::string line;
				while (std::getline(ss, line))
				{
					doc.append(std::string("- ") + line);
				}
			}
		}

		// std::map<string, T>
		template <typename M>
		static auto to_yaml_impl(myyaml::yaml &doc, const M &m, short)
			-> std::enable_if_t<
				is_std_map<M>::value &&
				std::is_same<typename M::key_type, std::string>::value>
		{
			for (const auto &p : m)
			{
				myyaml::yaml nested;
				Serializer<typename M::mapped_type>::to_yaml(nested, p.second);
				// write key: value (if nested has single line) or key:\n  <indented
				// block>
				std::istringstream ss(nested.toString());
				std::string first;
				if (std::getline(ss, first))
				{
					std::string rest;
					std::string line;
					std::ostringstream restoss;
					bool multiple = false;
					while (std::getline(ss, line))
					{
						multiple = true;
						restoss << "  " << line << '\n';
					}
					if (!multiple)
					{
						doc.append(p.first + ": " + first);
					}
					else
					{
						doc.append(p.first + ":");
						doc.append(restoss.str(), false);
						// ensure trailing newline
						doc.append(std::string());
					}
				}
				else
				{
					doc.append(p.first + ":");
				}
			}
		}
	};

	template <typename ObjectType, typename OutputType>
	class MYYAML_API Writer
	{
	public:
		Writer() = default;
		explicit Writer(const std::string &path);

		bool open();

		void close();

		bool write(const ObjectType &obj);

	private:
		std::string path_;
		std::ofstream file_;
	};

#pragma endregion // Writer

#endif // MYYAML_DISABLE_WRITER

}; // namespace myyaml

#endif //__cplusplus

#pragma endregion // Cpp

#endif // DJOEZEKE_MYYAML_H
