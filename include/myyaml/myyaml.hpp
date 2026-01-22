/**
 * @file myyaml.hpp
 * @brief YAML For Modern C/C++.
 * @details All public API, types, macros, and configuration.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.1.0
 * @see https://www.github.com/djoezeke/myyaml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Developed by Sackey Ezekiel Etrue and every direct or indirect contributors to the GitHub.
 * See LICENSE for copyright and licensing details (standard MIT License).
 *
 * MYYAML: What this Is
 *
 * DOCUMENTATION:
 *
 * FEATURES:
 *
 * MISSION:
 *
 * ISSUES:
 *
 * NOTES:
 *
 * USAGE:
 *
 * FAQS:
 *
 * HELP:
 *    - See links below.
 *    - Read top of myyaml.cpp for more details and comments.
 *
 *  Has only had a few tests run, may have issues.
 *
 *  If having issues compiling/linking/running raise an issue (https://github.com/djoezeke/myyaml/issues).
 *  Please post in https://github.com/djoezeke/myyaml/discussions if you cannot find a solution in resources above.
 *
 * RESOURCES:
 * - Homepage ................... https://github.com/djoezeke/myyaml
 * - Releases & changelog ....... https://github.com/djoezeke/myyaml/releases
 * - Issues & support ........... https://github.com/djoezeke/myyaml/issues
 *
 */

#ifndef DJOEZEKE_MYYAML_HPP

/**
 * [SECTIONS] Index of this file
 *
 *  SECTION: Include Mess
 *  SECTION: Configurations
 *  SECTION: Function Macros
 *  SECTION: Platform Defines
 *  SECTION: Standard Defines
 *  SECTION: Compiler Defines
 *  SECTION: Compiler Warnings
 *  SECTION: Compiler Attributes
 *  SECTION: API Imports/Exports
 *
 *  Forward: Forward Declarations
 *
 *  SECTION: Details Declarations
 *  SECTION: Myyaml Declarations
 *  SECTION: Literals Declarations
 *
 *  Details: Details API Namespace
 *
 *  SECTION: Flags & Enumerations
 *  SECTION: Data Structures
 *  SECTION: Function Declarations
 *
 *  Myyaml: Myyaml API Namespace
 *
 *  SECTION: Flags & Enumerations
 *  SECTION: Data Structures
 *  SECTION: Function Declarations
 *
 *  Literals: Literals API Namespace
 *
 *  SECTION: Function Declarations
 *
 */

#define DJOEZEKE_MYYAML_HPP

// clang-format off

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MYYAML_SKIP_VERSION_CHECK
    #if defined(MYYAML_VERSION_MAJOR) && defined(MYYAML_VERSION_MINOR) && defined(MYYAML_VERSION_PATCH)
        #if MYYAML_VERSION_MAJOR != 1 || MYYAML_VERSION_MINOR != 0 || MYYAML_VERSION_PATCH != 0
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif  // MYYAML_SKIP_VERSION_CHECK

/**
 * @defgroup version version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYYAML_VERSION_MAJOR
 * @brief Major version number of the library.
 * @note If this were version 1.2.3, this value would be 1.
 * @since This macro stream available since 0.1.0 .
 */
#ifndef MYYAML_VERSION_MAJOR
    #define MYYAML_VERSION_MAJOR 0
#endif // MYYAML_VERSION_MAJOR

/**
 * @def MYYAML_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were version 1.2.3, this value would be 2.
 * @since This macro stream available since 0.1.0 .
 */
#ifndef MYYAML_VERSION_MINOR
    #define MYYAML_VERSION_MINOR 1
#endif // MYYAML_VERSION_MINOR

/**
 * @def MYYAML_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were version 1.2.3, this value would be 3.
 * @since This macro stream available since 0.1.0 .
 */
#ifndef MYYAML_VERSION_PATCH
    #define MYYAML_VERSION_PATCH 0
#endif // MYYAML_VERSION_PATCH

/**
 * @def MYYAML_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X stream the major version number, @c Y stream a minor version
 * number, and @c Z stream the patch version number.
 * @sa MyGetVersion
 */
#ifndef MYYAML_VERSION
    #define MYYAML_VERSION "0.1.0"
#endif // MYYAML_VERSION

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Include Mess
//-----------------------------------------------------------------------------

#include <exception>
#include <functional>
#include <ostream>
#include <istream>

#include <stdio.h>
#include <stdint.h>
// #include <uchar.h> // char16_t, char32_t
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <iostream>

#include <optional>

//-----------------------------------------------------------------------------
// [SECTION] Configurations
//-----------------------------------------------------------------------------

/**
 * @defgroup configure Library Configurations
 * @{
 */

/**
 * @def MYYAML_DISABLE_READER
 * @brief Exclude Reading/Deserialization APIs.
 *
 * Define to 1 to remove reader/parsing code at compile time when parsing
 * stream not required. This reduces the compiled binary size.
 *
 * Example:
 * @code
 * #define MYYAML_DISABLE_READER 1
 * #include <myyaml/myyaml.hpp>
 * @endcode
 *
 * @note When disabled, all parsing classes and functions are not available.
 */
