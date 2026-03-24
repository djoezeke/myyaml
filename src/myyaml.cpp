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
    MYYAML_VERSION_NAMESPACE_BEGIN

    namespace detail
    {

        //-------------------------------------------------------------------------
        // [SECTION] Details : Encoding
        //-------------------------------------------------------------------------

        encoding determine_encoding(void *data, size_t size)
        {
            if (size >= 4 && !memcmp(data, MYYAML_BOM_UTF32BE, 4))
            {
                return encoding::utf32be;
            }
            if (size >= 4 && !memcmp(data, MYYAML_BOM_UTF32LE, 4))
            {
                return encoding::utf32le;
            }
            if (size >= 3 && !memcmp(data, MYYAML_BOM_UTF8, 3))
            {
                return encoding::utf8;
            }
            if (size >= 2 && !memcmp(data, MYYAML_BOM_UTF16BE, 2))
            {
                return encoding::utf16be;
            }
            if (size >= 2 && !memcmp(data, MYYAML_BOM_UTF16LE, 2))
            {
                return encoding::utf16le;
            }
            return encoding::unspecified;
        };

        /*
         * Decode a UTF-8 character.  Check RFC 3629
         * (https://datatracker.ietf.org/doc/html/rfc3629) for more details.
         *
         * The following table (taken from the RFC) is used for
         * decoding.
         *
         * The table below summarizes the format of these different octet types.
         * The letter x indicates bits available for encoding bits of the
         * character number.

         *    Char. number range |        UTF-8 octet sequence
         *      (hexadecimal)    |              (binary)
         *   --------------------+------------------------------------
         *   0000 0000-0000 007F | 0xxxxxxx
         *   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
         *   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
         *   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
         *
         * The definition of UTF-8 prohibits encoding character numbers between
         * U+D800 and U+DFFF, which are reserved for use with the UTF-16
         * encoding form (as surrogate pairs) and do not directly represent
         * characters. When encoding in UTF-8 from UTF-16 data, it is necessary
         * to first decode the UTF-16 data to obtain character numbers, which
         * are then encoded in UTF-8 as described above.
         *
         * (https://en.wikipedia.org/wiki/UTF-8)
         */

        int utf8::decode(const char *data, size_t size, unsigned int &value)
        {
            if (size == 0 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            unsigned char byte = static_cast<unsigned char>(data[0]);
            if (byte < 0x80)
            {
                value = byte;
                return 1;
            }
            if ((byte & 0xE0) == 0xC0)
            {
                if (size < 2)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x1F) << 6) | (static_cast<unsigned char>(data[1]) & 0x3F);
                return 2;
            }
            if ((byte & 0xF0) == 0xE0)
            {
                if (size < 3)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x0F) << 12) | ((static_cast<unsigned char>(data[1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(data[2]) & 0x3F);
                return 3;
            }
            if ((byte & 0xF8) == 0xF0)
            {
                if (size < 4)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x07) << 18) | ((static_cast<unsigned char>(data[1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(data[2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(data[3]) & 0x3F);
                return 4;
            }
            value = 0;
            return -1;
        };

        int utf8::encode(unsigned int codepoint, utf8::char_t *output, size_t size)
        {
            if (size < 1)
            {
                return -1;
            }

            if (codepoint < 0x80)
            {
                output[0] = static_cast<char>(codepoint);
                return 1;
            }

            if (codepoint < 0x800)
            {
                if (size < 2)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 6) | 0xC0);
                output[1] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 2;
            }

            if (codepoint < 0x10000)
            {
                if (size < 3)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 12) | 0xE0);
                output[1] = static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
                output[2] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 3;
            }

            if (codepoint <= 0x10FFFF)
            {
                if (size < 4)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 18) | 0xF0);
                output[1] = static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80);
                output[2] = static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
                output[3] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 4;
            }

            return -1;
        };

        std::vector<unsigned char> utf8::to_utf16(const std::string &string, endian order)
        {
            std::vector<unsigned char> out;
            size_t idx = 0;
            while (idx < string.size())
            {
                unsigned int cp = 0;
                int dec = utf8::decode(string.data() + idx, string.size() - idx, cp);
                MYYAML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf16::encode(cp, reinterpret_cast<utf16::char_t *>(tmp), sizeof(tmp), order);
                MYYAML_ASSERT(enc > 0);
                for (int i = 0; i < enc * 2; ++i)
                    out.push_back(static_cast<unsigned char>(tmp[i]));
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        std::vector<unsigned char> utf8::to_utf32(const std::string &string, endian order)
        {
            std::vector<unsigned char> out;
            size_t idx = 0;
            while (idx < string.size())
            {
                unsigned int cp = 0;
                int dec = utf8::decode(string.data() + idx, string.size() - idx, cp);
                MYYAML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf32::encode(cp, reinterpret_cast<utf32::char_t *>(tmp), sizeof(tmp), order);
                MYYAML_ASSERT(enc > 0);
                for (int i = 0; i < 4; ++i)
                    out.push_back(static_cast<unsigned char>(tmp[i]));
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        /*
         * Decode a UTF-16 character.  Check RFC 2781
         * (https://datatracker.ietf.org/doc/html/rfc2781).
         *
         * Normally, two subsequent bytes describe a Unicode
         * character.  However a special technique (called a
         * surrogate pair) is used for specifying character
         * values larger than 0xFFFF.
         *
         * A surrogate pair consists of two pseudo-characters:
         *      high surrogate area (0xD800-0xDBFF)
         *      low surrogate area (0xDC00-0xDFFF)
         *
         * The following formulas are used for decoding
         * and encoding characters using surrogate pairs:
         *
         *  U  = U' + 0x10000   (0x01 00 00 <= U <= 0x10 FF FF)
         *  U' = yyyyyyyyyyxxxxxxxxxx   (0 <= U' <= 0x0F FF FF)
         *  W1 = 110110yyyyyyyyyy
         *  W2 = 110111xxxxxxxxxx
         *
         * where U is the character value, W1 is the high surrogate
         * area, W2 is the low surrogate area.
         *
         * (https://en.wikipedia.org/wiki/UTF-16)
         *
         */

        int utf16::decode(const char *data, size_t size, unsigned int &value, endian order)
        {
            if (size < 2 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            const unsigned char *b = reinterpret_cast<const unsigned char *>(data);

            auto read_u16 = [&](size_t offset) -> uint16_t
            {
                if (order == endian::native)
                {
                    uint16_t tmp = 0;
                    memcpy(&tmp, b + offset, 2);
                    return tmp;
                }
                else if (order == endian::little)
                {
                    return static_cast<uint16_t>(b[offset] | (b[offset + 1] << 8));
                }
                else // big-endian
                {
                    return static_cast<uint16_t>((b[offset] << 8) | b[offset + 1]);
                }
            };

            uint16_t w1 = read_u16(0);

            // Check for surrogate pair (high surrogate 0xD800-0xDBFF)
            if (w1 >= 0xD800 && w1 <= 0xDBFF)
            {
                if (size < 4)
                {
                    value = 0;
                    return -1;
                }
                uint16_t w2 = read_u16(2);
                if (!(w2 >= 0xDC00 && w2 <= 0xDFFF))
                {
                    value = 0;
                    return -1;
                }
                unsigned int high = static_cast<unsigned int>(w1);
                unsigned int low = static_cast<unsigned int>(w2);
                value = ((high - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
                return 4;
            }

            value = static_cast<unsigned int>(w1);
            return 2;
        };

        int utf16::encode(unsigned int codepoint, utf16::char_t *output, size_t size, endian order)
        {
            if (size < 2 || output == nullptr)
            {
                return -1;
            }

            unsigned char *out = reinterpret_cast<unsigned char *>(output);

            auto write_u16 = [&](size_t offset, uint16_t val)
            {
                if (order == endian::native)
                {
                    memcpy(out + offset, &val, 2);
                }
                else if (order == endian::little)
                {
                    out[offset] = static_cast<unsigned char>(val & 0xFF);
                    out[offset + 1] = static_cast<unsigned char>((val >> 8) & 0xFF);
                }
                else // big-endian
                {
                    out[offset] = static_cast<unsigned char>((val >> 8) & 0xFF);
                    out[offset + 1] = static_cast<unsigned char>(val & 0xFF);
                }
            };

            if (codepoint < 0x10000)
            {
                uint16_t val = static_cast<uint16_t>(codepoint);
                write_u16(0, val);
                return 1;
            }

            if (codepoint > 0x10FFFF)
            {
                return -1;
            }

            if (size < 4)
            {
                return -1;
            }

            unsigned int cp = codepoint - 0x10000;
            uint16_t w1 = static_cast<uint16_t>((cp >> 10) + 0xD800);
            uint16_t w2 = static_cast<uint16_t>((cp & 0x3FF) + 0xDC00);
            write_u16(0, w1);
            write_u16(2, w2);
            return 2;
        };

        std::string utf16::to_utf8(const std::vector<unsigned char> &bytes, endian order)
        {
            std::string out;
            size_t idx = 0;
            while (idx < bytes.size())
            {
                unsigned int cp = 0;
                int dec = utf16::decode(reinterpret_cast<const char *>(bytes.data() + idx), bytes.size() - idx, cp, order);
                MYYAML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf8::encode(cp, reinterpret_cast<utf8::char_t *>(tmp), sizeof(tmp));
                MYYAML_ASSERT(enc > 0);
                out.append(tmp, tmp + enc);
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        int utf32::decode(const char *data, size_t size, unsigned int &value, endian order)
        {
            if (size < 4 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            const unsigned char *b = reinterpret_cast<const unsigned char *>(data);
            uint32_t tmp = 0;

            if (order == endian::native)
            {
                memcpy(&tmp, b, 4);
            }
            else if (order == endian::little)
            {
                tmp = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
            }
            else // big-endian
            {
                tmp = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
            }

            value = static_cast<unsigned int>(tmp);
            return 4;
        };

        int utf32::encode(unsigned int codepoint, utf32::char_t *output, size_t size, endian order)
        {
            if (size < 4 || output == nullptr)
            {
                return -1;
            }

            uint32_t cp = static_cast<uint32_t>(codepoint);
            unsigned char *out = reinterpret_cast<unsigned char *>(output);

            if (order == endian::native)
            {
                memcpy(out, &cp, 4);
            }
            else if (order == endian::little)
            {
                out[0] = static_cast<unsigned char>(cp & 0xFF);
                out[1] = static_cast<unsigned char>((cp >> 8) & 0xFF);
                out[2] = static_cast<unsigned char>((cp >> 16) & 0xFF);
                out[3] = static_cast<unsigned char>((cp >> 24) & 0xFF);
            }
            else // big-endian
            {
                out[0] = static_cast<unsigned char>((cp >> 24) & 0xFF);
                out[1] = static_cast<unsigned char>((cp >> 16) & 0xFF);
                out[2] = static_cast<unsigned char>((cp >> 8) & 0xFF);
                out[3] = static_cast<unsigned char>(cp & 0xFF);
            }

            return 1;
        };

        std::string utf32::to_utf8(const std::vector<unsigned char> &bytes, endian order)
        {
            std::string out;
            size_t idx = 0;
            while (idx + 4 <= bytes.size())
            {
                unsigned int cp = 0;
                int dec = utf32::decode(reinterpret_cast<const char *>(bytes.data() + idx), bytes.size() - idx, cp, order);
                MYYAML_ASSERT(dec == 4);
                char tmp[4];
                int enc = utf8::encode(cp, reinterpret_cast<utf8::char_t *>(tmp), sizeof(tmp));
                MYYAML_ASSERT(enc > 0);
                out.append(tmp, tmp + enc);
                idx += 4;
            }
            return out;
        };

        //-------------------------------------------------------------------------
        // [SECTION] Details : Input
        //-------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // [Class] file_iadapter
        //-----------------------------------------------------------------------------
        // - file_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        file_iadapter::file_iadapter(FILE *file)
            : m_file(file) {};

        size_t file_iadapter::read(void *data, size_t size)
        {
            if (m_file == nullptr || data == nullptr || size == 0)
            {
                return 0;
            }
            // fread returns number of elements read; using 1-byte elements.
            const size_t nsize = fread(data, 1, size, m_file);
            return nsize;
        };

#ifndef MYYAML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] stream_iadapter
        //-----------------------------------------------------------------------------
        // - stream_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        stream_iadapter::stream_iadapter(std::istream &stream)
            : m_stream(&stream) {};

        size_t stream_iadapter::read(void *data, size_t size)
        {
            if ((m_stream == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            // Read using istream::readsome when available to avoid blocking, but
            // fall back to read() which will block until requested size or EOF.
            std::istream &stream = *m_stream;
            stream.read(static_cast<char *>(data), static_cast<std::streamsize>(size));
            const std::streamsize got = stream.gcount();
            if (got < 0)
            {
                return 0;
            }
            return static_cast<size_t>(got);
        };

#endif // MYYAML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] memory_iadapter
        //-----------------------------------------------------------------------------
        // - memory_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        memory_iadapter::memory_iadapter(void *data, size_t size)
            : m_pos(0), m_size(size), m_data(data) {};

        size_t memory_iadapter::read(void *data, size_t size)
        {
            if ((data == nullptr) || size == 0 || m_data == nullptr)
            {
                return 0;
            }
            // remaining bytes
            if (m_pos >= m_size)
            {
                return 0;
            }
            const size_t remaining = m_size - m_pos;
            const size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(data, static_cast<char *>(m_data) + m_pos, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

        //-----------------------------------------------------------------------------
        // [Class] lexer
        //-----------------------------------------------------------------------------
        // - lexer()
        //-----------------------------------------------------------------------------

        // Lexer constructor: read the whole input from the adapter, detect
        // encoding (using BOM when present), strip the BOM and convert the
        // payload into UTF-8 bytes stored in m_input. This approach simplifies
        // tokenization: subsequent get_char() returns UTF-8 bytes only.
        lexer::lexer(iadapter *adapter)
            : m_position{}, m_token{}, m_string(), m_adapter(adapter), m_encoding(myyaml::encoding::unspecified), m_putback(-1)
        {
            if (!m_adapter)
            {
                m_input_pos = 0;
                return;
            }

            // Read a small header to probe encoding (BOM detection)
            unsigned char header[4] = {0};
            size_t got_header = m_adapter->read(header, sizeof(header));
            if (got_header > 0)
            {
                m_encoding = determine_encoding(header, got_header);
            }

            // Read the rest of the adapter into a raw buffer. We already
            // consumed header bytes from the adapter by reading them above,
            // so append the remainder as well.
            std::vector<unsigned char> raw;
            // start with header bytes we read (they came from adapter)
            for (size_t i = 0; i < got_header; ++i)
                raw.push_back(header[i]);

            // Read until adapter returns 0
            while (true)
            {
                char buf[4096];
                size_t n = m_adapter->read(buf, sizeof(buf));
                if (n == 0)
                    break;
                raw.insert(raw.end(), reinterpret_cast<unsigned char *>(buf), reinterpret_cast<unsigned char *>(buf) + n);
            }

            // Strip BOM bytes if present and convert to UTF-8 when needed.
            // If encoding is unspecified, prefer UTF-8 semantics (no conversion).
            size_t skip = 0;
            switch (m_encoding)
            {
            case myyaml::encoding::utf8:
                // UTF-8 BOM is 3 bytes
                if (raw.size() >= 3 && !memcmp(raw.data(), MYYAML_BOM_UTF8, 3))
                    skip = 3;
                break;
            case myyaml::encoding::utf16le:
            case myyaml::encoding::utf16be:
                // UTF-16 BOM is 2 bytes
                if (raw.size() >= 2 && (!memcmp(raw.data(), MYYAML_BOM_UTF16LE, 2) || !memcmp(raw.data(), MYYAML_BOM_UTF16BE, 2)))
                    skip = 2;
                break;
            case myyaml::encoding::utf32le:
            case myyaml::encoding::utf32be:
                // UTF-32 BOM is 4 bytes
                if (raw.size() >= 4 && (!memcmp(raw.data(), MYYAML_BOM_UTF32LE, 4) || !memcmp(raw.data(), MYYAML_BOM_UTF32BE, 4)))
                    skip = 4;
                break;
            default:
                break;
            }

            // Create a vector without BOM for conversion/consumption
            std::vector<unsigned char> payload;
            if (skip < raw.size())
                payload.insert(payload.end(), raw.begin() + skip, raw.end());

            // Convert to UTF-8 if necessary, otherwise copy bytes directly.
            if (m_encoding == myyaml::encoding::utf16le || m_encoding == myyaml::encoding::utf16be || m_encoding == myyaml::encoding::utf16)
            {
                endian order = (m_encoding == myyaml::encoding::utf16be) ? endian::big : endian::little;
                std::string utf8 = utf16::to_utf8(payload, order);
                m_input.assign(utf8.begin(), utf8.end());
            }
            else if (m_encoding == myyaml::encoding::utf32le || m_encoding == myyaml::encoding::utf32be || m_encoding == myyaml::encoding::utf32)
            {
                endian order = (m_encoding == myyaml::encoding::utf32be) ? endian::big : endian::little;
                std::string utf8 = utf32::to_utf8(payload, order);
                m_input.assign(utf8.begin(), utf8.end());
            }
            else
            {
                // Treat as UTF-8 or unspecified: copy bytes directly
                m_input.assign(reinterpret_cast<char *>(payload.data()), reinterpret_cast<char *>(payload.data()) + payload.size());
            }

            m_input_pos = 0;
        };

        token lexer::scan()
        {
            m_token.text.clear();
            m_token.type = token_t::unknown;
            m_char = skip_ws();

            if (m_char == EOF)
            {
                m_token.type = token_t::end_of_input;
                return m_token;
            }
            m_token.type = token_t::unknown;
            return m_token;
        };

        mark lexer::position() const
        {
            return m_position;
        };

        void lexer::advance(size_t amount)
        {
            int times = 1;
            do
            {
                m_position.index += 1;
                m_position.column += 1;
                if (m_char == '\n')
                {
                    m_position.line += 1;
                    m_position.column = 0;
                }

                if (m_position.index < m_input.size())
                {
                    m_char = m_input[m_position.index];
                }

            } while (times != amount);
        };

        void lexer::reverse(size_t amount)
        {
            int times = 1;
            do
            {
                m_position.index -= 1;
                m_position.column -= 1;
                if (m_char == '\n')
                {
                    m_position.line -= 1;
                    m_position.column = 0;
                }

                if (m_position.index > 0)
                {
                    m_char = m_input[m_position.index];
                }

            } while (times != amount);
        };

        bool lexer::scan_literal()
        {
            std::string string;
            if ((m_char >= 'a' && m_char <= 'z') || (m_char >= 'A' && m_char <= 'Z'))
            {
                // string.push_back(static_cast<char>(m_char));
                while (true)
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    if ((m_char >= 'a' && m_char <= 'z') || (m_char >= 'A' && m_char <= 'Z'))
                    {
                        string.push_back(static_cast<char>(m_char));
                        continue;
                    }
                    reverse();
                    break;
                }
                if (string == "false")
                {
                    m_token.type = token_t::false_literal;
                    m_token.text = string;
                    return true;
                }
                if (string == "true")
                {
                    m_token.type = token_t::true_literal;
                    m_token.text = string;
                    return true;
                }
                if (string == "null")
                {
                    m_token.type = token_t::null_literal;
                    return true;
                }
            }
            return false;
        };

        bool lexer::scan_comment()
        {
            std::string string;
            switch (m_char)
            {
            case '/':
            {
                while (true)
                {
                    advance();
                    switch (m_char)
                    {
                    case '\n':
                    case '\r':
                    case '\0':
                        return true;
                    default:
                        break;
                    }
                }
            }

            case '*':
            {
                while (true)
                {
                    advance();
                    switch (m_char)
                    {
                    case '\0':
                    {
                        return false;
                    }

                    case '*':
                    {
                        advance();
                        switch (m_char)
                        {
                        case '/':
                            return true;
                        default:
                        {
                            reverse();
                            continue;
                        }
                        }
                    }
                    default:
                        continue;
                    }
                }
            }
            default:
            {
                return false;
            }
            }
        };

        bool lexer::scan_string()
        {
            std::string string;
            while (true)
            {
                advance();
                if (m_char == EOF)
                {
                    m_token.type = token_t::string_value;
                    return true;
                }
                if (m_char == '\\')
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    string.push_back(static_cast<char>('\\'));
                    string.push_back(static_cast<char>(m_char));
                    continue;
                }
                if (m_char == '"')
                {
                    m_token.type = token_t::string_value;
                    m_token.text = string;
                    return true;
                }
                string.push_back(static_cast<char>(m_char));
            }
            return false;
        };

        bool lexer::scan_number()
        {
            std::string string;
            if ((m_char >= '0' && m_char <= '9') || m_char == '-')
            {
                string.push_back(static_cast<char>(m_char));
                while (true)
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    if ((m_char >= '0' && m_char <= '9') || m_char == '+' || m_char == '-' || m_char == 'e' || m_char == 'E' || m_char == '.')
                    {
                        string.push_back(static_cast<char>(m_char));
                        continue;
                    }
                    reverse();
                    break;
                }
                m_token.type = token_t::number_value;
                m_token.text = string;
                return true;
            }
            return false;
        };

        [[nodiscard]] int lexer::get_char()
        {
            // return putback if available
            if (m_putback != -1)
            {
                int c = m_putback;
                m_putback = -1;
                return c;
            }

            // ensure input buffer has data; if empty, read more from adapter
            if (m_input_pos >= m_input.size())
            {
                if (!m_adapter)
                    return EOF;
                char buf[512];
                size_t n = m_adapter->read(buf, sizeof(buf));
                if (n == 0)
                    return EOF;
                m_input.append(buf, buf + n);
            }

            unsigned char c = static_cast<unsigned char>(m_input[m_input_pos++]);
            return static_cast<int>(c);
        }

        void lexer::unget_char()
        {
            // allow single-char putback
            if (m_input_pos > 0)
            {
                --m_input_pos;
            }
        }

        void lexer::add_char(int character)
        {
            m_string.push_back(static_cast<char>(character));
        }

        token lexer::get_token()
        {
            return m_token;
        }

        token_t lexer::get_type() const
        {
            return m_token.type;
        }

        // Helper: skip whitespace and return first non-space character (but
        // leave it buffered via putback semantics)
        int lexer::skip_ws()
        {
            int c;
            do
            {
                c = get_char();
                if (c == EOF)
                    return EOF;
            } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
            // put it back for caller
            unget_char();
            return c;
        }

        token lexer::next_token()
        {
            m_string.clear();
            m_token.text.clear();
            m_token.type = token_t::unknown;

            int c = skip_ws();
            if (c == EOF)
            {
                m_token.type = token_t::end_of_input;
                return m_token;
            }

            c = get_char();

            switch (c)
            {
            case '{':
                m_token.type = token_t::object_start;
                return m_token;
            case '}':
                m_token.type = token_t::object_end;
                return m_token;
            case '[':
                m_token.type = token_t::array_start;
                return m_token;
            case ']':
                m_token.type = token_t::array_end;
                return m_token;
            case ',':
                m_token.type = token_t::value_separator;
                return m_token;
            case ':':
                m_token.type = token_t::name_separator;
                return m_token;
            case '"':
            {
                // parse string
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                    {
                        m_token.type = token_t::string_value;
                        return m_token;
                    }
                    if (ch == '\\')
                    {
                        int esc = get_char();
                        if (esc == EOF)
                            break;
                        // handle simple escapes; for brevity we append raw
                        // sequences; a complete implementation would decode
                        // unicode escapes.
                        add_char('\\');
                        add_char(esc);
                        continue;
                    }
                    if (ch == '"')
                    {
                        m_token.type = token_t::string_value;
                        m_token.text = m_string;
                        return m_token;
                    }
                    add_char(ch);
                }
                break;
            }
            default:
                break;
            }

            // numbers, literals (true,false,null)
            if ((c >= '0' && c <= '9') || c == '-')
            {
                // number
                m_string.push_back(static_cast<char>(c));
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                        break;
                    if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E' || ch == '.')
                    {
                        m_string.push_back(static_cast<char>(ch));
                        continue;
                    }
                    unget_char();
                    break;
                }
                m_token.type = token_t::number_value;
                m_token.text = m_string;
                return m_token;
            }

            // true, false, null
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                m_string.push_back(static_cast<char>(c));
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                        break;
                    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
                    {
                        m_string.push_back(static_cast<char>(ch));
                        continue;
                    }
                    unget_char();
                    break;
                }

                if (m_string == "false")
                {
                    m_token.type = token_t::false_literal;
                    m_token.text = m_string;
                    return m_token;
                }

                if (m_string == "true")
                {
                    m_token.type = token_t::true_literal;
                    m_token.text = m_string;
                    return m_token;
                }

                if (m_string == "null")
                {
                    m_token.type = token_t::null_literal;
                    return m_token;
                }
            }

            // unknown single char
            m_token.type = token_t::unknown;
            return m_token;
        }

        //-------------------------------------------------------------------------
        // [SECTION] Details : Output
        //-------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // [Class] file_oadapter
        //-----------------------------------------------------------------------------
        // - file_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        file_oadapter::file_oadapter(FILE *file)
            : m_file(file) {};

        size_t file_oadapter::write(const void *data, size_t size)
        {
            if ((m_file == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            const size_t nsize = fwrite(data, 1, size, m_file);
            return nsize;
        };

#ifndef MYYAML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] stream_oadapter
        //-----------------------------------------------------------------------------
        // - stream_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        stream_oadapter::stream_oadapter(std::ostream &stream)
            : m_stream(&stream) {};

        size_t stream_oadapter::write(const void *data, size_t size)
        {
            if ((m_stream == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            std::ostream &stream = *m_stream;
            stream.write(static_cast<const char *>(data), static_cast<std::streamsize>(size));
            if (!stream.good())
            {
                return 0;
            }
            return size;
        };

#endif // MYYAML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] memory_oadapter
        //-----------------------------------------------------------------------------
        // - memory_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        memory_oadapter::memory_oadapter(void *data, size_t size)
            : m_pos(0), m_size(size), m_data(data) {};

        size_t memory_oadapter::write(const void *data, size_t size)
        {
            if ((data == nullptr) || size == 0 || m_data == nullptr)
            {
                return 0;
            }
            // remaining space
            if (m_pos >= m_size)
            {
                return 0;
            }
            const size_t remaining = m_size - m_pos;
            const size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(static_cast<char *>(m_data) + m_pos, data, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

        /** @} group output */

        //-----------------------------------------------------------------------------
        // [SECTION] Functions Definitions : Detail
        //-----------------------------------------------------------------------------

        const char *string(token_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(error_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(event_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(value_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(break_t type)
        {
            switch (type)
            {
            case break_t::cr:
                return "cr";
            case break_t::ln:
                return "ln";
            case break_t::crln:
                return "crln";
            default:
                return "any";
            }
        };

        const char *string(mark type) { return ""; };

        const char *string(event type) { return ""; };

        const char *string(token type) { return ""; };

#ifndef MYYAML_NO_STL

        std::ostream &operator<<(std::ostream &ostream, const token_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const error_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const event_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const value_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const break_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const mark &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const event &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const token &type)
        {
            ostream << string(type);
            return ostream;
        };

#endif // MYYAML_NO_STL

    }; // namespace detail

    MYYAML_VERSION_NAMESPACE_END

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

namespace MYYAML
{

    MYYAML_VERSION_NAMESPACE_BEGIN

    namespace literals
    {

        //-----------------------------------------------------------------------------
        // [SECTION] Literals : Toml
        //-----------------------------------------------------------------------------

        MYYAML_INLINE yaml MYYAML_QUOTE_OPERATOR(const char *string, size_t size)
        {
            return yaml{};
        };

#if MYYAML_HAS_CHAR8_T

        MYYAML_INLINE yaml MYYAML_QUOTE_OPERATOR(const char8_t *string, size_t size)
        {
            return yaml{};
        };

#endif // MYYAML_HAS_CHAR8_T

        MYYAML_INLINE yaml MYYAML_QUOTE_OPERATOR(const char16_t *string, size_t size)
        {
            return yaml{};
        };

        MYYAML_INLINE yaml MYYAML_QUOTE_OPERATOR(const char32_t *string, size_t size)
        {
            return yaml{};
        };

    }; // namespace literals

    MYYAML_VERSION_NAMESPACE_END

}; // namespace MYYAML

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