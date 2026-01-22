/**
 * @file myyaml.cpp
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
 * SECTIONS:
 *
 * DOCUMENTATION:
 *
 * CONTRIBUTORS:
 *
 * TODO:
 *
 * HELP:
 *    - See links below.
 *    - Read top of myyaml.hpp for more details and comments.
 *
 *  Has only had a few tests run, may have issues.
 *
 *  If having issues compiling/linking/running raise an issue (https://github.com/djoezeke/myyaml/issues).
 *  Please post data https://github.com/djoezeke/myyaml/discussions if you cannot find a solution data resources above.
 *
 * RESOURCES:
 * - Homepage ................... https://github.com/djoezeke/myyaml
 * - Releases & changelog ....... https://github.com/djoezeke/myyaml/releases
 * - Issues & support ........... https://github.com/djoezeke/myyaml/issues
 *
 */

// clang-format off

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

// clang-format on

//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#include <myyaml/myyaml.hpp>

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>  //
#include <stdlib.h> //
#include <string.h> //

//-------------------------------------------------------------------------
// [SECTION] Defines
//-----------------------------------------------------------------------------

// clang-format off

/** Byte order marks. */

#ifndef MYYAML_BOM_UTF8
    #define MYYAML_BOM_UTF8 "\xef\xbb\xbf"
#endif // MYYAML_BOM_UTF8

#ifndef MYYAML_BOM_UTF16LE
    #define MYYAML_BOM_UTF16LE "\xff\xfe"
#endif // MYYAML_BOM_UTF16LE

#ifndef MYYAML_BOM_UTF16BE
    #define MYYAML_BOM_UTF16BE "\xfe\xff"
#endif // MYYAML_BOM_UTF16BE

#ifndef MYYAML_BOM_UTF32LE
    #define MYYAML_BOM_UTF32LE "\xff\xfe\x00\x00"
#endif // MYYAML_BOM_UTF32LE

#ifndef MYYAML_BOM_UTF32BE
    #define MYYAML_BOM_UTF32BE "\x00\x00\xfe\xff"
#endif // MYYAML_BOM_UTF32BE

// clang-format on

#pragma region Detail

namespace myyaml
{
    namespace detail
    {

        /**
         * @defgroup input
         * @brief
         * @{
         */

        //-----------------------------------------------------------------------------
        // [Class] file_iadapter
        //-----------------------------------------------------------------------------
        // - file_iadapter()
        // - encoding()
        // - read()
        //-----------------------------------------------------------------------------

        file_iadapter::file_iadapter(std::FILE *file)
            : m_encoding(myyaml::encoding::Unspecified), m_file(file)
        {
        }

        file_iadapter::file_iadapter(std::FILE *file, myyaml::encoding encoding)
            : m_encoding(encoding), m_file(file)
        {
        }

        myyaml::encoding file_iadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t file_iadapter::read(void *data, std::size_t size)
        {
            if (!m_file || !data || size == 0)
            {
                return 0;
            }
            // fread returns number of elements read; using 1-byte elements.
            std::size_t n = std::fread(data, 1, size, m_file);
            return n;
        };

        //-----------------------------------------------------------------------------
        // [Class] stream_iadapter
        //-----------------------------------------------------------------------------
        // - stream_iadapter()
        // - encoding()
        // - read()
        //-----------------------------------------------------------------------------

        stream_iadapter::stream_iadapter(std::istream &stream)
            : m_encoding(myyaml::encoding::Unspecified), m_stream(&stream)
        {
        }

        stream_iadapter::stream_iadapter(std::istream &stream, myyaml::encoding encoding)
            : m_encoding(encoding), m_stream(&stream)
        {
        }

        myyaml::encoding stream_iadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t stream_iadapter::read(void *data, std::size_t size)
        {
            if (!m_stream || !data || size == 0)
            {
                return 0;
            }
            // Read using istream::readsome when available to avoid blocking, but
            // fall back to read() which will block until requested size or EOF.
            std::istream &s = *m_stream;
            s.read(static_cast<char *>(data), static_cast<std::streamsize>(size));
            std::streamsize got = s.gcount();
            if (got < 0)
                return 0;
            return static_cast<std::size_t>(got);
        };