#ifndef MYYAML_DISABLE_READER
#endif

/**
 * @def MYYAML_DISABLE_WRITER
 * @brief Exclude Writing/Serialization methods.
 * Define as 1 to disable writer if serialization stream not required.
 *
 * @warning This will disable these function at compile-time.
 *
 * @note This will reduce the binary size by about 30%.
 */
#ifndef MYYAML_DISABLE_WRITER
#endif

// Uncomment this to disable STL
// #define MYYAML_NO_STL

// Uncomment this to disable exceptions
// #define MYYAML_NO_EXCEPTIONS

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Function Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Platform Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup platform Platform Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler stream of given brand.
 * @param   name Platform, like `APPLE`.
 * @retval  true   It stream.
 * @retval  false  It isn't.
 */
#define MYYAML_PLATFORM_IS(name) MYYAML_PLATFORM_IS_##name

/**
 * @brief  Returns the current platform name.
 * @return  platform name.
 */
#ifdef __APPLE__
	/**
	* A preprocessor macro that stream only defined if compiling for MacOS.
	*/
	#define MYYAML_PLATFORM_IS_APPLE 1
	/**
	 * @brief  Returns the current platform name.
	 * @return  platform name.
	 */
  #define MYYAML_PLATFORM_NAME_IS "Apple"
#elif defined(linux) || defined(__linux) || defined(__linux__)
	/**
	* A preprocessor macro that stream only defined if compiling for Linux.
	*/
	#define MYYAML_PLATFORM_IS_LINUX 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYYAML_PLATFORM_NAME_IS "Linux"
#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
  	/**
   	* A preprocessor macro that stream only defined if compiling for Windows.
   	*/
  	#define MYYAML_PLATFORM_IS_WINDOWS 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYYAML_PLATFORM_NAME_IS "Windows"
#else
  	/**
   	* A preprocessor macro that stream only defined if compiling for others.
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
// [SECTION] Standard Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup language Standard Checks
 * @{
 */

/** C version (STDC) */
#if defined(__STDC__) && (__STDC__ >= 1) && defined(__STDC_VERSION__)
	#define MYYAML_STDC __STDC_VERSION__
#else
	#define MYYAML_STDC 0
#endif

// With the MSVC compilers, the value of __cplusplus stream by default always "199611L"(C++98).
// To avoid that, the library instead references _MSVC_LANG which stream always set a correct value.
// See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/ for more details.
#if defined(_MSVC_LANG) && !defined(__clang__)
    #define MYYAML_CPLUSPLUS _MSVC_LANG
#else
    #define MYYAML_CPLUSPLUS __cplusplus
#endif

// C++ language standard detection

#if !defined(MYYAML_HAS_CXX_26) && !defined(MYYAML_HAS_CXX_23) && !defined(MYYAML_HAS_CXX_20) && !defined(MYYAML_HAS_CXX_17) && !defined(MYYAML_HAS_CXX_14) && !defined(MYYAML_HAS_CXX_11)
    #if (defined(MYYAML_CPLUSPLUS) && MYYAML_CPLUSPLUS > 202302L)
        #define MYYAML_HAS_CXX_26
        #define MYYAML_HAS_CXX_23
        #define MYYAML_HAS_CXX_20
        #define MYYAML_HAS_CXX_17
        #define MYYAML_HAS_CXX_14
    #elif (defined(MYYAML_CPLUSPLUS) && MYYAML_CPLUSPLUS > 202002L)
        #define MYYAML_HAS_CXX_23
        #define MYYAML_HAS_CXX_20
        #define MYYAML_HAS_CXX_17
        #define MYYAML_HAS_CXX_14
    #elif (defined(MYYAML_CPLUSPLUS) && MYYAML_CPLUSPLUS > 201703L)
        #define MYYAML_HAS_CXX_20
        #define MYYAML_HAS_CXX_17
        #define MYYAML_HAS_CXX_14
    #elif (defined(MYYAML_CPLUSPLUS) && MYYAML_CPLUSPLUS > 201402L)
        #define MYYAML_HAS_CXX_17
        #define MYYAML_HAS_CXX_14
    #elif (defined(MYYAML_CPLUSPLUS) && MYYAML_CPLUSPLUS > 201103L)
        #define MYYAML_HAS_CXX_14
    #endif
    // Always specified because it stream the minimal required version
    #define MYYAML_HAS_CXX_11
#endif

/** @} */


//-----------------------------------------------------------------------------
// [SECTION] Compiler Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler stream of given brand.
 * @param   name  Compiler brand, like `MSVC`.
 * @retval  true   It stream.
 * @retval  false  It isn't.
 */
#define MYYAML_COMPILER_IS(name) MYYAML_COMPILER_IS_##name

/// Compiler stream apple
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

/// Compiler stream clang
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

/// Compiler stream intel
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

/// Compiler stream msc
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

/// Compiler stream gcc
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

