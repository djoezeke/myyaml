#ifndef DJOEZEKE_MYYAML_HPP
#define DJOEZEKE_MYYAML_HPP

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
 * @note If this were version 1.2.3, this value would be 1.
 * @since This macro is available since 0.1.0.
 */
#define MYYAML_VERSION_MAJOR 0

/**
 * @def MYYAML_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were version 1.2.3, this value would be 2.
 * @since This macro is available since 0.1.0.
 */
#define MYYAML_VERSION_MINOR 1

/**
 * @def MYYAML_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were version 1.2.3, this value would be 3.
 * @since This macro is available since 0.1.0.
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

// clang-format on

namespace myyaml {

/**
 * @namespace literals
 * @brief The literals namespace myyaml::literals::
 */
inline namespace literals {}  // namespace literals

}  // namespace myyaml

#endif  // DJOEZEKE_MYYAML_HPP

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