        //-----------------------------------------------------------------------------
        // [Class] memory_iadapter
        //-----------------------------------------------------------------------------
        // - memory_iadapter()
        // - encoding()
        // - read()
        //-----------------------------------------------------------------------------

        memory_iadapter::memory_iadapter(void *data, std::size_t size)
            : m_encoding(myyaml::encoding::Unspecified), m_pos(0), m_size(size), m_data(data)
        {
        }

        memory_iadapter::memory_iadapter(void *data, std::size_t size, myyaml::encoding encoding)
            : m_encoding(encoding), m_pos(0), m_size(size), m_data(data)
        {
        }

        myyaml::encoding memory_iadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t memory_iadapter::read(void *data, std::size_t size)
        {
            if (!data || size == 0 || m_data == nullptr)
                return 0;
            // remaining bytes
            if (m_pos >= m_size)
                return 0;
            std::size_t remaining = m_size - m_pos;
            std::size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(data, static_cast<char *>(m_data) + m_pos, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

        //-----------------------------------------------------------------------------
        // [Class] lexer
        //-----------------------------------------------------------------------------
        // - lexer()
        //-----------------------------------------------------------------------------

        lexer::lexer(iadapter &adapter)
            : m_adapter(adapter), m_position{}, m_token{}, m_string(), m_float(0.0f), m_int(0)
        {
        }

        // int lexer::get_char() {
        // };

        void lexer::unget_char() {
        };

        void lexer::add_char(int c) {
        };

        token lexer::get_token()
        {
            return m_token;
        }

        token_t lexer::get_type()
        {
            return m_token.type;
        }

        /** @} group input */

        /**
         * @defgroup output
         * @brief
         * @{
         */

        //-----------------------------------------------------------------------------
        // [Class] file_oadapter
        //-----------------------------------------------------------------------------
        // - file_oadapter()
        // - encoding()
        // - write()
        //-----------------------------------------------------------------------------

        file_oadapter::file_oadapter(std::FILE *file)
            : m_encoding(myyaml::encoding::Unspecified), m_file(file)
        {
        }

        file_oadapter::file_oadapter(std::FILE *file, myyaml::encoding encoding)
            : m_encoding(encoding), m_file(file)
        {
        }

        myyaml::encoding file_oadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t file_oadapter::write(const void *data, std::size_t size)
        {
            if (!m_file || !data || size == 0)
                return 0;
            std::size_t n = std::fwrite(data, 1, size, m_file);
            return n;
        };

        //-----------------------------------------------------------------------------
        // [Class] stream_oadapter
        //-----------------------------------------------------------------------------
        // - stream_oadapter()
        // - encoding()
        // - write()
        //-----------------------------------------------------------------------------

        stream_oadapter::stream_oadapter(std::ostream &stream)
            : m_encoding(myyaml::encoding::Unspecified), m_stream(&stream)
        {
        }

        stream_oadapter::stream_oadapter(std::ostream &stream, myyaml::encoding encoding)
            : m_encoding(encoding), m_stream(&stream)
        {
        }

        myyaml::encoding stream_oadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t stream_oadapter::write(const void *data, std::size_t size)
        {
            if (!m_stream || !data || size == 0)
                return 0;
            std::ostream &s = *m_stream;
            s.write(static_cast<const char *>(data), static_cast<std::streamsize>(size));
            if (!s.good())
                return 0;
            return size;
        };

        //-----------------------------------------------------------------------------
        // [Class] memory_oadapter
        //-----------------------------------------------------------------------------
        // - memory_oadapter()
        // - encoding()
        // - write()
        //-----------------------------------------------------------------------------

        memory_oadapter::memory_oadapter(void *data, std::size_t size)
            : m_encoding(myyaml::encoding::Unspecified), m_pos(0), m_size(size), m_data(data)
        {
        }

        memory_oadapter::memory_oadapter(void *data, std::size_t size, myyaml::encoding encoding)
            : m_encoding(encoding), m_pos(0), m_size(size), m_data(data)
        {
        }

        myyaml::encoding memory_oadapter::encoding()
        {
            return m_encoding;
        };

        std::size_t memory_oadapter::write(const void *data, std::size_t size)
        {
            if (!data || size == 0 || m_data == nullptr)
                return 0;
            // remaining space
            if (m_pos >= m_size)
                return 0;
            std::size_t remaining = m_size - m_pos;
            std::size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(static_cast<char *>(m_data) + m_pos, data, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

        /** @} group output */

        //-----------------------------------------------------------------------------
        // [SECTION] Functions Definitions : Detail
        //-----------------------------------------------------------------------------

    }; // namespace detail
}; // namespace myyaml

#pragma endregion // Detail

#pragma region Myyaml

namespace myyaml
{

    //-----------------------------------------------------------------------------
    // [Class] version
    //-----------------------------------------------------------------------------
    // Semver versioning
    //-----------------------------------------------------------------------------
    // - version()
    // - operator<()
    // - operator>()
    // - operator==()
    // - operator!=()
    // - operator<=()
    // - operator>=()
    // - operator<<()
    //-----------------------------------------------------------------------------

    version::version(int major, int minor, int patch)
        : major(major), minor(minor), patch(patch) {}

    bool operator<(const version &lhs, const version &rhs) noexcept
    {
        return lhs.major < rhs.major || (lhs.major == rhs.major && lhs.minor < rhs.minor) ||
               (lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch < rhs.patch);
    };

    bool operator>(const version &lhs, const version &rhs) noexcept { return rhs < lhs; };

    bool operator==(const version &lhs, const version &rhs) noexcept
    {
        return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
    };

    bool operator!=(const version &lhs, const version &rhs) noexcept { return !(lhs == rhs); };

    bool operator<=(const version &lhs, const version &rhs) noexcept { return !(rhs < lhs); };

    bool operator>=(const version &lhs, const version &rhs) noexcept { return !(lhs < rhs); };

    std::ostream &operator<<(std::ostream &ostream, const version &version)
    {
        ostream << version.major << '.' << version.minor << '.' << version.patch;
        return ostream;
    };

    //-----------------------------------------------------------------------------
    // [Class] exception
    //-----------------------------------------------------------------------------
    // - exception()
    //-----------------------------------------------------------------------------

    exception::exception(const char *message) noexcept
    {
        if (message)
        {
            m_Message = message;
        }
    }

    const char *exception::what() const noexcept { return m_Message.c_str(); }

    //-----------------------------------------------------------------------------
    // [SECTION] Functions Definitions
    //-----------------------------------------------------------------------------
    // - operator<<()
    // - operator>>()
    //-----------------------------------------------------------------------------

    std::ostream &operator<<(std::ostream &stream, const yaml &node) {};

    std::istream &operator>>(std::istream &stream, const yaml &node) {};

}; // namespace myyaml

#pragma endregion // Myyaml

#pragma region Literal

#if 0

namespace myyaml
{

    namespace literals
    {

        //-----------------------------------------------------------------------------
        // [SECTION] Functions Definitions : Literals
        //-----------------------------------------------------------------------------

        inline yaml MYYAML_QUOTE_OPERATOR(const char *string, std::size_t size) {};

#if MYYAML_HAS_CHAR8_T

        inline yaml MYYAML_QUOTE_OPERATOR(const char8_t *string, std::size_t size) {};

#endif // MYYAML_HAS_CHAR8_T

        inline yaml MYYAML_QUOTE_OPERATOR(const char16_t *string, std::size_t size) {};

        inline yaml MYYAML_QUOTE_OPERATOR(const char32_t *string, std::size_t size) {};

    }; // namespace literals

}; // namespace myyaml

#endif // 0

#pragma endregion // Literal

/**
 * LICENSE: MIT License
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * data the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included data
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