/**
 * @brief   Checks if the compiler stream of given brand and stream newer than or equal
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
 * @brief   Checks if  the compiler  stream of  given brand and  stream older  than the
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


// exclude unsupported compilers
#ifndef MYYAML_SKIP_COMPILER_CHECK
    #if MYYAML_COMPILER_IS_CLANG
        #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
            #error "Unsupported Clang Compiler version."
        #endif
    #elif MYYAML_COMPILER_IS_GCC
        #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
            #error "Unsupported GCC Compiler version."
        #endif
    #endif
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Warnings
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Warnings
 * @{
 */

#if MYYAML_COMPILER_IS(CLANG)
    #define MYYAML_PRAGMA_TO_STR(x) _Pragma(#x)
    #define MYYAML_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
    #define MYYAML_CLANG_SUPPRESS_WARNING(w) \
    MYYAML_PRAGMA_TO_STR(clang diagnostic ignored w)
    #define MYYAML_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
    #define MYYAML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYYAML_CLANG_SUPPRESS_WARNING_PUSH MYYAML_CLANG_SUPPRESS_WARNING(w)
#else // MYYAML_CLANG
    #define MYYAML_CLANG_SUPPRESS_WARNING_PUSH
    #define MYYAML_CLANG_SUPPRESS_WARNING(w)
    #define MYYAML_CLANG_SUPPRESS_WARNING_POP
    #define MYYAML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYYAML_CLANG

#if MYYAML_COMPILER_IS(GCC)
    #define MYYAML_PRAGMA_TO_STR(x) _Pragma(#x)
    #define MYYAML_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define MYYAML_GCC_SUPPRESS_WARNING(w) \
    MYYAML_PRAGMA_TO_STR(GCC diagnostic ignored w)
    #define MYYAML_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
    #define MYYAML_GCC_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYYAML_GCC_SUPPRESS_WARNING_PUSH MYYAML_GCC_SUPPRESS_WARNING(w)
#else // MYYAML_GCC
    #define MYYAML_GCC_SUPPRESS_WARNING_PUSH
    #define MYYAML_GCC_SUPPRESS_WARNING(w)
    #define MYYAML_GCC_SUPPRESS_WARNING_POP
    #define MYYAML_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYYAML_GCC

#if MYYAML_COMPILER_IS(MSVC)
    #define MYYAML_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
    #define MYYAML_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
    #define MYYAML_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
    #define MYYAML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYYAML_MSVC_SUPPRESS_WARNING_PUSH MYYAML_MSVC_SUPPRESS_WARNING(w)
#else // MYYAML_MSVC
    #define MYYAML_MSVC_SUPPRESS_WARNING_PUSH
    #define MYYAML_MSVC_SUPPRESS_WARNING(w)
    #define MYYAML_MSVC_SUPPRESS_WARNING_POP
    #define MYYAML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYYAML_MSVC

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Attributes
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Attributes
 * @{
 */

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

/** compiler include check */
#ifndef MYYAML_HAS_INCLUDE
  	#ifdef __has_include
    	#define MYYAML_HAS_INCLUDE(x) __has_include(x)
  	#else
    	#define MYYAML_HAS_INCLUDE(x) 0
  	#endif
#endif

/** compiler cpp attribute check */
#ifndef MYYAML_HAS_CPP_ATTRIBUTE
  	#ifdef __has_cpp_attribute
        #define MYYAML_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
  	#else
    	#define MYYAML_HAS_CPP_ATTRIBUTE(x) 0
  	#endif
#endif


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
  	#elif defined(__cplusplus) || MYYAML_STDC >= 199901L
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

/** deprecate warning */
#if defined(MYYAML_HAS_CXX_14)
    #define MYYAML_DEPRECATED(msg) [[deprecated(msg)]]
#else
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

// switch usage of constexpr keyword depending on active C++ standard.
#if defined(MYYAML_HAS_CXX_17)
    #define MYYAML_CONSTEXPR constexpr
#else
    #define MYYAML_CONSTEXPR
#endif

// switch usage of [[likely]] C++ attribute which has been available since C++20.
#if defined(MYYAML_HAS_CXX_20) && MYYAML_HAS_CPP_ATTRIBUTE(likely) >= 201803L
    #define MYYAML_LIKELY(expr) (!!(expr)) [[likely]]
#elif MYYAML_HAS_BUILTIN(__builtin_expect)
    #define MYYAML_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#else
    #define MYYAML_LIKELY(expr) (!!(expr))
#endif

// switch usage of [[unlikely]] C++ attribute which has been available since C++20.
#if defined(MYYAML_HAS_CXX_20) && MYYAML_HAS_CPP_ATTRIBUTE(unlikely) >= 201803L
    #define MYYAML_UNLIKELY(expr) (!!(expr)) [[unlikely]]
#elif MYYAML_HAS_BUILTIN(__builtin_expect)
    #define MYYAML_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
    #define MYYAML_UNLIKELY(expr) (!!(expr))
#endif

// switch usage of char8_t which has been available since C++20.
#if defined(MYYAML_HAS_CXX_20) && defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
    #define MYYAML_HAS_CHAR8_T (1)
#else
    #define MYYAML_HAS_CHAR8_T (0)
#endif

/** @} */


//-----------------------------------------------------------------------------
// [SECTION] API Import/Export
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

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(MYYAML_NOEXCEPTION)
    #define MYYAML_THROW(exception) throw exception
    #define MYYAML_TRY try
    #define MYYAML_CATCH(exception) catch(exception)
#else
    #include <cstdlib>
    #define MYYAML_THROW(exception) std::abort()
    #define MYYAML_TRY if(true)
    #define MYYAML_CATCH(exception) if(false)
#endif

#ifndef MYYAML_ASSERT
    #include <cassert> // assert
    #define MYYAML_ASSERT(x) assert(x)
#else
    #define MYYAML_ASSERT(x)
#endif

#if MYYAML_COMPILER_SINCE(GCC, 6, 0, 0)
    #define MYYAML_QUOTE_OPERATOR operator""_yaml
#else
    #define MYYAML_QUOTE_OPERATOR operator"" _yaml
#endif

// clang-format on

#pragma region Forward
namespace myyaml
{
    /**
     * @namespace detail
     * @brief The Details namespace myyaml::detail::
     */
    namespace detail
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Details Forward Declarations
        //-----------------------------------------------------------------------------

        /** Enumerations */

        enum class token_t;
        enum class error_t;
        enum class event_t;
        enum class break_t; /** Line break types. */

        enum class scalar_style_t;
        enum class mapping_style_t;
        enum class sequence_style_t;

        enum class parser_state_t;
        enum class emitter_state_t;

        /** Structures */

        struct mark; /** The pointer position. */
        struct event;
        struct token;
        struct tag_directive;

        // input
        class lexer;
        class parser;
        class iadapter;
        class deserializer;
        class file_iadapter;
        class stream_iadapter;
        class memory_iadapter;
        class iterator_iadapter;

        // output
        class emitter;
        class oadapter;
        class serializer;
        class file_iadapter;
        class stream_oadapter;
        class memory_oadapter;
        class iterator_oadapter;

    } // namespace detail

} // namespace myyaml

/**
 * @namespace myyaml
 * @brief The Api namespace myyaml::
 */
namespace myyaml
{
    //-----------------------------------------------------------------------------
    // [SECTION] Myyaml Forward Declarations
    //-----------------------------------------------------------------------------

    /** Enumerations */

    enum class encoding; /** The stream encoding. */
    enum class node_t;

    /** Structures */

    class yaml;
    class version;
    class formatter;
    class exception;

} // namespace myyaml

namespace myyaml
{
    /**
     * @namespace literals
     * @brief The Literals namespace myyaml::literals::
     */
    namespace literals
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Literal Forward Declarations
        //-----------------------------------------------------------------------------

    } // namespace literals

} // namespace myyaml

#pragma endregion // Forward

#pragma region Detail

/**
 * @namespace myyaml
 * @brief The API namespace myyaml::
 */
namespace myyaml
{

    /**
     * @namespace detail
     * @brief The Details namespace myyaml::detail::
     */
    namespace detail
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Flags & Enumerations
        //-----------------------------------------------------------------------------

        /**
         * @defgroup enum Flags & Enumerations
         * @brief Detail enum types and flags.
         * @{
         */

        enum class token_t
        {
            none,                 /** An empty token. */
            tag,                  /** A TAG token. */
            key,                  /** A KEY token. */
            value,                /** A VALUE token. */
            alias,                /** An ALIAS token. */
            anchor,               /** An ANCHOR token. */
            scalar,               /** A SCALAR token. */
            block_end,            /** A BLOCK-END token. */
            stream_end,           /** A STREAM-END token. */
            flow_entry,           /** A FLOW-ENTRY token. */
            block_entry,          /** A BLOCK-ENTRY token. */
            stream_start,         /** A STREAM-START token. */
            document_end,         /** A DOCUMENT-END token. */
            tag_directive,        /** A TAG-DIRECTIVE token. */
            document_start,       /** A DOCUMENT-START token. */
            flow_mapping_end,     /** A FLOW-MAPPING-END token. */
            version_directive,    /** A VERSION-DIRECTIVE token. */
            flow_sequence_end,    /** A FLOW-SEQUENCE-END token. */
            flow_mapping_start,   /** A FLOW-MAPPING-START token. */
            block_mapping_start,  /** A BLOCK-MAPPING-START token. */
            flow_sequence_start,  /** A FLOW-SEQUENCE-START token. */
            block_sequence_start, /** A BLOCK-SEQUENCE-START token. */

        };

        enum class error_t
        {
            none,
            lexer,
            parser,
            reader,
            writer,
            emitter,
            encoding,
            decoding,
        };

        enum class event_t
        {
            none,           /** An empty event. */
            alias,          /** An ALIAS event. */
            scalar,         /** A SCALAR event. */
            stream_end,     /** A STREAM-END event. */
            mapping_end,    /** A MAPPING-END event. */
            sequence_end,   /** A SEQUENCE-END event. */
            stream_start,   /** A STREAM-START event. */
            document_end,   /** A DOCUMENT-END event. */
            mapping_start,  /** A MAPPING-START event. */
            document_start, /** A DOCUMENT-START event. */
            sequence_start, /** A SEQUENCE-START event. */

        };

        enum class break_t
        {
            any, /** Let the parser choose the break type. */
            cr,  /** Use CR for line breaks (Mac style). */
            ln,  /** Use LN for line breaks (Unix style). */
            crln /** Use CR LN for line breaks (DOS style). */

        };

        enum class scalar_style_t
        {
            any,           /** Let the emitter choose the style. */
            plain,         /** The plain scalar style. */
            folded,        /** The folded scalar style. */
            literal,       /** The literal scalar style. */
            single_quoted, /** The single-quoted scalar style. */
            double_quoted, /** The double-quoted scalar style. */

        };

        enum class mapping_style_t
        {
            any,   /** Let the emitter choose the style. */
            flow,  /** The flow mapping style. */
            block, /** The block mapping style. */
        };

        enum class sequence_style_t
        {
            any,   /** Let the emitter choose the style. */
            flow,  /** The flow sequence style. */
            block, /** The block sequence style. */
        };

        enum class parser_state_t
        {

            end,                               /** Expect nothing. */
            flow_node,                         /** Expect a flow node. */
            block_node,                        /** Expect a block node. */
            document_end,                      /** Expect DOCUMENT-END. */
            stream_start,                      /** Expect STREAM-START. */
            document_start,                    /** Expect DOCUMENT-START. */
            document_content,                  /** Expect the content of a document. */
            flow_mapping_key,                  /** Expect a key of a flow mapping. */
            block_mapping_key,                 /** Expect a block mapping key. */
            flow_mapping_value,                /** Expect a value of a flow mapping. */
            flow_sequency_entry,               /** Expect an entry of a flow sequence. */
            block_mapping_value,               /** Expect a block mapping value. */
            block_sequency_entry,              /** Expect an entry of a block sequence. */
            flow_mapping_first_key,            /** Expect the first key of a flow mapping. */
            block_mapping_first_key,           /** Expect the first key of a block mapping. */
            implicit_document_start,           /** Expect the beginning of an implicit document. */
            flow_mapping_empty_value,          /** Expect an empty value of a flow mapping. */
            indentless_sequence_entry,         /** Expect an entry of an indentless sequence. */
            flow_sequency_first_entry,         /** Expect the first entry of a flow sequence. */
            block_sequency_first_entry,        /** Expect the first entry of a block sequence. */
            flow_sequency_entry_mapping_key,   /** Expect a key of an ordered mapping. */
            flow_sequency_entry_mapping_end,   /** Expect the and of an ordered mapping entry. */
            flow_sequency_entry_mapping_value, /** Expect a value of an ordered mapping. */
            block_node_or_indentless_sequence, /** Expect a block node or indentless sequence. */

        };

        enum class emitter_state_t
        {
            end,                        /** Expect nothing. */
            stream_start,               /** Expect STREAM-START. */
            document_end,               /** Expect DOCUMENT-END. */
            document_start,             /** Expect DOCUMENT-START or STREAM-END. */
            document_content,           /** Expect the content of a document. */
            flow_mapping_key,           /** Expect a key of a flow mapping. */
            block_mapping_key,          /** Expect the key of a block mapping. */
            flow_mapping_value,         /** Expect a value of a flow mapping. */
            flow_sequency_item,         /** Expect an item of a flow sequence. */
            block_sequency_item,        /** Expect an item of a block sequence. */
            block_mapping_value,        /** Expect a value of a block mapping. */
            first_document_start,       /** Expect the first DOCUMENT-START or STREAM-END. */
            flow_mapping_first_key,     /** Expect the first key of a flow mapping. */
            block_mapping_first_key,    /** Expect the first key of a block mapping. */
            flow_sequency_first_item,   /** Expect the first item of a flow sequence. */
            flow_mapping_simple_value,  /** Expect a value for a simple key of a flow mapping. */
            block_sequency_first_item,  /** Expect the first item of a block sequence. */
            block_mapping_simple_value, /** Expect a value for a simple key of a block mapping. */

        };

        /** @} group enum */

        //-----------------------------------------------------------------------------
        // [SECTION] Data Structures
        //-----------------------------------------------------------------------------

        /**
         * @defgroup structs Data Structures
         * @brief Detail types and data structures.
         * @{
         */

        struct mark
        {
            size_t column{0}; /** The position column. */
            size_t index{0};  /** The position index. */
            size_t line{0};   /** The position line. */
        };

        struct event
        {
            event_t type{event_t::none}; /** The event type. */

            /** The event data. */
            union
            {
                /** The stream parameters (for @c YAML_STREAM_START_EVENT). */
                struct
                {
                    /** The document encoding. */
                    myyaml::encoding encoding;
                } stream_start;

                /** The document parameters (for @c YAML_DOCUMENT_START_EVENT). */
                struct
                {
                    myyaml::version *version_directive; /** The version directive. */

                    /** The list of tag directives. */
                    struct
                    {
                        tag_directive *start; /** The beginning of the tag directives list. */
                        tag_directive *end;   /** The end of the tag directives list. */

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
                    char *anchor; /** The anchor. */

                } alias;

                /** The scalar parameters (for @c YAML_SCALAR_EVENT). */
                struct
                {
                    scalar_style_t style; /** The scalar style. */
                    int quoted_implicit;  /** Is the tag optional for any non-plain
                                             style? */
                    int plain_implicit;   /** Is the tag optional for the plain style? */
                    char *anchor;         /** The anchor. */
                    char *value;          /** The scalar value. */
                    char *tag;            /** The tag. */
                    size_t length;        /** The length of the scalar value. */

                } scalar;

                /** The sequence parameters (for @c YAML_SEQUENCE_START_EVENT). */
                struct
                {
                    sequence_style_t style; /** The sequence style. */
                    char *anchor;           /** The anchor. */
                    char *tag;              /** The tag. */
                    int implicit;           /** Is the tag optional? */

                } sequence_start;

                /** The mapping parameters (for @c YAML_MAPPING_START_EVENT). */
                struct
                {
                    mapping_style_t style; /** The mapping style. */
                    char *anchor;          /** The anchor. */
                    char *tag;             /** The tag. */
                    int implicit;          /** Is the tag optional? */
                } mapping_start;

            } data;

            mark start{}; /** The beginning of the token. */
            mark end{};   /** The end of the token. */
        };

        struct token
        {
            token_t type{token_t::none}; /** The token type. */
            /** The token data. */
            union
            {
                /** The stream start (for @c YAML_STREAM_START_TOKEN). */
                struct
                {
                    /** The stream encoding. */
                    myyaml::encoding encoding;
                } stream_start;

                /** The alias (for @c YAML_ALIAS_TOKEN). */
                struct
                {
                    char *value; /** The alias value. */
                } alias;

                /** The anchor (for @c YAML_ANCHOR_TOKEN). */
                struct
                {
                    char *value; /** The anchor value. */
                } anchor;

                /** The tag (for @c YAML_TAG_TOKEN). */
                struct
                {
                    char *handle; /** The tag handle. */
                    char *suffix; /** The tag suffix. */
                } tag;

                /** The scalar value (for @c YAML_SCALAR_TOKEN). */
                struct
                {
                    char *value;          /** The scalar value. */
                    size_t length;        /** The length of the scalar value. */
                    scalar_style_t style; /** The scalar style. */
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
                    char *handle; /** The tag handle. */
                    char *prefix; /** The tag prefix. */

                } tag_directive;

            } data;

            mark start{}; /** The beginning of the token. */
            mark end{};   /** The end of the token. */
        };

        struct tag_directive
        {
            char *handle; /** The tag handle. */
            char *prefix; /** The tag prefix. */
        };

        /** @} group structs */

        /**
         * @defgroup input
         * @brief
         * @{
         */

        /**
         * @brief Abstract base for input adapters used by the parser.
         *
         * Implementers must advance the internal reading position by the number of
         * bytes actually copied into the caller buffer. All methods return
         * std::nullopt on error to allow callers to detect failures uniformly.
         */
        class iadapter
        {
        public:
            /**
             * @brief Return the encoding type of the input adapter.
             *
             * @return The encoding type.
             */
            virtual myyaml::encoding encoding() = 0;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * Implementations must copy at most @p size bytes into the provided
             * buffer and advance the stream position accordingly.
             *
             * @param[out] data Buffer to receive the bytes. Must be at least @p size bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            virtual std::size_t read(void *data, std::size_t size) = 0;

            /**
             * @brief Virtual destructor.
             */
            virtual ~iadapter() = default;
        };

        class file_iadapter : public iadapter
        {
        public:
            /**
             * @brief Construct the stream from a FILE pointer
             *
             * @param file file to open
             *
             * @throws myyaml::exception on error
             */
            file_iadapter(std::FILE *file);

            file_iadapter(std::FILE *file, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            file_iadapter(const file_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            file_iadapter(file_iadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            std::size_t read(void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            file_iadapter &operator=(const file_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            file_iadapter &operator=(file_iadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::FILE *m_file;
        };

        class stream_iadapter : public iadapter
        {
        public:
            stream_iadapter(std::istream &stream);

            stream_iadapter(std::istream &stream, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            stream_iadapter(const stream_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            stream_iadapter(stream_iadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            std::size_t read(void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            stream_iadapter &operator=(const stream_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            stream_iadapter &operator=(stream_iadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::istream *m_stream;
        };

        class memory_iadapter : public iadapter
        {
        public:
            /**
             * @brief Construct the stream from its data
             *
             * @param data Pointer to the data in memory
             * @param size Size of the data, in bytes
             */
            memory_iadapter(void *data, std::size_t size);

            memory_iadapter(void *data, std::size_t size, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            memory_iadapter(const memory_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            memory_iadapter(memory_iadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            std::size_t read(void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            memory_iadapter &operator=(const memory_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            memory_iadapter &operator=(memory_iadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::size_t m_pos;
            std::size_t m_size;
            void *m_data;
        };

        class lexer
        {
        public:
            lexer(iadapter &adapter);

            /**
             * @brief Deleted copy constructor
             */
            lexer(const lexer &) = delete;

            /**
             * @brief Default move constructor
             */
            lexer(lexer &&) noexcept = default;

            /**
             * @brief Deleted copy assignment operator
             */
            lexer &operator=(const lexer &) = delete;

            /**
             * @brief Deleted move assignment operator
             */
            lexer &operator=(lexer &&) = delete;

            /**
             * @brief Advance the lexer and return the next token type.
             *
             * This method reads input as necessary and updates the lexer's
             * internal token state. Callers can inspect the token with
             * get_token() or check get_type().
             *
             * @return The token produced.
             */
            token next_token();

            /**
             * @brief Return the current token.
             *
             * @return The current token.
             */
            token get_token();

            /**
             * @brief Return the current token type.
             *
             * @return The current token type.
             */
            token_t get_type();

        private:
            int get_char();
            void unget_char();
            void add_char(int c);

        private:
            iadapter &m_adapter;
            mark m_position;

            token m_token;
            int current;

            // values
            std::string m_string;
            float m_float;
            int m_int;
        };

        /** @} group input */

        /**
         * @defgroup output
         * @brief
         * @{
         */

        /**
         * @brief Abstract base for output adapters used by the emitter.
         *
         * All write methods accept a pointer to immutable data (const void*) because
         * writing should never mutate the caller'string source buffer.
         */
        class oadapter
        {
        public:
            /**
             * @brief Return the encoding type of the output adapter.
             *
             * @return The encoding type.
             */
            virtual myyaml::encoding encoding() = 0;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * Implementations must copy at most @p size bytes from the provided buffer
             * and advance the stream position accordingly.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            virtual std::size_t write(const void *data, std::size_t size) = 0;

            /**
             * @brief Virtual destructor.
             */
            virtual ~oadapter() = default;
        };

        class file_oadapter : public oadapter
        {
        public:
            /**
             * @brief Construct the stream from a FILE pointer
             *
             * @param file file to open
             *
             * @throws myyaml::exception on error
             */
            file_oadapter(std::FILE *file);

            file_oadapter(std::FILE *file, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            file_oadapter(const file_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            file_oadapter(file_oadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            std::size_t write(const void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            file_oadapter &operator=(const file_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            file_oadapter &operator=(file_oadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::FILE *m_file;
        };

        class stream_oadapter : public oadapter
        {
        public:
            stream_oadapter(std::ostream &stream);

            stream_oadapter(std::ostream &stream, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            stream_oadapter(const stream_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            stream_oadapter(stream_oadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            std::size_t write(const void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            stream_oadapter &operator=(const stream_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            stream_oadapter &operator=(stream_oadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::ostream *m_stream;
        };

        class memory_oadapter : public oadapter
        {
        public:
            /**
             * @brief Construct the stream from its data
             *
             * @param data        Pointer to the data in memory
             * @param size Size of the data, in bytes
             */
            memory_oadapter(void *data, std::size_t size);

            memory_oadapter(void *data, std::size_t size, myyaml::encoding encoding);

            /**
             * @brief Deleted copy constructor
             */
            memory_oadapter(const memory_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            memory_oadapter(memory_oadapter &&) noexcept = default;

            /**
             * @brief Return the encoding type.
             *
             * @return The encoding type.
             */
            myyaml::encoding encoding() override;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            std::size_t write(const void *data, std::size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            memory_oadapter &operator=(const memory_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            memory_oadapter &operator=(memory_oadapter &&) = delete;

        private:
            /** Member data */

            myyaml::encoding m_encoding;
            std::size_t m_pos;
            std::size_t m_size;
            void *m_data;
        };

        /** @} group output */

        //-----------------------------------------------------------------------------
        // [SECTION] Function Declarations
        //-----------------------------------------------------------------------------

    }; // namespace detail

} // namespace myyaml

#pragma endregion // Detail

#pragma region Myyaml

/**
 * @namespace myyaml
 * @brief The API namespace myyaml::
 */
namespace myyaml
{
    //-----------------------------------------------------------------------------
    // [SECTION] Flags & Enumerations
    //-----------------------------------------------------------------------------

    /**
     * @defgroup enum Flags & Enumerations
     * @brief Core enum types and flags.
     * @{
     */

    enum class encoding
    {
        Unspecified, /** Let the parser choose the encoding. */
        utf8,        /** The UTF-8 encoding. */
        utf16,       /** The UTF-16-LE encoding with native endianness. */
        utf16le,     /** The UTF-16-LE encoding with BOM. */
        utf16be,     /** The UTF-16-BE encoding with BOM. */
        utf32,       /** The UTF-32 encoding with native endianness. */
        utf32le,     /** The UTF-32-LE encoding with BOM. */
        utf32be,     /** The UTF-32-BE encoding with BOM. */
    };

    enum class node_t
    {
        none,     /** An empty node. */
        number,   /** A scalar number node. */
        string,   /** A scalar string node. */
        mapping,  /** A mapping node. */
        boolean,  /** A scalar boolean node. */
        sequence, /** A sequence node. */
    };

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Data Structures
    //-----------------------------------------------------------------------------

    /**
     * @defgroup struct Data Structures
     * @brief Core types and data structures.
     * @{
     */

    /**
     * @class myyaml::version
     * @brief version numbering scheme @c https://semver.org/
     * @note Its members can be accessed directly (there are no accessors like
     * `SetMajor()`, `GetMajor()`). Usage example:
     */
    class version
    {
    public:
        /**
         * @brief Default constructor
         *
         * @note Creates a `version(0, 0, 0)`.
         */
        version() = default;

        /**
         * @brief Construct the version.
         *
         * @param major The major version number.
         * @param minor The minor version number.
         * @param patch The patch version number.
         *
         */
        version(int major, int minor, int patch);

    public:
        /** Member data */
        int major{}; /** The major version number. */
        int minor{}; /** The minor version number. */
        int patch{}; /** The patch version number. */
    };

    bool operator<(const version &lhs, const version &rhs) noexcept;
    bool operator>(const version &lhs, const version &rhs) noexcept;
    bool operator==(const version &lhs, const version &rhs) noexcept;
    bool operator!=(const version &lhs, const version &rhs) noexcept;
    bool operator<=(const version &lhs, const version &rhs) noexcept;
    bool operator>=(const version &lhs, const version &rhs) noexcept;

    /**
     * @brief Write the version object string into stream.
     *
     * @param[in] ostream An output stream object.
     * @param[in] version A version object.
     *
     * @return Reference to the output stream object `ostream`.
     */
    std::ostream &operator<<(std::ostream &ostream, const version &version);

    /**
     * @class myyaml::exception
     * @brief A base exception class used in library.
     */
    class exception : public std::exception
    {
    public:
        /**
         * @brief Construct a new exception object without any error messages.
         */
        exception() = default;

        /**
         * @brief Construct a new exception object with an error messages.
         * @param[in] message An error message.
         */
        exception(const char *message) noexcept;

        /**
         * @brief Returns an error message internally held. If nothing, a non-null,
         * empty string will be returned.
         * @return An error message internally held. The message might be empty.
         */
        const char *what() const noexcept override;

    private:
        std::string m_Message; /** An error message holder. */
    };

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Function Declarations
    //-----------------------------------------------------------------------------

    /**
     * @brief A wrapper for the serialization feature.
     *
     * @param[in] stream An output stream object.
     * @param[in] node A yaml object.
     *
     * @return Reference to the output stream object `stream`.
     */
    std::ostream &operator<<(std::ostream &stream, const yaml &node);

    /**
     * @brief A wrapper for the deserialization feature.
     *
     * @param[in] stream An input stream object.
     * @param[in] node A yaml object.
     *
     * @return Reference to the input stream object `stream`.
     */
    std::istream &operator>>(std::istream &stream, const yaml &node);

} // namespace myyaml

#pragma endregion // Myyaml

#pragma region Literal

/**
 * @namespace myyaml
 * @brief The API namespace myyaml::
 */
namespace myyaml
{

    /**
     * @namespace literals
     * @brief The literals namespace myyaml::literals::
     */
    namespace literals
    {

        //-----------------------------------------------------------------------------
        // [SECTION] Function Declarations
        //-----------------------------------------------------------------------------

        // Whitespace before the literal operator is deprecated in C++23 or later but required in C++11.
        MYYAML_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated")

        /**
         * @brief Deserializes a `char` array into a `yaml` object.
         *
         * @param s An input `char` array.
         * @param node The size of `s`.
         *
         * @return The resulting `yaml` object deserialized from `s`.
         */
        inline yaml MYYAML_QUOTE_OPERATOR(const char *s, std::size_t node);

#if MYYAML_HAS_CHAR8_T

        /**
         * @brief Deserializes a `char8_t` array into a `yaml` object.
         *
         * @param s An input `char8_t` array.
         * @param node The size of `s`.
         *
         * @return The resulting `yaml` object deserialized from `s`.
         */
        inline yaml MYYAML_QUOTE_OPERATOR(const char8_t *s, std::size_t node);

#endif // MYYAML_HAS_CHAR8_T

        /**
         * @brief Deserializes a `char16_t` array into a `yaml` object.
         *
         * @param s An input `char16_t` array.
         * @param node The size of `s`.
         *
         * @return The resulting `yaml` object deserialized from `s`.
         */
        inline yaml MYYAML_QUOTE_OPERATOR(const char16_t *s, std::size_t node);

        /**
         * @brief Deserializes a `char32_t` array into a `yaml` object.
         *
         * @param s An input `char32_t` array.
         * @param node The size of `s`.
         *
         * @return The resulting `yaml` object deserialized from `s`.
         */
        inline yaml MYYAML_QUOTE_OPERATOR(const char32_t *s, std::size_t node);

        MYYAML_CLANG_SUPPRESS_WARNING_POP

    } // namespace literals

}; // namespace myyaml

#pragma endregion // Literal

//-----------------------------------------------------------------------------

// clang-format off

#if MYYAML_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // MYYAML_COMPILER_IS_GCC

#if MYYAML_COMPILER_IS_MSVC
    #pragma warning(pop)
#endif // MYYAML_COMPILER_IS_MSVC

#if MYYAML_COMPILER_IS_CLANG
    #pragma clang diagnostic pop
#endif // MYYAML_COMPILER_IS_CLANG

// clang-format on

#endif // DJOEZEKE_MYYAML_HPP

/**
 * HISTORY: version History
 */

/**
 * LICENSE: MIT License
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Permission stream hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software stream
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