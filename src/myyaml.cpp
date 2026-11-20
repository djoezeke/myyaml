/**
 * @file myyaml.cpp
 * @brief YAML for modern C++.
 * @details Parser, emitter, encoding helpers and public C++ API implementation.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.2.0
 * @see https://www.github.com/djoezeke/myyaml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Developed by Sackey Ezekiel Etrue and every direct or indirect contributor
 * to the GitHub project. See LICENSE for copyright and licensing details.
 */

#include <myyaml/myyaml.hpp>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <limits>
#include <locale>
#include <map>
#include <sstream>
#include <unordered_map>
#include <variant>

#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
#define MYYAML_HAS_FLOAT_TO_CHARS 1
#else
#define MYYAML_HAS_FLOAT_TO_CHARS 0
#endif

namespace myyaml
{
    inline namespace v0_2_0
    {
        //---------------------------------------------------------------------
        // [SECTION] Exceptions
        //---------------------------------------------------------------------

        exception::exception(std::string message) noexcept
            : m_message(std::move(message))
        {
        }

        const char *exception::what() const noexcept
        {
            return m_message.c_str();
        }

        parse_error::parse_error(std::string message, std::size_t line, std::size_t column,
                                 std::size_t index)
            : exception(build_message(std::move(message), line, column)),
              m_line(line),
              m_column(column),
              m_index(index)
        {
        }

        std::string parse_error::build_message(std::string message, std::size_t line,
                                               std::size_t column)
        {
            if (line != 0)
            {
                message += " (line " + std::to_string(line);
                if (column != 0)
                {
                    message += ", column " + std::to_string(column);
                }
                message += ')';
            }
            return message;
        }

        encoding_error::encoding_error(std::string message) : exception(std::move(message)) {}
        io_error::io_error(std::string message) : exception(std::move(message)) {}
        type_error::type_error(std::string message) : exception(std::move(message)) {}
        lookup_error::lookup_error(std::string message) : exception(std::move(message)) {}

        //---------------------------------------------------------------------
        // [SECTION] Version
        //---------------------------------------------------------------------

        version::version(int major_value, int minor_value, int patch_value) noexcept
            : major(major_value), minor(minor_value), patch(patch_value)
        {
        }

        bool operator<(const version &lhs, const version &rhs) noexcept
        {
            if (lhs.major != rhs.major)
            {
                return lhs.major < rhs.major;
            }
            if (lhs.minor != rhs.minor)
            {
                return lhs.minor < rhs.minor;
            }
            return lhs.patch < rhs.patch;
        }

        bool operator>(const version &lhs, const version &rhs) noexcept { return rhs < lhs; }

        bool operator==(const version &lhs, const version &rhs) noexcept
        {
            return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
        }

        bool operator!=(const version &lhs, const version &rhs) noexcept { return !(lhs == rhs); }
        bool operator<=(const version &lhs, const version &rhs) noexcept { return !(rhs < lhs); }
        bool operator>=(const version &lhs, const version &rhs) noexcept { return !(lhs < rhs); }

        std::ostream &operator<<(std::ostream &stream, const version &value)
        {
            stream << value.major << '.' << value.minor << '.' << value.patch;
            return stream;
        }

        version library_version() noexcept
        {
            return version(MYYAML_VERSION_MAJOR, MYYAML_VERSION_MINOR, MYYAML_VERSION_PATCH);
        }

        const char *library_version_string() noexcept
        {
            return MYYAML_VERSION;
        }

        //---------------------------------------------------------------------
        // [SECTION] Value implementation
        //---------------------------------------------------------------------

        struct yaml::impl
        {
            using storage = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string,
                                         sequence, mapping>;

            storage value;

            impl() noexcept : value(nullptr) {}
            explicit impl(bool v) : value(v) {}
            explicit impl(std::int64_t v) : value(v) {}
            explicit impl(double v) : value(v) {}
            explicit impl(std::string v) : value(std::move(v)) {}
            explicit impl(sequence v) : value(std::move(v)) {}
            explicit impl(mapping v) : value(std::move(v)) {}
        };

        namespace
        {
            [[noreturn]] void throw_type_error(node_t actual, const char *expected)
            {
                throw type_error(std::string("expected ") + expected + ", value is " +
                                 string(actual));
            }

            std::string trim_copy(std::string_view text)
            {
                std::size_t first = 0;
                while (first < text.size() &&
                       (text[first] == ' ' || text[first] == '\t' || text[first] == '\n' ||
                        text[first] == '\r'))
                {
                    ++first;
                }
                std::size_t last = text.size();
                while (last > first &&
                       (text[last - 1] == ' ' || text[last - 1] == '\t' ||
                        text[last - 1] == '\n' || text[last - 1] == '\r'))
                {
                    --last;
                }
                return std::string(text.substr(first, last - first));
            }

            bool iequals(std::string_view lhs, std::string_view rhs) noexcept
            {
                if (lhs.size() != rhs.size())
                {
                    return false;
                }
                for (std::size_t i = 0; i < lhs.size(); ++i)
                {
                    char a = lhs[i];
                    char b = rhs[i];
                    if (a >= 'A' && a <= 'Z')
                    {
                        a = static_cast<char>(a - 'A' + 'a');
                    }
                    if (b >= 'A' && b <= 'Z')
                    {
                        b = static_cast<char>(b - 'A' + 'a');
                    }
                    if (a != b)
                    {
                        return false;
                    }
                }
                return true;
            }

            bool is_space(char c) noexcept { return c == ' ' || c == '\t'; }

            bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }

            int hex_value(char c) noexcept
            {
                if (c >= '0' && c <= '9')
                {
                    return c - '0';
                }
                if (c >= 'a' && c <= 'f')
                {
                    return c - 'a' + 10;
                }
                if (c >= 'A' && c <= 'F')
                {
                    return c - 'A' + 10;
                }
                return -1;
            }

            std::string format_double(double value)
            {
                if (std::isnan(value))
                {
                    return ".nan";
                }
                if (std::isinf(value))
                {
                    return std::signbit(value) ? "-.inf" : ".inf";
                }

                std::string text;
#if MYYAML_HAS_FLOAT_TO_CHARS
                char buffer[64];
                const auto result =
                    std::to_chars(buffer, buffer + sizeof(buffer), value,
                                  std::chars_format::general);
                if (result.ec != std::errc())
                {
                    throw type_error("unable to format floating-point value");
                }
                text.assign(buffer, result.ptr);
#else
                std::ostringstream stream;
                stream.imbue(std::locale::classic());
                stream << std::setprecision(std::numeric_limits<double>::max_digits10)
                       << value;
                text = stream.str();
#endif
                if (text.find('.') == std::string::npos && text.find('e') == std::string::npos &&
                    text.find('E') == std::string::npos)
                {
                    text += ".0";
                }
                // YAML uses .inf and .nan spellings, never inf/nan. to_chars is
                // required to emit one of those by the checks above, but keep a
                // defensive normalization for alternative standard libraries.
                if (text == "inf")
                {
                    text = ".inf";
                }
                else if (text == "-inf")
                {
                    text = "-.inf";
                }
                else if (text == "nan" || text == "-nan")
                {
                    text = ".nan";
                }
                return text;
            }

            std::string format_integer(std::int64_t value)
            {
                char buffer[32];
                const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
                return std::string(buffer, result.ptr);
            }

            std::string escape_double_quoted(std::string_view text)
            {
                std::string out;
                out.reserve(text.size() + 8);
                out.push_back('"');
                for (std::size_t i = 0; i < text.size(); ++i)
                {
                    const unsigned char c = static_cast<unsigned char>(text[i]);
                    switch (c)
                    {
                    case '"':
                        out += "\\\"";
                        break;
                    case '\\':
                        out += "\\\\";
                        break;
                    case '\0':
                        out += "\\0";
                        break;
                    case '\a':
                        out += "\\a";
                        break;
                    case '\b':
                        out += "\\b";
                        break;
                    case '\t':
                        out += "\\t";
                        break;
                    case '\n':
                        out += "\\n";
                        break;
                    case '\v':
                        out += "\\v";
                        break;
                    case '\f':
                        out += "\\f";
                        break;
                    case '\r':
                        out += "\\r";
                        break;
                    case 0x1B:
                        out += "\\e";
                        break;
                    default:
                        if (c < 0x20 || c == 0x7F)
                        {
                            static const char hex[] = "0123456789ABCDEF";
                            out += "\\x";
                            out.push_back(hex[(c >> 4) & 0x0F]);
                            out.push_back(hex[c & 0x0F]);
                        }
                        else
                        {
                            // Non-ASCII bytes are copied verbatim. The caller
                            // validates that the string is well-formed UTF-8.
                            out.push_back(static_cast<char>(c));
                        }
                        break;
                    }
                }
                out.push_back('"');
                return out;
            }

            bool starts_with(std::string_view text, std::string_view prefix) noexcept
            {
                return text.size() >= prefix.size() &&
                       text.compare(0, prefix.size(), prefix) == 0;
            }

            bool is_null_token(std::string_view text) noexcept
            {
                return text.empty() || text == "~" || iequals(text, "null");
            }

            bool parse_boolean(std::string_view text, bool &value) noexcept
            {
                if (iequals(text, "true"))
                {
                    value = true;
                    return true;
                }
                if (iequals(text, "false"))
                {
                    value = false;
                    return true;
                }
                return false;
            }

            bool parse_integer(std::string_view text, std::int64_t &value) noexcept
            {
                if (text.empty())
                {
                    return false;
                }

                bool negative = false;
                std::size_t pos = 0;
                if (text[pos] == '+' || text[pos] == '-')
                {
                    negative = text[pos] == '-';
                    ++pos;
                }
                if (pos >= text.size())
                {
                    return false;
                }

                unsigned base = 10;
                if (text.size() - pos > 2 && text[pos] == '0' &&
                    (text[pos + 1] == 'x' || text[pos + 1] == 'X'))
                {
                    base = 16;
                    pos += 2;
                }
                else if (text.size() - pos > 2 && text[pos] == '0' &&
                         (text[pos + 1] == 'o' || text[pos + 1] == 'O'))
                {
                    base = 8;
                    pos += 2;
                }
                else if (text.size() - pos > 2 && text[pos] == '0' &&
                         (text[pos + 1] == 'b' || text[pos + 1] == 'B'))
                {
                    base = 2;
                    pos += 2;
                }
                else if (text.size() - pos > 1 && text[pos] == '0')
                {
                    // YAML 1.2 core schema does not allow leading zeros.
                    return false;
                }

                if (pos >= text.size())
                {
                    return false;
                }

                const std::uint64_t limit =
                    negative ? static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1u
                             : static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
                std::uint64_t result = 0;
                bool saw_digit = false;
                bool previous_underscore = false;

                for (; pos < text.size(); ++pos)
                {
                    const char c = text[pos];
                    if (c == '_')
                    {
                        if (!saw_digit || previous_underscore)
                        {
                            return false;
                        }
                        previous_underscore = true;
                        continue;
                    }
                    previous_underscore = false;
                    const int digit = hex_value(c);
                    if (digit < 0 || static_cast<unsigned>(digit) >= base)
                    {
                        return false;
                    }
                    if (result > (limit - static_cast<std::uint64_t>(digit)) / base)
                    {
                        return false;
                    }
                    result = result * base + static_cast<std::uint64_t>(digit);
                    saw_digit = true;
                }

                if (!saw_digit || previous_underscore)
                {
                    return false;
                }

                if (negative)
                {
                    if (result == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1u)
                    {
                        value = std::numeric_limits<std::int64_t>::min();
                    }
                    else
                    {
                        value = -static_cast<std::int64_t>(result);
                    }
                }
                else
                {
                    value = static_cast<std::int64_t>(result);
                }
                return true;
            }

            bool parse_floating(std::string_view text, double &value) noexcept
            {
                if (text.empty())
                {
                    return false;
                }

                std::string lower(text);
                for (char &c : lower)
                {
                    if (c >= 'A' && c <= 'Z')
                    {
                        c = static_cast<char>(c - 'A' + 'a');
                    }
                }
                if (lower == ".inf" || lower == "+.inf")
                {
                    value = std::numeric_limits<double>::infinity();
                    return true;
                }
                if (lower == "-.inf")
                {
                    value = -std::numeric_limits<double>::infinity();
                    return true;
                }
                if (lower == ".nan" || lower == "+.nan" || lower == "-.nan")
                {
                    value = std::numeric_limits<double>::quiet_NaN();
                    return true;
                }

                std::size_t pos = 0;
                if (text[pos] == '+' || text[pos] == '-')
                {
                    ++pos;
                }
                if (pos >= text.size())
                {
                    return false;
                }

                bool saw_digit = false;
                bool saw_dot = false;
                bool saw_exponent = false;
                bool previous_underscore = false;
                std::size_t exponent_pos = 0;

                for (; pos < text.size(); ++pos)
                {
                    const char c = text[pos];
                    if (c == '_')
                    {
                        if (!saw_digit || previous_underscore || saw_exponent)
                        {
                            return false;
                        }
                        // An underscore must be surrounded by digits.
                        if (pos + 1 >= text.size() || !is_digit(text[pos + 1]))
                        {
                            return false;
                        }
                        previous_underscore = true;
                        continue;
                    }
                    previous_underscore = false;
                    if (is_digit(c))
                    {
                        saw_digit = true;
                        continue;
                    }
                    if (c == '.' && !saw_dot && !saw_exponent)
                    {
                        saw_dot = true;
                        continue;
                    }
                    if ((c == 'e' || c == 'E') && saw_digit && !saw_exponent)
                    {
                        saw_exponent = true;
                        exponent_pos = pos;
                        if (pos + 1 < text.size() &&
                            (text[pos + 1] == '+' || text[pos + 1] == '-'))
                        {
                            ++pos;
                        }
                        continue;
                    }
                    return false;
                }

                if (!saw_digit || previous_underscore || (!saw_dot && !saw_exponent))
                {
                    // A floating-point literal needs a decimal point or an
                    // exponent. Plain integers are handled by parse_integer().
                    return false;
                }
                if (saw_exponent)
                {
                    // Must have at least one digit after the exponent marker.
                    bool exponent_digit = false;
                    for (std::size_t i = exponent_pos + 1; i < text.size(); ++i)
                    {
                        if (is_digit(text[i]))
                        {
                            exponent_digit = true;
                            break;
                        }
                        if (text[i] == '+' || text[i] == '-')
                        {
                            continue;
                        }
                        return false;
                    }
                    if (!exponent_digit)
                    {
                        return false;
                    }
                }

                std::string cleaned;
                cleaned.reserve(text.size());
                for (char c : text)
                {
                    if (c != '_')
                    {
                        cleaned.push_back(c);
                    }
                }

#if MYYAML_HAS_FLOAT_TO_CHARS
                const auto result = std::from_chars(cleaned.data(), cleaned.data() + cleaned.size(),
                                                    value, std::chars_format::general);
                return result.ec == std::errc() && result.ptr == cleaned.data() + cleaned.size();
#else
                std::istringstream stream(cleaned);
                stream.imbue(std::locale::classic());
                stream >> value;
                return stream && stream.eof();
#endif
            }

            bool is_numeric_token(std::string_view text) noexcept
            {
                std::int64_t integer = 0;
                double floating = 0.0;
                return parse_integer(text, integer) || parse_floating(text, floating);
            }

            bool looks_like_null_or_bool(std::string_view text) noexcept
            {
                bool boolean = false;
                return is_null_token(text) || parse_boolean(text, boolean);
            }

            bool is_plain_safe(std::string_view text)
            {
                if (text.empty())
                {
                    return false;
                }
                if (text.front() == ' ' || text.front() == '\t' || text.back() == ' ' ||
                    text.back() == '\t')
                {
                    return false;
                }
                if (looks_like_null_or_bool(text) || is_numeric_token(text))
                {
                    return false;
                }
                // Document markers must never be emitted as plain scalars.
                if (starts_with(text, "---") || starts_with(text, "..."))
                {
                    return false;
                }

                const char first = text.front();
                switch (first)
                {
                case '-':
                case '?':
                case ':':
                case ',':
                case '[':
                case ']':
                case '{':
                case '}':
                case '#':
                case '&':
                case '*':
                case '!':
                case '|':
                case '>':
                case '\'':
                case '"':
                case '%':
                case '@':
                case '`':
                    return false;
                default:
                    break;
                }

                for (std::size_t i = 0; i < text.size(); ++i)
                {
                    const unsigned char c = static_cast<unsigned char>(text[i]);
                    if (c < 0x20 || c == 0x7F || c == '\t')
                    {
                        return false;
                    }
                    if (c == ':' && (i + 1 == text.size() || text[i + 1] == ' ' ||
                                     text[i + 1] == '\t'))
                    {
                        return false;
                    }
                    if (c == '#' && i > 0 && (text[i - 1] == ' ' || text[i - 1] == '\t'))
                    {
                        return false;
                    }
                    if (c == ',' || c == '[' || c == ']' || c == '{' || c == '}')
                    {
                        return false;
                    }
                    // Quotes inside a plain scalar confuse the flow scanner, so
                    // always emit such strings in double-quoted form.
                    if (c == '\'' || c == '"')
                    {
                        return false;
                    }
                }
                return detail::utf8::validate(std::string(text));
            }

            bool is_plain_key_safe(std::string_view text)
            {
                if (text.empty())
                {
                    return false;
                }
                const unsigned char first = static_cast<unsigned char>(text.front());
                if (!(std::isalnum(first) != 0 || first == '_'))
                {
                    return false;
                }
                for (unsigned char c : text)
                {
                    if (!(std::isalnum(c) != 0 || c == '_' || c == '-' || c == '.' || c == '/'))
                    {
                        return false;
                    }
                }
                return true;
            }

            yaml infer_scalar(std::string_view text)
            {
                const std::string trimmed = trim_copy(text);
                if (is_null_token(trimmed))
                {
                    return yaml();
                }
                bool boolean = false;
                if (parse_boolean(trimmed, boolean))
                {
                    return yaml(boolean);
                }
                std::int64_t integer = 0;
                if (parse_integer(trimmed, integer))
                {
                    return yaml(integer);
                }
                double floating = 0.0;
                if (parse_floating(trimmed, floating))
                {
                    return yaml(floating);
                }
                return yaml(trimmed);
            }

            std::string decode_quoted(std::string_view raw, bool single_quoted,
                                      const detail::mark &position);
        } // namespace

        //---------------------------------------------------------------------
        // [SECTION] yaml basics
        //---------------------------------------------------------------------

        yaml::yaml() : m_impl(std::make_unique<impl>()) {}

        yaml::yaml(std::nullptr_t) noexcept : m_impl(std::make_unique<impl>()) {}

        yaml::yaml(bool value) : m_impl(std::make_unique<impl>(value)) {}
        yaml::yaml(int value) : m_impl(std::make_unique<impl>(static_cast<std::int64_t>(value))) {}
        yaml::yaml(long value) : m_impl(std::make_unique<impl>(static_cast<std::int64_t>(value))) {}
        yaml::yaml(long long value)
            : m_impl(std::make_unique<impl>(static_cast<std::int64_t>(value)))
        {
        }
        yaml::yaml(unsigned int value) : yaml(static_cast<unsigned long long>(value)) {}
        yaml::yaml(unsigned long value) : yaml(static_cast<unsigned long long>(value)) {}

        yaml::yaml(unsigned long long value)
            : m_impl(std::make_unique<impl>(
                  static_cast<std::int64_t>(static_cast<std::uint64_t>(value) >
                                                    static_cast<std::uint64_t>(
                                                        std::numeric_limits<std::int64_t>::max())
                                                ? 0
                                                : value)))
        {
            if (static_cast<std::uint64_t>(value) >
                static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
            {
                throw type_error("unsigned integer does not fit in a signed 64-bit integer");
            }
        }

        yaml::yaml(float value) : m_impl(std::make_unique<impl>(static_cast<double>(value))) {}
        yaml::yaml(double value) : m_impl(std::make_unique<impl>(value)) {}
        yaml::yaml(const char *value)
            : m_impl(std::make_unique<impl>(std::string(value == nullptr ? "" : value)))
        {
        }
        yaml::yaml(const std::string &value) : m_impl(std::make_unique<impl>(value)) {}
        yaml::yaml(std::string &&value) : m_impl(std::make_unique<impl>(std::move(value))) {}
        yaml::yaml(std::string_view value) : m_impl(std::make_unique<impl>(std::string(value))) {}
        yaml::yaml(sequence value) : m_impl(std::make_unique<impl>(std::move(value))) {}
        yaml::yaml(mapping value) : m_impl(std::make_unique<impl>(std::move(value))) {}

        yaml::yaml(const yaml &other)
        {
            if (other.m_impl)
            {
                m_impl = std::make_unique<impl>(*other.m_impl);
            }
            else
            {
                m_impl = std::make_unique<impl>();
            }
        }

        yaml::yaml(yaml &&other) noexcept = default;
        yaml::~yaml() = default;

        yaml &yaml::operator=(const yaml &other)
        {
            if (this != &other)
            {
                if (other.m_impl)
                {
                    m_impl = std::make_unique<impl>(*other.m_impl);
                }
                else
                {
                    m_impl = std::make_unique<impl>();
                }
            }
            return *this;
        }

        yaml &yaml::operator=(yaml &&other) noexcept = default;

        yaml yaml::from_int64(std::int64_t value)
        {
            yaml result;
            result.m_impl->value = value;
            return result;
        }

        yaml yaml::make_sequence()
        {
            yaml result;
            result.m_impl->value = sequence{};
            return result;
        }

        yaml yaml::make_sequence(std::initializer_list<yaml> values)
        {
            return yaml(sequence(values.begin(), values.end()));
        }

        yaml yaml::make_mapping()
        {
            yaml result;
            result.m_impl->value = mapping{};
            return result;
        }

        yaml yaml::make_mapping(std::initializer_list<std::pair<std::string, yaml>> values)
        {
            mapping result(values.begin(), values.end());
            return yaml(std::move(result));
        }

        void yaml::swap(yaml &other) noexcept
        {
            m_impl.swap(other.m_impl);
        }

        void swap(yaml &lhs, yaml &rhs) noexcept
        {
            lhs.swap(rhs);
        }

        node_t yaml::type() const noexcept
        {
            if (!m_impl)
            {
                return node_t::null;
            }
            switch (m_impl->value.index())
            {
            case 0:
                return node_t::null;
            case 1:
                return node_t::boolean;
            case 2:
                return node_t::integer;
            case 3:
                return node_t::floating;
            case 4:
                return node_t::string;
            case 5:
                return node_t::sequence;
            case 6:
                return node_t::mapping;
            default:
                return node_t::null;
            }
        }

        const char *yaml::type_name() const noexcept
        {
            return string(type());
        }

        bool yaml::is_null() const noexcept { return type() == node_t::null; }
        bool yaml::is_boolean() const noexcept { return type() == node_t::boolean; }
        bool yaml::is_integer() const noexcept { return type() == node_t::integer; }
        bool yaml::is_floating() const noexcept { return type() == node_t::floating; }
        bool yaml::is_number() const noexcept { return is_integer() || is_floating(); }
        bool yaml::is_string() const noexcept { return type() == node_t::string; }
        bool yaml::is_sequence() const noexcept { return type() == node_t::sequence; }
        bool yaml::is_mapping() const noexcept { return type() == node_t::mapping; }
        bool yaml::is_scalar() const noexcept
        {
            return is_null() || is_boolean() || is_integer() || is_floating() || is_string();
        }

        bool yaml::as_boolean() const
        {
            if (!is_boolean())
            {
                throw_type_error(type(), "boolean");
            }
            return std::get<bool>(m_impl->value);
        }

        std::int64_t yaml::as_integer() const
        {
            if (!is_integer())
            {
                throw_type_error(type(), "integer");
            }
            return std::get<std::int64_t>(m_impl->value);
        }

        double yaml::as_floating() const
        {
            if (!is_floating())
            {
                throw_type_error(type(), "floating-point");
            }
            return std::get<double>(m_impl->value);
        }

        const std::string &yaml::as_string() const
        {
            if (!is_string())
            {
                throw_type_error(type(), "string");
            }
            return std::get<std::string>(m_impl->value);
        }

        std::string &yaml::as_string()
        {
            if (!is_string())
            {
                throw_type_error(type(), "string");
            }
            return std::get<std::string>(m_impl->value);
        }

        const char *yaml::as_c_str() const { return as_string().c_str(); }

        yaml::sequence &yaml::as_sequence()
        {
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            return std::get<sequence>(m_impl->value);
        }

        const yaml::sequence &yaml::as_sequence() const
        {
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            return std::get<sequence>(m_impl->value);
        }

        yaml::mapping &yaml::as_mapping()
        {
            if (!is_mapping())
            {
                throw_type_error(type(), "mapping");
            }
            return std::get<mapping>(m_impl->value);
        }

        const yaml::mapping &yaml::as_mapping() const
        {
            if (!is_mapping())
            {
                throw_type_error(type(), "mapping");
            }
            return std::get<mapping>(m_impl->value);
        }

        std::optional<bool> yaml::get_boolean() const noexcept
        {
            if (!is_boolean())
            {
                return std::nullopt;
            }
            return std::get<bool>(m_impl->value);
        }

        std::optional<std::int64_t> yaml::get_integer() const noexcept
        {
            if (!is_integer())
            {
                return std::nullopt;
            }
            return std::get<std::int64_t>(m_impl->value);
        }

        std::optional<double> yaml::get_floating() const noexcept
        {
            if (!is_floating())
            {
                return std::nullopt;
            }
            return std::get<double>(m_impl->value);
        }

        std::optional<std::string> yaml::get_string() const
        {
            if (!is_string())
            {
                return std::nullopt;
            }
            return std::get<std::string>(m_impl->value);
        }

        std::optional<std::string_view> yaml::get_string_view() const noexcept
        {
            if (!is_string())
            {
                return std::nullopt;
            }
            return std::string_view(std::get<std::string>(m_impl->value));
        }

        std::optional<double> yaml::get_number() const noexcept
        {
            if (is_integer())
            {
                return static_cast<double>(std::get<std::int64_t>(m_impl->value));
            }
            if (is_floating())
            {
                return std::get<double>(m_impl->value);
            }
            return std::nullopt;
        }

        std::string yaml::to_string() const
        {
            switch (type())
            {
            case node_t::null:
                return "null";
            case node_t::boolean:
                return std::get<bool>(m_impl->value) ? "true" : "false";
            case node_t::integer:
                return format_integer(std::get<std::int64_t>(m_impl->value));
            case node_t::floating:
                return format_double(std::get<double>(m_impl->value));
            case node_t::string:
                return std::get<std::string>(m_impl->value);
            case node_t::sequence:
            case node_t::mapping:
            {
                dump_options options;
                options.flow = true;
                options.trailing_newline = false;
                return dump(options);
            }
            }
            return {};
        }

        //---------------------------------------------------------------------
        // [SECTION] Mapping and sequence access
        //---------------------------------------------------------------------

        yaml &yaml::operator[](std::string_view key)
        {
            if (is_null())
            {
                *this = make_mapping();
            }
            if (!is_mapping())
            {
                throw_type_error(type(), "mapping");
            }
            mapping &values = std::get<mapping>(m_impl->value);
            for (auto &entry : values)
            {
                if (entry.first == key)
                {
                    return entry.second;
                }
            }
            values.emplace_back(std::string(key), yaml());
            return values.back().second;
        }

        const yaml &yaml::operator[](std::string_view key) const
        {
            return at(key);
        }

        yaml &yaml::operator[](std::size_t index)
        {
            if (is_null())
            {
                *this = make_sequence();
            }
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            sequence &values = std::get<sequence>(m_impl->value);
            if (index >= values.size())
            {
                throw lookup_error("sequence index " + std::to_string(index) +
                                   " is out of range (size " + std::to_string(values.size()) +
                                   ")");
            }
            return values[index];
        }

        const yaml &yaml::operator[](std::size_t index) const
        {
            return at(index);
        }

        yaml &yaml::at(std::string_view key)
        {
            if (!is_mapping())
            {
                throw_type_error(type(), "mapping");
            }
            mapping &values = std::get<mapping>(m_impl->value);
            for (auto &entry : values)
            {
                if (entry.first == key)
                {
                    return entry.second;
                }
            }
            throw lookup_error("mapping key '" + std::string(key) + "' was not found");
        }

        const yaml &yaml::at(std::string_view key) const
        {
            if (!is_mapping())
            {
                throw_type_error(type(), "mapping");
            }
            const mapping &values = std::get<mapping>(m_impl->value);
            for (const auto &entry : values)
            {
                if (entry.first == key)
                {
                    return entry.second;
                }
            }
            throw lookup_error("mapping key '" + std::string(key) + "' was not found");
        }

        yaml &yaml::at(std::size_t index)
        {
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            sequence &values = std::get<sequence>(m_impl->value);
            if (index >= values.size())
            {
                throw lookup_error("sequence index " + std::to_string(index) +
                                   " is out of range (size " + std::to_string(values.size()) +
                                   ")");
            }
            return values[index];
        }

        const yaml &yaml::at(std::size_t index) const
        {
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            const sequence &values = std::get<sequence>(m_impl->value);
            if (index >= values.size())
            {
                throw lookup_error("sequence index " + std::to_string(index) +
                                   " is out of range (size " + std::to_string(values.size()) +
                                   ")");
            }
            return values[index];
        }

        yaml *yaml::find(std::string_view key) noexcept
        {
            if (!is_mapping())
            {
                return nullptr;
            }
            for (auto &entry : std::get<mapping>(m_impl->value))
            {
                if (entry.first == key)
                {
                    return &entry.second;
                }
            }
            return nullptr;
        }

        const yaml *yaml::find(std::string_view key) const noexcept
        {
            if (!is_mapping())
            {
                return nullptr;
            }
            for (const auto &entry : std::get<mapping>(m_impl->value))
            {
                if (entry.first == key)
                {
                    return &entry.second;
                }
            }
            return nullptr;
        }

        bool yaml::contains(std::string_view key) const noexcept
        {
            return find(key) != nullptr;
        }

        yaml &yaml::push_back(yaml value)
        {
            if (is_null())
            {
                *this = make_sequence();
            }
            if (!is_sequence())
            {
                throw_type_error(type(), "sequence");
            }
            std::get<sequence>(m_impl->value).push_back(std::move(value));
            return std::get<sequence>(m_impl->value).back();
        }

        bool yaml::erase(std::string_view key)
        {
            if (!is_mapping())
            {
                return false;
            }
            auto &values = std::get<mapping>(m_impl->value);
            for (auto it = values.begin(); it != values.end(); ++it)
            {
                if (it->first == key)
                {
                    values.erase(it);
                    return true;
                }
            }
            return false;
        }

        bool yaml::erase(std::size_t index)
        {
            if (!is_sequence())
            {
                return false;
            }
            auto &values = std::get<sequence>(m_impl->value);
            if (index >= values.size())
            {
                return false;
            }
            values.erase(values.begin() + static_cast<std::ptrdiff_t>(index));
            return true;
        }

        void yaml::clear()
        {
            switch (type())
            {
            case node_t::sequence:
                std::get<sequence>(m_impl->value).clear();
                break;
            case node_t::mapping:
                std::get<mapping>(m_impl->value).clear();
                break;
            case node_t::string:
                std::get<std::string>(m_impl->value).clear();
                break;
            default:
                *this = yaml();
                break;
            }
        }

        std::size_t yaml::size() const noexcept
        {
            switch (type())
            {
            case node_t::null:
                return 0;
            case node_t::string:
                return std::get<std::string>(m_impl->value).size();
            case node_t::sequence:
                return std::get<sequence>(m_impl->value).size();
            case node_t::mapping:
                return std::get<mapping>(m_impl->value).size();
            default:
                return 1;
            }
        }

        bool yaml::empty() const noexcept { return size() == 0; }

        std::vector<std::string> yaml::keys() const
        {
            std::vector<std::string> result;
            if (!is_mapping())
            {
                return result;
            }
            result.reserve(std::get<mapping>(m_impl->value).size());
            for (const auto &entry : std::get<mapping>(m_impl->value))
            {
                result.push_back(entry.first);
            }
            return result;
        }

        yaml &yaml::set(std::string_view key, yaml value)
        {
            yaml &slot = (*this)[key];
            slot = std::move(value);
            return slot;
        }

        bool operator==(const yaml &lhs, const yaml &rhs) noexcept
        {
            if (lhs.type() != rhs.type())
            {
                return false;
            }
            try
            {
                switch (lhs.type())
                {
                case node_t::null:
                    return true;
                case node_t::boolean:
                    return lhs.as_boolean() == rhs.as_boolean();
                case node_t::integer:
                    return lhs.as_integer() == rhs.as_integer();
                case node_t::floating:
                {
                    const double a = lhs.as_floating();
                    const double b = rhs.as_floating();
                    if (std::isnan(a) && std::isnan(b))
                    {
                        return true;
                    }
                    return a == b;
                }
                case node_t::string:
                    return lhs.as_string() == rhs.as_string();
                case node_t::sequence:
                {
                    const auto &a = lhs.as_sequence();
                    const auto &b = rhs.as_sequence();
                    if (a.size() != b.size())
                    {
                        return false;
                    }
                    for (std::size_t i = 0; i < a.size(); ++i)
                    {
                        if (!(a[i] == b[i]))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                case node_t::mapping:
                {
                    const auto &a = lhs.as_mapping();
                    const auto &b = rhs.as_mapping();
                    if (a.size() != b.size())
                    {
                        return false;
                    }
                    for (const auto &entry : a)
                    {
                        const yaml *other = rhs.find(entry.first);
                        if (other == nullptr || !(entry.second == *other))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                }
            }
            catch (...)
            {
                return false;
            }
            return false;
        }

        bool operator!=(const yaml &lhs, const yaml &rhs) noexcept { return !(lhs == rhs); }

        //---------------------------------------------------------------------
        // [SECTION] Encoding helpers
        //---------------------------------------------------------------------

        namespace detail
        {
            namespace
            {
                constexpr unsigned char bom_utf8[] = {0xEF, 0xBB, 0xBF};
                constexpr unsigned char bom_utf16le[] = {0xFF, 0xFE};
                constexpr unsigned char bom_utf16be[] = {0xFE, 0xFF};
                constexpr unsigned char bom_utf32le[] = {0xFF, 0xFE, 0x00, 0x00};
                constexpr unsigned char bom_utf32be[] = {0x00, 0x00, 0xFE, 0xFF};

                bool has_prefix(const std::string &bytes, const unsigned char *prefix,
                                std::size_t size) noexcept
                {
                    return bytes.size() >= size &&
                           std::memcmp(bytes.data(), prefix, size) == 0;
                }

                std::string utf16_to_utf8(const std::string &bytes, bool big_endian)
                {
                    std::vector<unsigned char> data(bytes.begin(), bytes.end());
                    return utf16::to_utf8(data, big_endian);
                }

                std::string utf32_to_utf8(const std::string &bytes, bool big_endian)
                {
                    std::vector<unsigned char> data(bytes.begin(), bytes.end());
                    return utf32::to_utf8(data, big_endian);
                }
            } // namespace

            encoding determine_encoding(const void *data, std::size_t size) noexcept
            {
                if (data == nullptr || size == 0)
                {
                    return encoding::unspecified;
                }
                const unsigned char *bytes = static_cast<const unsigned char *>(data);
                auto matches = [&](const unsigned char *prefix, std::size_t length)
                {
                    return size >= length && std::memcmp(bytes, prefix, length) == 0;
                };
                if (matches(bom_utf32be, 4))
                {
                    return encoding::utf32be;
                }
                if (matches(bom_utf32le, 4))
                {
                    return encoding::utf32le;
                }
                if (matches(bom_utf8, 3))
                {
                    return encoding::utf8;
                }
                if (matches(bom_utf16be, 2))
                {
                    return encoding::utf16be;
                }
                if (matches(bom_utf16le, 2))
                {
                    return encoding::utf16le;
                }
                return encoding::unspecified;
            }

            int utf8::decode(const char *data, std::size_t size, unsigned int &value) noexcept
            {
                value = 0;
                if (data == nullptr || size == 0)
                {
                    return -1;
                }
                const unsigned char b0 = static_cast<unsigned char>(data[0]);
                if (b0 < 0x80)
                {
                    value = b0;
                    return 1;
                }
                if (b0 < 0xC2 || b0 > 0xF4)
                {
                    return -1;
                }

                std::size_t length = 0;
                unsigned int result = 0;
                if (b0 <= 0xDF)
                {
                    length = 2;
                    result = b0 & 0x1Fu;
                }
                else if (b0 <= 0xEF)
                {
                    length = 3;
                    result = b0 & 0x0Fu;
                }
                else
                {
                    length = 4;
                    result = b0 & 0x07u;
                }
                if (size < length)
                {
                    return -1;
                }
                for (std::size_t i = 1; i < length; ++i)
                {
                    const unsigned char b = static_cast<unsigned char>(data[i]);
                    if ((b & 0xC0u) != 0x80u)
                    {
                        return -1;
                    }
                    result = (result << 6) | (b & 0x3Fu);
                }

                if ((length == 2 && result < 0x80) ||
                    (length == 3 && result < 0x800) ||
                    (length == 4 && result < 0x10000) || result > 0x10FFFF ||
                    (result >= 0xD800 && result <= 0xDFFF))
                {
                    return -1;
                }
                value = result;
                return static_cast<int>(length);
            }

            int utf8::encode(unsigned int codepoint, char *output,
                             std::size_t size) noexcept
            {
                if (output == nullptr || codepoint > 0x10FFFF ||
                    (codepoint >= 0xD800 && codepoint <= 0xDFFF))
                {
                    return -1;
                }
                if (codepoint < 0x80)
                {
                    if (size < 1)
                    {
                        return -1;
                    }
                    output[0] = static_cast<char>(codepoint);
                    return 1;
                }
                if (codepoint < 0x800)
                {
                    if (size < 2)
                    {
                        return -1;
                    }
                    output[0] = static_cast<char>(0xC0u | (codepoint >> 6));
                    output[1] = static_cast<char>(0x80u | (codepoint & 0x3Fu));
                    return 2;
                }
                if (codepoint < 0x10000)
                {
                    if (size < 3)
                    {
                        return -1;
                    }
                    output[0] = static_cast<char>(0xE0u | (codepoint >> 12));
                    output[1] = static_cast<char>(0x80u | ((codepoint >> 6) & 0x3Fu));
                    output[2] = static_cast<char>(0x80u | (codepoint & 0x3Fu));
                    return 3;
                }
                if (size < 4)
                {
                    return -1;
                }
                output[0] = static_cast<char>(0xF0u | (codepoint >> 18));
                output[1] = static_cast<char>(0x80u | ((codepoint >> 12) & 0x3Fu));
                output[2] = static_cast<char>(0x80u | ((codepoint >> 6) & 0x3Fu));
                output[3] = static_cast<char>(0x80u | (codepoint & 0x3Fu));
                return 4;
            }

            std::string utf8::from_codepoint(unsigned int codepoint)
            {
                char buffer[4];
                const int length = encode(codepoint, buffer, sizeof(buffer));
                if (length < 0)
                {
                    throw encoding_error("invalid Unicode code point");
                }
                return std::string(buffer, buffer + length);
            }

            bool utf8::validate(const std::string &text) noexcept
            {
                std::size_t pos = 0;
                while (pos < text.size())
                {
                    unsigned int value = 0;
                    const int length = decode(text.data() + pos, text.size() - pos, value);
                    if (length < 0)
                    {
                        return false;
                    }
                    pos += static_cast<std::size_t>(length);
                }
                return true;
            }

            int utf16::decode(const char *data, std::size_t size, unsigned int &value,
                              bool big_endian) noexcept
            {
                value = 0;
                if (data == nullptr || size < 2)
                {
                    return -1;
                }
                auto read_unit = [&](std::size_t offset) -> unsigned int
                {
                    const unsigned char *b = reinterpret_cast<const unsigned char *>(data + offset);
                    if (big_endian)
                    {
                        return (static_cast<unsigned int>(b[0]) << 8) | b[1];
                    }
                    return static_cast<unsigned int>(b[0]) |
                           (static_cast<unsigned int>(b[1]) << 8);
                };
                const unsigned int first = read_unit(0);
                if (first >= 0xD800 && first <= 0xDBFF)
                {
                    if (size < 4)
                    {
                        return -1;
                    }
                    const unsigned int second = read_unit(2);
                    if (second < 0xDC00 || second > 0xDFFF)
                    {
                        return -1;
                    }
                    value = 0x10000 + ((first - 0xD800) << 10) + (second - 0xDC00);
                    return 4;
                }
                if (first >= 0xDC00 && first <= 0xDFFF)
                {
                    return -1;
                }
                value = first;
                return 2;
            }

            int utf16::encode(unsigned int codepoint, char *output, std::size_t size,
                              bool big_endian) noexcept
            {
                if (output == nullptr || codepoint > 0x10FFFF ||
                    (codepoint >= 0xD800 && codepoint <= 0xDFFF))
                {
                    return -1;
                }
                auto write_unit = [&](std::size_t offset, unsigned int unit)
                {
                    unsigned char *out = reinterpret_cast<unsigned char *>(output);
                    if (big_endian)
                    {
                        out[offset] = static_cast<unsigned char>((unit >> 8) & 0xFFu);
                        out[offset + 1] = static_cast<unsigned char>(unit & 0xFFu);
                    }
                    else
                    {
                        out[offset] = static_cast<unsigned char>(unit & 0xFFu);
                        out[offset + 1] = static_cast<unsigned char>((unit >> 8) & 0xFFu);
                    }
                };
                if (codepoint < 0x10000)
                {
                    if (size < 2)
                    {
                        return -1;
                    }
                    write_unit(0, codepoint);
                    return 2;
                }
                if (size < 4)
                {
                    return -1;
                }
                const unsigned int adjusted = codepoint - 0x10000;
                write_unit(0, 0xD800 + (adjusted >> 10));
                write_unit(2, 0xDC00 + (adjusted & 0x3FF));
                return 4;
            }

            std::string utf16::to_utf8(const std::vector<unsigned char> &bytes,
                                       bool big_endian)
            {
                std::string out;
                out.reserve(bytes.size());
                std::size_t pos = 0;
                while (pos < bytes.size())
                {
                    unsigned int codepoint = 0;
                    const int consumed = utf16::decode(
                        reinterpret_cast<const char *>(bytes.data() + pos), bytes.size() - pos,
                        codepoint, big_endian);
                    if (consumed < 0)
                    {
                        throw encoding_error("invalid UTF-16 byte sequence");
                    }
                    char buffer[4];
                    const int written = utf8::encode(codepoint, buffer, sizeof(buffer));
                    if (written < 0)
                    {
                        throw encoding_error("invalid Unicode code point in UTF-16 input");
                    }
                    out.append(buffer, buffer + written);
                    pos += static_cast<std::size_t>(consumed);
                }
                return out;
            }

            int utf32::decode(const char *data, std::size_t size, unsigned int &value,
                              bool big_endian) noexcept
            {
                value = 0;
                if (data == nullptr || size < 4)
                {
                    return -1;
                }
                const unsigned char *b = reinterpret_cast<const unsigned char *>(data);
                unsigned int result = 0;
                if (big_endian)
                {
                    result = (static_cast<unsigned int>(b[0]) << 24) |
                             (static_cast<unsigned int>(b[1]) << 16) |
                             (static_cast<unsigned int>(b[2]) << 8) |
                             static_cast<unsigned int>(b[3]);
                }
                else
                {
                    result = static_cast<unsigned int>(b[0]) |
                             (static_cast<unsigned int>(b[1]) << 8) |
                             (static_cast<unsigned int>(b[2]) << 16) |
                             (static_cast<unsigned int>(b[3]) << 24);
                }
                if (result > 0x10FFFF || (result >= 0xD800 && result <= 0xDFFF))
                {
                    return -1;
                }
                value = result;
                return 4;
            }

            int utf32::encode(unsigned int codepoint, char *output, std::size_t size,
                              bool big_endian) noexcept
            {
                if (output == nullptr || size < 4 || codepoint > 0x10FFFF ||
                    (codepoint >= 0xD800 && codepoint <= 0xDFFF))
                {
                    return -1;
                }
                unsigned char *out = reinterpret_cast<unsigned char *>(output);
                if (big_endian)
                {
                    out[0] = static_cast<unsigned char>((codepoint >> 24) & 0xFFu);
                    out[1] = static_cast<unsigned char>((codepoint >> 16) & 0xFFu);
                    out[2] = static_cast<unsigned char>((codepoint >> 8) & 0xFFu);
                    out[3] = static_cast<unsigned char>(codepoint & 0xFFu);
                }
                else
                {
                    out[0] = static_cast<unsigned char>(codepoint & 0xFFu);
                    out[1] = static_cast<unsigned char>((codepoint >> 8) & 0xFFu);
                    out[2] = static_cast<unsigned char>((codepoint >> 16) & 0xFFu);
                    out[3] = static_cast<unsigned char>((codepoint >> 24) & 0xFFu);
                }
                return 4;
            }

            std::string utf32::to_utf8(const std::vector<unsigned char> &bytes,
                                       bool big_endian)
            {
                std::string out;
                out.reserve(bytes.size());
                std::size_t pos = 0;
                while (pos < bytes.size())
                {
                    if (bytes.size() - pos < 4)
                    {
                        throw encoding_error("truncated UTF-32 byte sequence");
                    }
                    unsigned int codepoint = 0;
                    const int consumed = utf32::decode(
                        reinterpret_cast<const char *>(bytes.data() + pos), bytes.size() - pos,
                        codepoint, big_endian);
                    if (consumed < 0)
                    {
                        throw encoding_error("invalid UTF-32 byte sequence");
                    }
                    char buffer[4];
                    const int written = utf8::encode(codepoint, buffer, sizeof(buffer));
                    if (written < 0)
                    {
                        throw encoding_error("invalid Unicode code point in UTF-32 input");
                    }
                    out.append(buffer, buffer + written);
                    pos += 4;
                }
                return out;
            }

            std::string decode_to_utf8(const std::string &bytes, encoding detected)
            {
                if (bytes.empty())
                {
                    return {};
                }

                encoding actual = detected;
                if (actual == encoding::unspecified)
                {
                    actual = determine_encoding(bytes.data(), bytes.size());
                    if (actual == encoding::unspecified)
                    {
                        actual = encoding::utf8;
                    }
                }

                std::string payload = bytes;
                switch (actual)
                {
                case encoding::utf8:
                    if (has_prefix(payload, bom_utf8, 3))
                    {
                        payload.erase(0, 3);
                    }
                    if (!utf8::validate(payload))
                    {
                        throw encoding_error("invalid UTF-8 input");
                    }
                    return payload;

                case encoding::utf16le:
                case encoding::utf16be:
                case encoding::utf16:
                {
                    const bool big = actual == encoding::utf16be;
                    if (has_prefix(payload, big ? bom_utf16be : bom_utf16le, 2))
                    {
                        payload.erase(0, 2);
                    }
                    else if (has_prefix(payload, bom_utf16le, 2) ||
                             has_prefix(payload, bom_utf16be, 2))
                    {
                        // BOM disagrees with the requested endianness; trust the BOM.
                        const bool bom_big = has_prefix(payload, bom_utf16be, 2);
                        payload.erase(0, 2);
                        return utf16_to_utf8(payload, bom_big);
                    }
                    return utf16_to_utf8(payload, big);
                }

                case encoding::utf32le:
                case encoding::utf32be:
                case encoding::utf32:
                {
                    const bool big = actual == encoding::utf32be;
                    if (has_prefix(payload, big ? bom_utf32be : bom_utf32le, 4))
                    {
                        payload.erase(0, 4);
                    }
                    else if (has_prefix(payload, bom_utf32le, 4) ||
                             has_prefix(payload, bom_utf32be, 4))
                    {
                        const bool bom_big = has_prefix(payload, bom_utf32be, 4);
                        payload.erase(0, 4);
                        return utf32_to_utf8(payload, bom_big);
                    }
                    return utf32_to_utf8(payload, big);
                }

                case encoding::unspecified:
                    break;
                }
                return payload;
            }

            memory_iadapter::memory_iadapter(const void *data, std::size_t size)
                : m_data(static_cast<const unsigned char *>(data)), m_size(size)
            {
            }

            std::size_t memory_iadapter::read(void *data, std::size_t size)
            {
                if (data == nullptr || size == 0 || m_data == nullptr || m_pos >= m_size)
                {
                    return 0;
                }
                const std::size_t remaining = m_size - m_pos;
                const std::size_t count = std::min(size, remaining);
                std::memcpy(data, m_data + m_pos, count);
                m_pos += count;
                return count;
            }

            file_iadapter::file_iadapter(std::FILE *file) : m_file(file) {}

            std::size_t file_iadapter::read(void *data, std::size_t size)
            {
                if (m_file == nullptr || data == nullptr || size == 0)
                {
                    return 0;
                }
                return std::fread(data, 1, size, m_file);
            }

            stream_iadapter::stream_iadapter(std::istream &stream) : m_stream(&stream) {}

            std::size_t stream_iadapter::read(void *data, std::size_t size)
            {
                if (m_stream == nullptr || data == nullptr || size == 0)
                {
                    return 0;
                }
                m_stream->read(static_cast<char *>(data),
                               static_cast<std::streamsize>(size));
                const std::streamsize read = m_stream->gcount();
                return read < 0 ? 0 : static_cast<std::size_t>(read);
            }

            std::string read_all(iadapter &adapter)
            {
                std::string result;
                char buffer[4096];
                while (true)
                {
                    const std::size_t count = adapter.read(buffer, sizeof(buffer));
                    if (count == 0)
                    {
                        break;
                    }
                    result.append(buffer, buffer + count);
                }
                return result;
            }

            memory_oadapter::memory_oadapter(void *data, std::size_t size)
                : m_data(static_cast<unsigned char *>(data)), m_size(size)
            {
            }

            std::size_t memory_oadapter::write(const void *data, std::size_t size)
            {
                if (data == nullptr || size == 0 || m_data == nullptr || m_pos >= m_size)
                {
                    return 0;
                }
                const std::size_t remaining = m_size - m_pos;
                const std::size_t count = std::min(size, remaining);
                std::memcpy(m_data + m_pos, data, count);
                m_pos += count;
                return count;
            }

            file_oadapter::file_oadapter(std::FILE *file) : m_file(file) {}

            std::size_t file_oadapter::write(const void *data, std::size_t size)
            {
                if (m_file == nullptr || data == nullptr || size == 0)
                {
                    return 0;
                }
                return std::fwrite(data, 1, size, m_file);
            }

            stream_oadapter::stream_oadapter(std::ostream &stream) : m_stream(&stream) {}

            std::size_t stream_oadapter::write(const void *data, std::size_t size)
            {
                if (m_stream == nullptr || data == nullptr || size == 0)
                {
                    return 0;
                }
                m_stream->write(static_cast<const char *>(data),
                                static_cast<std::streamsize>(size));
                return m_stream->good() ? size : 0;
            }
        } // namespace detail

        //---------------------------------------------------------------------
        // [SECTION] Parser
        //---------------------------------------------------------------------

        namespace
        {
            std::string decode_quoted(std::string_view raw, bool single_quoted,
                                      const detail::mark &position)
            {
                if (raw.size() < 2)
                {
                    throw parse_error("unterminated quoted scalar", position.line,
                                      position.column, position.index);
                }
                const char quote = single_quoted ? '\'' : '"';
                if (raw.front() != quote || raw.back() != quote)
                {
                    throw parse_error("unterminated quoted scalar", position.line,
                                      position.column, position.index);
                }
                const std::string_view content = raw.substr(1, raw.size() - 2);

                auto code_point = [&](std::string_view digits, unsigned int &value) -> bool
                {
                    if (digits.empty())
                    {
                        return false;
                    }
                    value = 0;
                    for (char c : digits)
                    {
                        const int digit = hex_value(c);
                        if (digit < 0)
                        {
                            return false;
                        }
                        value = (value << 4) | static_cast<unsigned int>(digit);
                    }
                    return true;
                };

                std::string out;
                out.reserve(content.size());
                std::size_t i = 0;
                while (i < content.size())
                {
                    const char c = content[i];
                    if (c == '\n')
                    {
                        while (!out.empty() && (out.back() == ' ' || out.back() == '\t'))
                        {
                            out.pop_back();
                        }
                        std::size_t breaks = 0;
                        while (i < content.size() &&
                               (content[i] == '\n' || content[i] == ' ' || content[i] == '\t'))
                        {
                            if (content[i] == '\n')
                            {
                                ++breaks;
                            }
                            ++i;
                        }
                        if (i >= content.size())
                        {
                            break;
                        }
                        if (breaks > 1)
                        {
                            out.append(breaks - 1, '\n');
                        }
                        else
                        {
                            out.push_back(' ');
                        }
                        continue;
                    }

                    if (single_quoted)
                    {
                        if (c == '\'')
                        {
                            if (i + 1 < content.size() && content[i + 1] == '\'')
                            {
                                out.push_back('\'');
                                i += 2;
                                continue;
                            }
                            throw parse_error("unexpected quote in single-quoted scalar",
                                              position.line, position.column, position.index);
                        }
                        out.push_back(c);
                        ++i;
                        continue;
                    }

                    if (c != '\\')
                    {
                        out.push_back(c);
                        ++i;
                        continue;
                    }

                    ++i;
                    if (i >= content.size())
                    {
                        throw parse_error("unterminated escape sequence", position.line,
                                          position.column, position.index);
                    }
                    const char escape = content[i++];
                    switch (escape)
                    {
                    case '\n':
                        while (i < content.size() &&
                               (content[i] == ' ' || content[i] == '\t'))
                        {
                            ++i;
                        }
                        break;
                    case '0':
                        out.push_back('\0');
                        break;
                    case 'a':
                        out.push_back('\a');
                        break;
                    case 'b':
                        out.push_back('\b');
                        break;
                    case 't':
                    case '\t':
                        out.push_back('\t');
                        break;
                    case 'n':
                        out.push_back('\n');
                        break;
                    case 'v':
                        out.push_back('\v');
                        break;
                    case 'f':
                        out.push_back('\f');
                        break;
                    case 'r':
                        out.push_back('\r');
                        break;
                    case 'e':
                        out.push_back('\x1B');
                        break;
                    case '"':
                        out.push_back('"');
                        break;
                    case '\\':
                        out.push_back('\\');
                        break;
                    case '/':
                        out.push_back('/');
                        break;
                    case ' ':
                        out.push_back(' ');
                        break;
                    case 'N':
                        out += detail::utf8::from_codepoint(0x85);
                        break;
                    case '_':
                        out += detail::utf8::from_codepoint(0xA0);
                        break;
                    case 'L':
                        out += detail::utf8::from_codepoint(0x2028);
                        break;
                    case 'P':
                        out += detail::utf8::from_codepoint(0x2029);
                        break;
                    case 'x':
                    case 'u':
                    case 'U':
                    {
                        const std::size_t digits = escape == 'x' ? 2 : (escape == 'u' ? 4 : 8);
                        if (i + digits > content.size())
                        {
                            throw parse_error("truncated Unicode escape sequence", position.line,
                                              position.column, position.index);
                        }
                        unsigned int value = 0;
                        if (!code_point(content.substr(i, digits), value) ||
                            value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF))
                        {
                            throw parse_error("invalid Unicode escape sequence", position.line,
                                              position.column, position.index);
                        }
                        out += detail::utf8::from_codepoint(value);
                        i += digits;
                        break;
                    }
                    default:
                        throw parse_error(std::string("unknown escape sequence '\\") + escape + "'",
                                          position.line, position.column, position.index);
                    }
                }
                return out;
            }

            bool is_sequence_entry(std::string_view text) noexcept
            {
                return !text.empty() && text.front() == '-' &&
                       (text.size() == 1 || text[1] == ' ' || text[1] == '\t');
            }

            std::string normalize_newlines(std::string text)
            {
                std::string out;
                out.reserve(text.size());
                for (std::size_t i = 0; i < text.size(); ++i)
                {
                    const char c = text[i];
                    if (c == '\r')
                    {
                        if (i + 1 < text.size() && text[i + 1] == '\n')
                        {
                            ++i;
                        }
                        out.push_back('\n');
                    }
                    else
                    {
                        out.push_back(c);
                    }
                }
                return out;
            }

            std::string normalize_tag(std::string tag)
            {
                if (tag.size() >= 2 && tag[0] == '!' && tag[1] == '<' && tag.back() == '>')
                {
                    tag = tag.substr(2, tag.size() - 3);
                }
                else if (tag.size() >= 2 && tag[0] == '!' && tag[1] == '!')
                {
                    // Secondary handle: !!str, !!int, ...
                    tag.erase(0, 2);
                }
                else if (!tag.empty() && tag[0] == '!')
                {
                    tag.erase(tag.begin());
                }
                const std::string prefix = "tag:yaml.org,2002:";
                if (tag.compare(0, prefix.size(), prefix) == 0)
                {
                    tag.erase(0, prefix.size());
                }
                return tag;
            }

            yaml apply_tag(yaml value, const std::string &tag, const detail::mark &position)
            {
                if (tag.empty() || tag == "!")
                {
                    return value;
                }
                const std::string name = normalize_tag(tag);
                if (name.empty() || name == "!")
                {
                    return value;
                }
                if (name == "str")
                {
                    if (!value.is_string())
                    {
                        value = yaml(value.to_string());
                    }
                    return value;
                }
                if (name == "null")
                {
                    return yaml();
                }
                if (name == "bool")
                {
                    if (value.is_boolean())
                    {
                        return value;
                    }
                    bool boolean = false;
                    if (value.is_string() && parse_boolean(value.as_string(), boolean))
                    {
                        return yaml(boolean);
                    }
                    throw parse_error("value cannot be interpreted as !!bool", position.line,
                                      position.column, position.index);
                }
                if (name == "int")
                {
                    if (value.is_integer())
                    {
                        return value;
                    }
                    std::int64_t integer = 0;
                    if (value.is_string() && parse_integer(value.as_string(), integer))
                    {
                        return yaml(integer);
                    }
                    throw parse_error("value cannot be interpreted as !!int", position.line,
                                      position.column, position.index);
                }
                if (name == "float")
                {
                    if (value.is_floating())
                    {
                        return value;
                    }
                    if (value.is_integer())
                    {
                        return yaml(static_cast<double>(value.as_integer()));
                    }
                    double floating = 0.0;
                    std::int64_t integer = 0;
                    if (value.is_string() &&
                        (parse_floating(value.as_string(), floating) ||
                         parse_integer(value.as_string(), integer)))
                    {
                        if (parse_integer(value.as_string(), integer))
                        {
                            floating = static_cast<double>(integer);
                        }
                        return yaml(floating);
                    }
                    throw parse_error("value cannot be interpreted as !!float", position.line,
                                      position.column, position.index);
                }
                throw parse_error("unsupported tag '!" + name + "'", position.line,
                                  position.column, position.index);
            }

            std::size_t node_weight(const yaml &value)
            {
                switch (value.type())
                {
                case node_t::sequence:
                {
                    std::size_t total = 1;
                    for (const yaml &item : value.as_sequence())
                    {
                        total += node_weight(item);
                    }
                    return total;
                }
                case node_t::mapping:
                {
                    std::size_t total = 1;
                    for (const auto &entry : value.as_mapping())
                    {
                        total += node_weight(entry.second);
                    }
                    return total;
                }
                default:
                    return 1;
                }
            }

            class flow_parser
            {
            public:
                flow_parser(std::string text, const parse_options &options,
                            std::unordered_map<std::string, yaml> &anchors,
                            std::size_t &nodes, detail::mark base)
                    : text_(std::move(text)), options_(options), anchors_(anchors), nodes_(nodes),
                      base_(base)
                {
                }

                yaml parse()
                {
                    skip();
                    yaml value = parse_node(0);
                    skip();
                    if (position_ != text_.size())
                    {
                        fail("unexpected trailing characters in flow value");
                    }
                    return value;
                }

            private:
                void skip()
                {
                    while (position_ < text_.size())
                    {
                        const char c = text_[position_];
                        if (c == ' ' || c == '\t' || c == '\n')
                        {
                            ++position_;
                            continue;
                        }
                        if (c == '#')
                        {
                            while (position_ < text_.size() && text_[position_] != '\n')
                            {
                                ++position_;
                            }
                            continue;
                        }
                        break;
                    }
                }

                detail::mark mark_at(std::size_t position) const
                {
                    detail::mark result = base_;
                    std::size_t line = base_.line;
                    std::size_t column = base_.column;
                    for (std::size_t i = 0; i < position && i < text_.size(); ++i)
                    {
                        if (text_[i] == '\n')
                        {
                            ++line;
                            column = 1;
                        }
                        else
                        {
                            ++column;
                        }
                    }
                    result.line = line;
                    result.column = column;
                    result.index = base_.index + position;
                    return result;
                }

                [[noreturn]] void fail(const std::string &message) const
                {
                    const detail::mark position = mark_at(position_);
                    throw parse_error(message, position.line, position.column, position.index);
                }

                void count_node(std::size_t depth)
                {
                    if (depth > options_.max_depth)
                    {
                        fail("maximum nesting depth exceeded");
                    }
                    ++nodes_;
                    if (nodes_ > options_.max_nodes)
                    {
                        fail("maximum node count exceeded");
                    }
                }

                void add_alias_weight(const yaml &value)
                {
                    const std::size_t weight = node_weight(value);
                    if (nodes_ > options_.max_nodes - std::min(weight, options_.max_nodes))
                    {
                        fail("maximum node count exceeded while expanding an alias");
                    }
                    nodes_ += weight;
                }

                std::string read_property()
                {
                    const std::size_t start = position_;
                    while (position_ < text_.size())
                    {
                        const char c = text_[position_];
                        if (c == ' ' || c == '\t' || c == '\n' || c == ',' || c == ']' ||
                            c == '}' || c == ':' || c == '[' || c == '{')
                        {
                            break;
                        }
                        ++position_;
                    }
                    return std::string(text_.substr(start, position_ - start));
                }

                std::string parse_quoted()
                {
                    const char quote = text_[position_];
                    const bool single = quote == '\'';
                    const std::size_t start = position_;
                    std::string raw;
                    raw.push_back(quote);
                    ++position_;
                    bool closed = false;
                    while (position_ < text_.size())
                    {
                        const char c = text_[position_];
                        if (single)
                        {
                            raw.push_back(c);
                            ++position_;
                            if (c == '\'')
                            {
                                if (position_ < text_.size() && text_[position_] == '\'')
                                {
                                    raw.push_back('\'');
                                    ++position_;
                                }
                                else
                                {
                                    closed = true;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            raw.push_back(c);
                            ++position_;
                            if (c == '\\')
                            {
                                if (position_ >= text_.size())
                                {
                                    break;
                                }
                                raw.push_back(text_[position_++]);
                            }
                            else if (c == '"')
                            {
                                closed = true;
                                break;
                            }
                        }
                    }
                    if (!closed)
                    {
                        fail("unterminated quoted scalar");
                    }
                    return decode_quoted(raw, single, mark_at(start));
                }

                std::string parse_plain(bool key)
                {
                    const std::size_t start = position_;
                    while (position_ < text_.size())
                    {
                        const char c = text_[position_];
                        if (key)
                        {
                            if (c == ':')
                            {
                                break;
                            }
                        }
                        else if (c == ',' || c == ']' || c == '}')
                        {
                            break;
                        }
                        if (c == '#' &&
                            (position_ == start || text_[position_ - 1] == ' ' ||
                             text_[position_ - 1] == '\t' || text_[position_ - 1] == '\n'))
                        {
                            break;
                        }
                        ++position_;
                    }
                    std::string raw(text_.substr(start, position_ - start));
                    // Flow plain scalars fold line breaks and surrounding spaces.
                    std::string folded;
                    folded.reserve(raw.size());
                    bool previous_space = false;
                    for (char c : raw)
                    {
                        if (c == ' ' || c == '\t' || c == '\n')
                        {
                            if (!previous_space)
                            {
                                folded.push_back(' ');
                                previous_space = true;
                            }
                        }
                        else
                        {
                            folded.push_back(c);
                            previous_space = false;
                        }
                    }
                    const std::string result = trim_copy(folded);
                    if (key && result.empty())
                    {
                        fail("empty mapping key");
                    }
                    return result;
                }

                bool flow_pair_ahead() const
                {
                    bool single = false;
                    bool double_quoted = false;
                    bool escaped = false;
                    int depth = 0;
                    for (std::size_t i = position_; i < text_.size(); ++i)
                    {
                        const char c = text_[i];
                        if (double_quoted)
                        {
                            if (escaped)
                            {
                                escaped = false;
                            }
                            else if (c == '\\')
                            {
                                escaped = true;
                            }
                            else if (c == '"')
                            {
                                double_quoted = false;
                            }
                            continue;
                        }
                        if (single)
                        {
                            if (c == '\'')
                            {
                                if (i + 1 < text_.size() && text_[i + 1] == '\'')
                                {
                                    ++i;
                                }
                                else
                                {
                                    single = false;
                                }
                            }
                            continue;
                        }
                        if (c == '"')
                        {
                            double_quoted = true;
                        }
                        else if (c == '\'')
                        {
                            single = true;
                        }
                        else if (c == '[' || c == '{')
                        {
                            ++depth;
                        }
                        else if (c == ']' || c == '}')
                        {
                            if (depth == 0)
                            {
                                return false;
                            }
                            --depth;
                        }
                        else if (c == ',' && depth == 0)
                        {
                            return false;
                        }
                        else if (c == ':' && depth == 0)
                        {
                            const char next = i + 1 < text_.size() ? text_[i + 1] : '\0';
                            if (next == '\0' || next == ' ' || next == '\t' ||
                                next == '\n' || next == ',' || next == ']' || next == '}')
                            {
                                return true;
                            }
                        }
                        else if (c == '#' &&
                                 (i == position_ || text_[i - 1] == ' ' ||
                                  text_[i - 1] == '\t' || text_[i - 1] == '\n'))
                        {
                            while (i < text_.size() && text_[i] != '\n')
                            {
                                ++i;
                            }
                        }
                    }
                    return false;
                }

                yaml parse_mapping_item(std::size_t depth)
                {
                    yaml::mapping entries;
                    while (true)
                    {
                        skip();
                        if (position_ >= text_.size())
                        {
                            fail("unterminated flow mapping");
                        }
                        std::string key;
                        if (text_[position_] == '\'' || text_[position_] == '"')
                        {
                            key = parse_quoted();
                        }
                        else
                        {
                            key = parse_plain(true);
                        }
                        skip();
                        if (position_ >= text_.size() || text_[position_] != ':')
                        {
                            fail("expected ':' in flow mapping");
                        }
                        ++position_;
                        skip();
                        yaml value;
                        if (position_ < text_.size() && text_[position_] != ',' &&
                            text_[position_] != ']' && text_[position_] != '}')
                        {
                            value = parse_node(depth + 1);
                        }
                        if (!options_.allow_duplicate_keys)
                        {
                            for (const auto &entry : entries)
                            {
                                if (entry.first == key)
                                {
                                    fail("duplicate mapping key '" + key + "'");
                                }
                            }
                        }
                        entries.emplace_back(std::move(key), std::move(value));
                        skip();
                        if (position_ < text_.size() && text_[position_] == ',')
                        {
                            const std::size_t saved = position_;
                            ++position_;
                            skip();
                            if (flow_pair_ahead())
                            {
                                continue;
                            }
                            position_ = saved;
                        }
                        break;
                    }
                    return yaml(std::move(entries));
                }

                yaml parse_node(std::size_t depth)
                {
                    count_node(depth);
                    skip();
                    if (position_ >= text_.size())
                    {
                        fail("unexpected end of flow collection");
                    }

                    std::string anchor;
                    std::string tag;
                    while (position_ < text_.size() &&
                           (text_[position_] == '&' || text_[position_] == '!'))
                    {
                        if (text_[position_] == '&')
                        {
                            ++position_;
                            anchor = read_property();
                            if (anchor.empty())
                            {
                                fail("empty anchor name");
                            }
                        }
                        else
                        {
                            ++position_;
                            if (position_ < text_.size() && text_[position_] == '<')
                            {
                                const std::size_t start = position_ - 1;
                                while (position_ < text_.size() && text_[position_] != '>')
                                {
                                    ++position_;
                                }
                                if (position_ >= text_.size())
                                {
                                    fail("unterminated tag");
                                }
                                ++position_;
                                tag = std::string(text_.substr(start, position_ - start));
                            }
                            else
                            {
                                tag = "!" + read_property();
                            }
                        }
                        skip();
                    }

                    yaml value;
                    if (position_ < text_.size() && text_[position_] == '*')
                    {
                        ++position_;
                        const std::string name = read_property();
                        const auto found = anchors_.find(name);
                        if (found == anchors_.end())
                        {
                            fail("undefined alias '*" + name + "'");
                        }
                        value = found->second;
                        add_alias_weight(value);
                        skip();
                        return value;
                    }

                    if (position_ >= text_.size())
                    {
                        fail("expected a flow value");
                    }

                    const char c = text_[position_];
                    if (flow_pair_ahead())
                    {
                        value = parse_mapping_item(depth + 1);
                    }
                    else if (c == '[')
                    {
                        value = parse_sequence(depth + 1);
                    }
                    else if (c == '{')
                    {
                        value = parse_mapping(depth + 1);
                    }
                    else if (c == '\'' || c == '"')
                    {
                        value = yaml(parse_quoted());
                    }
                    else
                    {
                        value = infer_scalar(parse_plain(false));
                    }

                    value = apply_tag(std::move(value), tag, mark_at(position_));
                    if (!anchor.empty())
                    {
                        anchors_[anchor] = value;
                    }
                    skip();
                    return value;
                }

                yaml parse_sequence(std::size_t depth)
                {
                    count_node(depth);
                    ++position_; // '['
                    yaml::sequence items;
                    skip();
                    if (position_ < text_.size() && text_[position_] == ']')
                    {
                        ++position_;
                        return yaml(std::move(items));
                    }
                    while (true)
                    {
                        items.push_back(parse_node(depth + 1));
                        skip();
                        if (position_ >= text_.size())
                        {
                            fail("unterminated flow sequence");
                        }
                        if (text_[position_] == ']')
                        {
                            ++position_;
                            break;
                        }
                        if (text_[position_] != ',')
                        {
                            fail("expected ',' or ']' in flow sequence");
                        }
                        ++position_;
                        skip();
                        if (position_ < text_.size() && text_[position_] == ']')
                        {
                            ++position_;
                            break;
                        }
                    }
                    return yaml(std::move(items));
                }

                yaml parse_mapping(std::size_t depth)
                {
                    count_node(depth);
                    ++position_; // '{'
                    yaml::mapping entries;
                    skip();
                    if (position_ < text_.size() && text_[position_] == '}')
                    {
                        ++position_;
                        return yaml(std::move(entries));
                    }
                    while (true)
                    {
                        skip();
                        if (position_ >= text_.size())
                        {
                            fail("unterminated flow mapping");
                        }
                        std::string key;
                        if (text_[position_] == '\'' || text_[position_] == '"')
                        {
                            key = parse_quoted();
                        }
                        else
                        {
                            key = parse_plain(true);
                        }
                        skip();
                        if (position_ >= text_.size() || text_[position_] != ':')
                        {
                            fail("expected ':' in flow mapping");
                        }
                        ++position_;
                        skip();
                        yaml value;
                        if (position_ >= text_.size() || text_[position_] == ',' ||
                            text_[position_] == '}')
                        {
                            value = yaml();
                        }
                        else
                        {
                            value = parse_node(depth + 1);
                        }
                        if (!options_.allow_duplicate_keys)
                        {
                            for (const auto &entry : entries)
                            {
                                if (entry.first == key)
                                {
                                    fail("duplicate mapping key '" + key + "'");
                                }
                            }
                        }
                        entries.emplace_back(std::move(key), std::move(value));
                        skip();
                        if (position_ >= text_.size())
                        {
                            fail("unterminated flow mapping");
                        }
                        if (text_[position_] == '}')
                        {
                            ++position_;
                            break;
                        }
                        if (text_[position_] != ',')
                        {
                            fail("expected ',' or '}' in flow mapping");
                        }
                        ++position_;
                        skip();
                        if (position_ < text_.size() && text_[position_] == '}')
                        {
                            ++position_;
                            break;
                        }
                    }
                    return yaml(std::move(entries));
                }

                std::string text_;
                const parse_options &options_;
                std::unordered_map<std::string, yaml> &anchors_;
                std::size_t &nodes_;
                detail::mark base_;
                std::size_t position_{0};
            };

            class parser
            {
            public:
                parser(std::string source, const parse_options &options)
                    : source_(std::move(source)), options_(options)
                {
                }

                yaml parse()
                {
                    if (source_.find('\0') != std::string::npos)
                    {
                        fail("NUL bytes are not valid YAML");
                    }

                    skip_blank_lines();
                    while (!eof())
                    {
                        const line_info line = current_line();
                        if (line.indent != 0 || line.text.empty() || line.text.front() != '%')
                        {
                            break;
                        }
                        if (line.text.compare(0, 5, "%YAML") == 0)
                        {
                            const std::string rest = trim_copy(line.text.substr(5));
                            if (!rest.empty() && rest != "1.1" && rest != "1.2")
                            {
                                fail("unsupported YAML version in %YAML directive");
                            }
                        }
                        advance_to_next_line();
                        skip_blank_lines();
                    }

                    bool had_start = false;
                    if (is_marker("---"))
                    {
                        had_start = true;
                        const line_info line = current_line();
                        std::size_t position = line.content + 3;
                        while (position < line.end &&
                               (source_[position] == ' ' || source_[position] == '\t'))
                        {
                            ++position;
                        }
                        if (position < line.end && source_[position] != '#')
                        {
                            fail("content after '---' is not supported; put it on the next line");
                        }
                        advance_to_next_line();
                        skip_blank_lines();
                    }
                    (void)had_start;

                    yaml root;
                    if (!eof() && !is_marker("..."))
                    {
                        const line_info line = current_line();
                        set_at_content(line);
                        root = parse_node(line.indent, 0);
                    }

                    skip_blank_lines();
                    if (is_marker("..."))
                    {
                        advance_to_next_line();
                        skip_blank_lines();
                    }
                    if (!eof())
                    {
                        if (is_marker("---") || is_marker("..."))
                        {
                            fail("multiple YAML documents are not supported by yaml::parse");
                        }
                        fail("unexpected content after the document");
                    }
                    return root;
                }

            private:
                struct line_info
                {
                    std::size_t start{0};
                    std::size_t end{0};
                    std::size_t indent{0};
                    std::size_t content{0};
                    std::string_view text;
                    bool blank{true};
                    bool comment{false};
                };

                bool eof() const noexcept { return position_ >= source_.size(); }

                char peek(std::size_t offset = 0) const noexcept
                {
                    return position_ + offset < source_.size() ? source_[position_ + offset] : '\0';
                }

                void advance(std::size_t amount = 1)
                {
                    for (std::size_t i = 0; i < amount && position_ < source_.size(); ++i)
                    {
                        const char c = source_[position_++];
                        if (c == '\n')
                        {
                            ++line_;
                            line_start_ = position_;
                            column_ = 1;
                        }
                        else
                        {
                            ++column_;
                        }
                    }
                }

                line_info current_line() const
                {
                    line_info line;
                    line.start = line_start_;
                    const std::size_t newline = source_.find('\n', line_start_);
                    line.end = newline == std::string::npos ? source_.size() : newline;
                    std::size_t position = line_start_;
                    while (position < line.end && source_[position] == ' ')
                    {
                        ++position;
                    }
                    if (position < line.end && source_[position] == '\t')
                    {
                        throw parse_error("tabs are not allowed for indentation", line_,
                                          position - line_start_ + 1, position);
                    }
                    line.indent = position - line_start_;
                    line.content = position;
                    line.text = std::string_view(source_).substr(position, line.end - position);
                    line.blank = line.text.empty();
                    line.comment = !line.blank && line.text.front() == '#';
                    return line;
                }

                void set_at_content(const line_info &line)
                {
                    position_ = line.content;
                    column_ = line.indent + 1;
                }

                void advance_to_next_line()
                {
                    const std::size_t newline = source_.find('\n', position_);
                    if (newline == std::string::npos)
                    {
                        position_ = source_.size();
                        return;
                    }
                    position_ = newline + 1;
                    line_start_ = position_;
                    ++line_;
                    column_ = 1;
                }

                void skip_blank_lines()
                {
                    while (!eof())
                    {
                        const line_info line = current_line();
                        if (!line.blank && !line.comment)
                        {
                            break;
                        }
                        advance_to_next_line();
                    }
                }

                void skip_spaces()
                {
                    while (!eof() && (peek() == ' ' || peek() == '\t'))
                    {
                        advance();
                    }
                }

                bool at_line_end_or_comment()
                {
                    const std::size_t saved = position_;
                    skip_spaces();
                    const bool result = eof() || peek() == '\n' || peek() == '#';
                    position_ = saved;
                    return result;
                }

                void finish_line()
                {
                    skip_spaces();
                    if (!eof() && peek() == '#')
                    {
                        while (!eof() && peek() != '\n')
                        {
                            advance();
                        }
                    }
                    if (!eof() && peek() != '\n')
                    {
                        fail("unexpected trailing characters");
                    }
                    if (!eof())
                    {
                        advance();
                    }
                }

                bool is_marker(std::string_view marker) const
                {
                    try
                    {
                        const line_info line = current_line();
                        if (line.indent != 0 || !starts_with(line.text, marker))
                        {
                            return false;
                        }
                        if (line.text.size() == marker.size())
                        {
                            return true;
                        }
                        const char next = line.text[marker.size()];
                        return next == ' ' || next == '\t' || next == '#';
                    }
                    catch (...)
                    {
                        return false;
                    }
                }

                detail::mark current_mark() const
                {
                    return detail::mark{line_, column_, position_};
                }

                [[noreturn]] void fail(const std::string &message) const
                {
                    const detail::mark position = current_mark();
                    throw parse_error(message, position.line, position.column, position.index);
                }

                void count_node(std::size_t depth)
                {
                    if (depth > options_.max_depth)
                    {
                        fail("maximum nesting depth exceeded");
                    }
                    ++nodes_;
                    if (nodes_ > options_.max_nodes)
                    {
                        fail("maximum node count exceeded");
                    }
                }

                void add_alias_weight(const yaml &value)
                {
                    const std::size_t weight = node_weight(value);
                    if (nodes_ > options_.max_nodes - std::min(weight, options_.max_nodes))
                    {
                        fail("maximum node count exceeded while expanding an alias");
                    }
                    nodes_ += weight;
                }

                std::size_t find_block_colon(std::string_view text) const
                {
                    bool single = false;
                    bool double_quoted = false;
                    bool escaped = false;
                    int flow_depth = 0;
                    for (std::size_t i = 0; i < text.size(); ++i)
                    {
                        const char c = text[i];
                        if (double_quoted)
                        {
                            if (escaped)
                            {
                                escaped = false;
                            }
                            else if (c == '\\')
                            {
                                escaped = true;
                            }
                            else if (c == '"')
                            {
                                double_quoted = false;
                            }
                            continue;
                        }
                        if (single)
                        {
                            if (c == '\'')
                            {
                                if (i + 1 < text.size() && text[i + 1] == '\'')
                                {
                                    ++i;
                                }
                                else
                                {
                                    single = false;
                                }
                            }
                            continue;
                        }
                        if (c == '"')
                        {
                            double_quoted = true;
                        }
                        else if (c == '\'')
                        {
                            single = true;
                        }
                        else if (c == '[' || c == '{')
                        {
                            ++flow_depth;
                        }
                        else if (c == ']' || c == '}')
                        {
                            if (flow_depth > 0)
                            {
                                --flow_depth;
                            }
                        }
                        else if (c == '#' &&
                                 (i == 0 || text[i - 1] == ' ' || text[i - 1] == '\t'))
                        {
                            break;
                        }
                        else if (c == ':' && flow_depth == 0 &&
                                 (i + 1 == text.size() || text[i + 1] == ' ' ||
                                  text[i + 1] == '\t'))
                        {
                            return i;
                        }
                    }
                    return std::string_view::npos;
                }

                yaml parse_node(std::size_t indent, std::size_t depth)
                {
                    count_node(depth);
                    skip_blank_lines();
                    if (eof() || is_marker("...") || is_marker("---"))
                    {
                        return yaml();
                    }
                    const line_info line = current_line();
                    if (line.indent < indent)
                    {
                        return yaml();
                    }
                    indent = line.indent;
                    set_at_content(line);
                    if (is_sequence_entry(line.text))
                    {
                        return parse_sequence(indent, depth);
                    }
                    if (find_block_colon(line.text) != std::string_view::npos)
                    {
                        return parse_mapping(indent, depth);
                    }
                    if (!line.text.empty() && line.text.front() == '?')
                    {
                        fail("explicit mapping keys ('?') are not supported");
                    }
                    return parse_value(indent, depth + 1);
                }

                yaml parse_block_value(std::size_t parent_indent, std::size_t depth)
                {
                    skip_blank_lines();
                    if (eof() || is_marker("...") || is_marker("---"))
                    {
                        return yaml();
                    }
                    const line_info line = current_line();
                    if (line.indent > parent_indent)
                    {
                        set_at_content(line);
                        return parse_node(line.indent, depth);
                    }
                    if (line.indent == parent_indent && is_sequence_entry(line.text))
                    {
                        set_at_content(line);
                        return parse_sequence(line.indent, depth);
                    }
                    return yaml();
                }

                void insert_mapping(yaml::mapping &entries, std::string key, yaml value)
                {
                    if (!options_.allow_duplicate_keys)
                    {
                        for (const auto &entry : entries)
                        {
                            if (entry.first == key)
                            {
                                fail("duplicate mapping key '" + key + "'");
                            }
                        }
                    }
                    entries.emplace_back(std::move(key), std::move(value));
                }

                std::string parse_key(std::string_view raw, const detail::mark &position)
                {
                    const std::string trimmed = trim_copy(raw);
                    if (trimmed.empty())
                    {
                        fail("empty mapping key");
                    }
                    if (trimmed.front() == '\'' || trimmed.front() == '"')
                    {
                        const char quote = trimmed.front();
                        if (trimmed.back() != quote || trimmed.size() < 2)
                        {
                            throw parse_error("unterminated quoted mapping key", position.line,
                                              position.column, position.index);
                        }
                        return decode_quoted(trimmed, quote == '\'', position);
                    }
                    if (trimmed.front() == '?' && (trimmed.size() == 1 || trimmed[1] == ' '))
                    {
                        fail("explicit mapping keys ('?') are not supported");
                    }
                    if (trimmed.front() == '&' || trimmed.front() == '!')
                    {
                        fail("anchors and tags on mapping keys are not supported");
                    }
                    return trimmed;
                }

                std::pair<std::string, yaml> parse_mapping_entry(std::size_t indent,
                                                                 std::size_t depth)
                {
                    const line_info line = current_line();
                    const detail::mark position = current_mark();
                    if (position_ >= line.end)
                    {
                        fail("expected a mapping key");
                    }
                    const std::string_view rest =
                        std::string_view(source_).substr(position_, line.end - position_);
                    const std::size_t colon = find_block_colon(rest);
                    if (colon == std::string_view::npos)
                    {
                        fail("expected ':' in mapping entry");
                    }
                    std::string key = parse_key(rest.substr(0, colon), position);
                    advance(colon);
                    if (peek() != ':')
                    {
                        fail("expected ':' in mapping entry");
                    }
                    advance();
                    skip_spaces();

                    yaml value;
                    if (at_line_end_or_comment())
                    {
                        advance_to_next_line();
                        value = parse_block_value(indent, depth + 1);
                    }
                    else
                    {
                        value = parse_value(indent, depth + 1);
                    }
                    return {std::move(key), std::move(value)};
                }

                yaml parse_mapping(std::size_t indent, std::size_t depth)
                {
                    yaml::mapping entries;
                    while (true)
                    {
                        skip_blank_lines();
                        if (eof() || is_marker("...") || is_marker("---"))
                        {
                            break;
                        }
                        const line_info line = current_line();
                        if (line.indent < indent)
                        {
                            break;
                        }
                        if (line.indent > indent)
                        {
                            fail("unexpected indentation in mapping");
                        }
                        if (is_sequence_entry(line.text))
                        {
                            break;
                        }
                        if (line.text.empty() || line.text.front() == '#')
                        {
                            break;
                        }
                        set_at_content(line);
                        auto entry = parse_mapping_entry(indent, depth);
                        insert_mapping(entries, std::move(entry.first), std::move(entry.second));
                    }
                    return yaml(std::move(entries));
                }

                yaml parse_mapping_inline(std::size_t indent, std::size_t depth)
                {
                    count_node(depth);
                    yaml::mapping entries;
                    auto first = parse_mapping_entry(indent, depth);
                    insert_mapping(entries, std::move(first.first), std::move(first.second));
                    while (true)
                    {
                        skip_blank_lines();
                        if (eof() || is_marker("...") || is_marker("---"))
                        {
                            break;
                        }
                        const line_info line = current_line();
                        if (line.indent != indent)
                        {
                            break;
                        }
                        if (is_sequence_entry(line.text))
                        {
                            break;
                        }
                        set_at_content(line);
                        auto entry = parse_mapping_entry(indent, depth);
                        insert_mapping(entries, std::move(entry.first), std::move(entry.second));
                    }
                    return yaml(std::move(entries));
                }

                yaml parse_sequence(std::size_t indent, std::size_t depth, bool first_inline = false)
                {
                    count_node(depth);
                    yaml::sequence items;
                    bool first = true;
                    while (true)
                    {
                        if (!(first && first_inline))
                        {
                            skip_blank_lines();
                            if (eof() || is_marker("...") || is_marker("---"))
                            {
                                break;
                            }
                            const line_info line = current_line();
                            if (line.indent < indent)
                            {
                                break;
                            }
                            if (line.indent > indent)
                            {
                                fail("unexpected indentation in sequence");
                            }
                            if (!is_sequence_entry(line.text))
                            {
                                break;
                            }
                            set_at_content(line);
                        }

                        const line_info line = current_line();
                        if (peek() != '-' || !is_sequence_entry(
                                                  std::string_view(source_).substr(
                                                      position_, line.end - position_)))
                        {
                            fail("expected a sequence entry");
                        }
                        advance(); // '-'
                        skip_spaces();

                        yaml value;
                        if (at_line_end_or_comment())
                        {
                            advance_to_next_line();
                            value = parse_block_value(indent, depth + 1);
                        }
                        else
                        {
                            const std::string_view rest = std::string_view(source_).substr(
                                position_, line.end - position_);
                            if (is_sequence_entry(rest))
                            {
                                value = parse_sequence(column_ - 1, depth + 1, true);
                            }
                            else if (find_block_colon(rest) != std::string_view::npos)
                            {
                                value = parse_mapping_inline(column_ - 1, depth + 1);
                            }
                            else
                            {
                                value = parse_value(indent, depth + 1);
                            }
                        }
                        items.push_back(std::move(value));
                        first = false;
                        first_inline = false;
                    }
                    return yaml(std::move(items));
                }

                yaml parse_block_scalar(std::size_t parent_indent)
                {
                    const detail::mark start = current_mark();
                    const char style = peek();
                    advance();

                    char chomping = '\0';
                    int explicit_indent = 0;
                    bool seen_chomping = false;
                    bool seen_indent = false;
                    while (!eof() && !is_space(peek()) && peek() != '\n' && peek() != '#')
                    {
                        const char c = peek();
                        if ((c == '+' || c == '-') && !seen_chomping)
                        {
                            chomping = c;
                            seen_chomping = true;
                            advance();
                        }
                        else if (c >= '1' && c <= '9' && !seen_indent)
                        {
                            explicit_indent = c - '0';
                            seen_indent = true;
                            advance();
                        }
                        else
                        {
                            fail("invalid block scalar header");
                        }
                    }
                    skip_spaces();
                    if (!eof() && peek() == '#')
                    {
                        while (!eof() && peek() != '\n')
                        {
                            advance();
                        }
                    }
                    if (!eof() && peek() != '\n')
                    {
                        fail("invalid block scalar header");
                    }
                    if (!eof())
                    {
                        advance();
                    }

                    std::size_t content_indent = 0;
                    if (explicit_indent > 0)
                    {
                        content_indent = parent_indent + static_cast<std::size_t>(explicit_indent);
                    }
                    else
                    {
                        std::size_t scan = position_;
                        bool found = false;
                        while (scan < source_.size())
                        {
                            const std::size_t end = source_.find('\n', scan);
                            const std::size_t line_end =
                                end == std::string::npos ? source_.size() : end;
                            std::size_t p = scan;
                            while (p < line_end && source_[p] == ' ')
                            {
                                ++p;
                            }
                            if (p < line_end)
                            {
                                content_indent = p - scan;
                                found = true;
                                break;
                            }
                            if (end == std::string::npos)
                            {
                                break;
                            }
                            scan = end + 1;
                        }
                        if (!found || content_indent <= parent_indent)
                        {
                            return yaml(std::string());
                        }
                    }

                    std::vector<std::string> lines;
                    while (!eof())
                    {
                        const line_info line = current_line();
                        if (line.blank)
                        {
                            lines.emplace_back();
                            advance_to_next_line();
                            continue;
                        }
                        if (line.indent < content_indent)
                        {
                            break;
                        }
                        const std::size_t content_start = line.start + content_indent;
                        lines.emplace_back(source_.substr(content_start, line.end - content_start));
                        advance_to_next_line();
                    }

                    std::string body;
                    if (style == '|')
                    {
                        for (const std::string &line : lines)
                        {
                            body += line;
                            body.push_back('\n');
                        }
                    }
                    else
                    {
                        bool previous_nonempty = false;
                        for (const std::string &line : lines)
                        {
                            if (line.empty())
                            {
                                body.push_back('\n');
                                previous_nonempty = false;
                                continue;
                            }
                            const bool more_indented = line.front() == ' ' || line.front() == '\t';
                            if (previous_nonempty)
                            {
                                body += more_indented ? "\n" : " ";
                            }
                            body += line;
                            previous_nonempty = true;
                        }
                        if (!lines.empty())
                        {
                            body.push_back('\n');
                        }
                    }

                    if (chomping == '-')
                    {
                        while (!body.empty() && body.back() == '\n')
                        {
                            body.pop_back();
                        }
                    }
                    else if (chomping != '+')
                    {
                        while (!body.empty() && body.back() == '\n')
                        {
                            body.pop_back();
                        }
                        bool has_content = false;
                        for (const std::string &line : lines)
                        {
                            if (!line.empty())
                            {
                                has_content = true;
                                break;
                            }
                        }
                        if (has_content)
                        {
                            body.push_back('\n');
                        }
                    }
                    (void)start;
                    return yaml(std::move(body));
                }

                std::string parse_quoted_block()
                {
                    const detail::mark start = current_mark();
                    const char quote = peek();
                    const bool single = quote == '\'';
                    std::string raw;
                    raw.push_back(quote);
                    advance();
                    bool closed = false;
                    while (!eof())
                    {
                        const char c = peek();
                        if (single)
                        {
                            raw.push_back(c);
                            advance();
                            if (c == '\'')
                            {
                                if (!eof() && peek() == '\'')
                                {
                                    raw.push_back('\'');
                                    advance();
                                }
                                else
                                {
                                    closed = true;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            raw.push_back(c);
                            advance();
                            if (c == '\\')
                            {
                                if (eof())
                                {
                                    break;
                                }
                                raw.push_back(peek());
                                advance();
                            }
                            else if (c == '"')
                            {
                                closed = true;
                                break;
                            }
                        }
                    }
                    if (!closed)
                    {
                        throw parse_error("unterminated quoted scalar", start.line, start.column,
                                          start.index);
                    }
                    return decode_quoted(raw, single, start);
                }

                std::string collect_flow()
                {
                    const detail::mark start = current_mark();
                    std::string out;
                    std::vector<char> stack;
                    bool single = false;
                    bool double_quoted = false;
                    bool escaped = false;

                    // A quote only opens a quoted scalar at the start of a
                    // value. Quotes embedded in a plain scalar (for example an
                    // apostrophe in "don't") are ordinary characters.
                    auto at_scalar_start = [&]()
                    {
                        std::size_t i = out.size();
                        while (i > 0 && (out[i - 1] == ' ' || out[i - 1] == '\t' ||
                                         out[i - 1] == '\n'))
                        {
                            --i;
                        }
                        if (i == 0)
                        {
                            return true;
                        }
                        const char previous = out[i - 1];
                        if (previous == '[' || previous == '{' || previous == ',' ||
                            previous == ':')
                        {
                            return true;
                        }
                        std::size_t token_start = i;
                        while (token_start > 0 && out[token_start - 1] != ' ' &&
                               out[token_start - 1] != '\t' && out[token_start - 1] != '\n' &&
                               out[token_start - 1] != '[' && out[token_start - 1] != '{' &&
                               out[token_start - 1] != ',' && out[token_start - 1] != ':')
                        {
                            --token_start;
                        }
                        return token_start < i &&
                               (out[token_start] == '&' || out[token_start] == '!');
                    };

                    while (!eof())
                    {
                        const char c = peek();
                        if (double_quoted)
                        {
                            out.push_back(c);
                            advance();
                            if (escaped)
                            {
                                escaped = false;
                            }
                            else if (c == '\\')
                            {
                                escaped = true;
                            }
                            else if (c == '"')
                            {
                                double_quoted = false;
                            }
                            continue;
                        }
                        if (single)
                        {
                            out.push_back(c);
                            advance();
                            if (c == '\'')
                            {
                                if (!eof() && peek() == '\'')
                                {
                                    out.push_back('\'');
                                    advance();
                                }
                                else
                                {
                                    single = false;
                                }
                            }
                            continue;
                        }
                        if (c == '#' &&
                            (out.empty() || out.back() == ' ' || out.back() == '\t' ||
                             out.back() == '\n'))
                        {
                            while (!eof() && peek() != '\n')
                            {
                                advance();
                            }
                            out.push_back('\n');
                            if (!eof())
                            {
                                advance();
                            }
                            continue;
                        }

                        const bool opens_quote =
                            (c == '\'' || c == '"') && at_scalar_start();
                        out.push_back(c);
                        advance();
                        if (c == '\'' && opens_quote)
                        {
                            single = true;
                        }
                        else if (c == '"' && opens_quote)
                        {
                            double_quoted = true;
                        }
                        else if (c == '[' || c == '{')
                        {
                            stack.push_back(c);
                        }
                        else if (c == ']' || c == '}')
                        {
                            if (stack.empty())
                            {
                                throw parse_error("unmatched flow collection terminator",
                                                  start.line, start.column, start.index);
                            }
                            const char open = stack.back();
                            if ((open == '[' && c != ']') || (open == '{' && c != '}'))
                            {
                                throw parse_error("mismatched flow collection terminator",
                                                  start.line, start.column, start.index);
                            }
                            stack.pop_back();
                            if (stack.empty())
                            {
                                return out;
                            }
                        }
                    }
                    throw parse_error("unterminated flow collection", start.line, start.column,
                                      start.index);
                }

                yaml parse_value(std::size_t parent_indent, std::size_t depth)
                {
                    count_node(depth);
                    const detail::mark value_mark = current_mark();
                    std::string anchor;
                    std::string tag;

                    while (!eof() && (peek() == '&' || peek() == '!'))
                    {
                        if (peek() == '&')
                        {
                            advance();
                            const std::size_t start = position_;
                            while (!eof() && !is_space(peek()) && peek() != '\n' &&
                                   peek() != '#')
                            {
                                advance();
                            }
                            anchor = source_.substr(start, position_ - start);
                            if (anchor.empty())
                            {
                                fail("empty anchor name");
                            }
                        }
                        else
                        {
                            const std::size_t start = position_;
                            advance();
                            if (!eof() && peek() == '<')
                            {
                                while (!eof() && peek() != '>')
                                {
                                    advance();
                                }
                                if (eof())
                                {
                                    fail("unterminated tag");
                                }
                                advance();
                            }
                            else
                            {
                                while (!eof() && !is_space(peek()) && peek() != '\n' &&
                                       peek() != '#')
                                {
                                    advance();
                                }
                            }
                            tag = source_.substr(start, position_ - start);
                        }
                        skip_spaces();
                    }

                    if (!eof() && peek() == '*')
                    {
                        if (!anchor.empty() || !tag.empty())
                        {
                            fail("aliases cannot have anchors or tags");
                        }
                        advance();
                        const std::size_t start = position_;
                        while (!eof() && !is_space(peek()) && peek() != '\n' && peek() != '#')
                        {
                            advance();
                        }
                        const std::string name = source_.substr(start, position_ - start);
                        const auto found = anchors_.find(name);
                        if (found == anchors_.end())
                        {
                            fail("undefined alias '*" + name + "'");
                        }
                        yaml value = found->second;
                        add_alias_weight(value);
                        finish_line();
                        return value;
                    }

                    yaml value;
                    if (at_line_end_or_comment())
                    {
                        advance_to_next_line();
                        value = parse_block_value(parent_indent, depth + 1);
                    }
                    else if (peek() == '|' || peek() == '>')
                    {
                        value = parse_block_scalar(parent_indent);
                    }
                    else if (peek() == '[' || peek() == '{')
                    {
                        const detail::mark base = current_mark();
                        std::string flow = collect_flow();
                        flow_parser flow_value(std::move(flow), options_, anchors_, nodes_, base);
                        value = flow_value.parse();
                        finish_line();
                    }
                    else if (peek() == '\'' || peek() == '"')
                    {
                        value = yaml(parse_quoted_block());
                        finish_line();
                    }
                    else
                    {
                        const line_info line = current_line();
                        const std::size_t start = position_;
                        std::size_t end = start;
                        while (end < line.end)
                        {
                            const char c = source_[end];
                            if (c == '#' &&
                                (end == start || source_[end - 1] == ' ' ||
                                 source_[end - 1] == '\t'))
                            {
                                break;
                            }
                            ++end;
                        }
                        const std::string text = trim_copy(
                            std::string_view(source_).substr(start, end - start));
                        advance(end - start);
                        finish_line();
                        value = infer_scalar(text);
                    }

                    value = apply_tag(std::move(value), tag, value_mark);
                    if (!anchor.empty())
                    {
                        anchors_[anchor] = value;
                    }
                    return value;
                }

                std::string source_;
                const parse_options &options_;
                std::size_t position_{0};
                std::size_t line_{1};
                std::size_t column_{1};
                std::size_t line_start_{0};
                std::size_t nodes_{0};
                std::unordered_map<std::string, yaml> anchors_;
            };

            yaml parse_document(std::string text, const parse_options &options)
            {
                if (text.size() > options.max_input_size)
                {
                    throw parse_error("input exceeds the configured maximum size");
                }
                text = normalize_newlines(std::move(text));
                parser parser(std::move(text), options);
                return parser.parse();
            }

            std::string read_stream(std::istream &stream)
            {
                std::ostringstream buffer;
                buffer << stream.rdbuf();
                return buffer.str();
            }

            std::string read_file_contents(const std::string &path)
            {
                std::ifstream file(path, std::ios::binary);
                if (!file)
                {
                    throw io_error("unable to open file '" + path + "'");
                }
                std::ostringstream buffer;
                buffer << file.rdbuf();
                if (!file.good() && !file.eof())
                {
                    throw io_error("unable to read file '" + path + "'");
                }
                return buffer.str();
            }
        } // namespace

        //---------------------------------------------------------------------
        // [SECTION] Parsing entry points
        //---------------------------------------------------------------------

#if MYYAML_READER_ENABLED
        yaml yaml::parse(std::string_view text, const parse_options &options)
        {
            const std::string bytes(text);
            const std::string decoded = detail::decode_to_utf8(bytes);
            return parse_document(decoded, options);
        }

        yaml yaml::parse(std::istream &stream, const parse_options &options)
        {
            if (!stream.good())
            {
                throw io_error("input stream is not readable");
            }
            const std::string bytes = read_stream(stream);
            if (stream.bad())
            {
                throw io_error("failed while reading input stream");
            }
            return parse(std::string_view(bytes), options);
        }

        yaml yaml::parse(std::FILE *file, const parse_options &options)
        {
            if (file == nullptr)
            {
                throw io_error("file pointer is null");
            }
            detail::file_iadapter adapter(file);
            const std::string bytes = detail::read_all(adapter);
            if (std::ferror(file) != 0)
            {
                throw io_error("failed while reading file");
            }
            return parse(std::string_view(bytes), options);
        }

        yaml yaml::parse_file(const std::string &path, const parse_options &options)
        {
            const std::string bytes = read_file_contents(path);
            return parse(std::string_view(bytes), options);
        }

        parse_result yaml::try_parse(std::string_view text, const parse_options &options) noexcept
        {
            parse_result result;
            try
            {
                result.value = parse(text, options);
            }
            catch (const parse_error &error)
            {
                result.error = error.what();
                result.line = error.line();
                result.column = error.column();
                result.index = error.index();
            }
            catch (const exception &error)
            {
                result.error = error.what();
            }
            catch (const std::exception &error)
            {
                result.error = error.what();
            }
            catch (...)
            {
                result.error = "unknown parsing error";
            }
            return result;
        }
#endif // MYYAML_READER_ENABLED

        //---------------------------------------------------------------------
        // [SECTION] Emitter
        //---------------------------------------------------------------------

        namespace
        {
            class emitter
            {
            public:
                emitter(const dump_options &options) : options_(options)
                {
                    if (options_.indent < 1 || options_.indent > 8)
                    {
                        throw type_error("dump indent must be between 1 and 8");
                    }
                    step_ = options_.indent;
                }

                std::string emit(const yaml &value)
                {
                    if (options_.flow)
                    {
                        emit_flow(value);
                        output_.push_back('\n');
                    }
                    else
                    {
                        emit_block_node(value, 0);
                    }
                    if (options_.trailing_newline && !output_.empty() && output_.back() != '\n')
                    {
                        output_.push_back('\n');
                    }
                    if (!options_.trailing_newline && !output_.empty() && output_.back() == '\n')
                    {
                        output_.pop_back();
                    }

                    if (options_.explicit_start)
                    {
                        output_.insert(0, "---\n");
                    }
                    if (options_.explicit_end)
                    {
                        if (!output_.empty() && output_.back() != '\n')
                        {
                            output_.push_back('\n');
                        }
                        output_ += "...\n";
                    }
                    return output_;
                }

            private:
                void indent_line(int indent)
                {
                    output_.append(static_cast<std::size_t>(indent), ' ');
                }

                std::string key_text(const std::string &key) const
                {
                    if (is_plain_key_safe(key))
                    {
                        return key;
                    }
                    return escape_double_quoted(key);
                }

                std::string scalar_text(const yaml &value, bool flow_context) const
                {
                    switch (value.type())
                    {
                    case node_t::null:
                        return "null";
                    case node_t::boolean:
                        return value.as_boolean() ? "true" : "false";
                    case node_t::integer:
                        return format_integer(value.as_integer());
                    case node_t::floating:
                        return format_double(value.as_floating());
                    case node_t::string:
                    {
                        const std::string &text = value.as_string();
                        if (!detail::utf8::validate(text))
                        {
                            throw encoding_error("cannot emit a string containing invalid UTF-8");
                        }
                        if (flow_context || !can_use_block_scalar(text))
                        {
                            if (is_plain_safe(text) && !flow_context)
                            {
                                return text;
                            }
                            if (is_plain_safe(text) && flow_context)
                            {
                                return text;
                            }
                            return escape_double_quoted(text);
                        }
                        return text; // handled by the caller as a block scalar
                    }
                    default:
                        return {};
                    }
                }

                static bool can_use_block_scalar(const std::string &text)
                {
                    if (text.find('\n') == std::string::npos)
                    {
                        return false;
                    }
                    for (unsigned char c : text)
                    {
                        if ((c < 0x20 && c != '\n' && c != '\t') || c == 0x7F)
                        {
                            return false;
                        }
                    }
                    return true;
                }

                void emit_block_scalar(const std::string &text, int parent_indent)
                {
                    std::size_t trailing = 0;
                    for (std::size_t i = text.size(); i > 0 && text[i - 1] == '\n'; --i)
                    {
                        ++trailing;
                    }
                    char chomping = '\0';
                    if (trailing == 0)
                    {
                        chomping = '-';
                    }
                    else if (trailing > 1)
                    {
                        chomping = '+';
                    }

                    output_.push_back('|');
                    output_.push_back(static_cast<char>('0' + step_));
                    if (chomping != '\0')
                    {
                        output_.push_back(chomping);
                    }
                    output_.push_back('\n');

                    const int content_indent = parent_indent + step_;
                    std::size_t start = 0;
                    while (start < text.size())
                    {
                        const std::size_t newline = text.find('\n', start);
                        const std::size_t end =
                            newline == std::string::npos ? text.size() : newline;
                        indent_line(content_indent);
                        output_.append(text, start, end - start);
                        output_.push_back('\n');
                        if (newline == std::string::npos)
                        {
                            break;
                        }
                        start = newline + 1;
                        if (start == text.size())
                        {
                            break;
                        }
                    }
                }

                void emit_value(const yaml &value, int parent_indent)
                {
                    if (value.is_string() && can_use_block_scalar(value.as_string()) &&
                        !options_.flow)
                    {
                        output_.push_back(' ');
                        emit_block_scalar(value.as_string(), parent_indent);
                        return;
                    }
                    if (value.is_mapping() && !value.as_mapping().empty() && !options_.flow)
                    {
                        output_.push_back('\n');
                        emit_block_mapping(value, parent_indent + step_);
                        return;
                    }
                    if (value.is_sequence() && !value.as_sequence().empty() && !options_.flow)
                    {
                        output_.push_back('\n');
                        emit_block_sequence(value, parent_indent + step_);
                        return;
                    }
                    output_.push_back(' ');
                    if (options_.flow || value.is_mapping() || value.is_sequence())
                    {
                        emit_flow(value);
                    }
                    else
                    {
                        output_ += scalar_text(value, false);
                    }
                    output_.push_back('\n');
                }

                void emit_block_mapping(const yaml &value, int indent, bool first_inline = false)
                {
                    yaml::mapping entries = value.as_mapping();
                    if (options_.sort_keys)
                    {
                        std::stable_sort(entries.begin(), entries.end(),
                                         [](const auto &lhs, const auto &rhs)
                                         { return lhs.first < rhs.first; });
                    }
                    bool first = true;
                    for (const auto &entry : entries)
                    {
                        if (!(first && first_inline))
                        {
                            indent_line(indent);
                        }
                        output_ += key_text(entry.first);
                        output_.push_back(':');
                        emit_value(entry.second, indent);
                        first = false;
                    }
                }

                void emit_block_sequence(const yaml &value, int indent)
                {
                    for (const yaml &item : value.as_sequence())
                    {
                        indent_line(indent);
                        output_.push_back('-');
                        if (item.is_mapping() && !item.as_mapping().empty() && !options_.flow)
                        {
                            output_.push_back(' ');
                            emit_block_mapping(item, indent + step_, true);
                        }
                        else
                        {
                            emit_value(item, indent);
                        }
                    }
                }

                void emit_block_node(const yaml &value, int indent)
                {
                    if (value.is_mapping() && !value.as_mapping().empty())
                    {
                        emit_block_mapping(value, indent);
                        return;
                    }
                    if (value.is_sequence() && !value.as_sequence().empty())
                    {
                        emit_block_sequence(value, indent);
                        return;
                    }
                    if (value.is_mapping())
                    {
                        output_ += "{}\n";
                        return;
                    }
                    if (value.is_sequence())
                    {
                        output_ += "[]\n";
                        return;
                    }
                    if (value.is_string() && can_use_block_scalar(value.as_string()))
                    {
                        emit_block_scalar(value.as_string(), 0);
                        return;
                    }
                    output_ += scalar_text(value, false);
                    output_.push_back('\n');
                }

                void emit_flow(const yaml &value)
                {
                    switch (value.type())
                    {
                    case node_t::mapping:
                    {
                        yaml::mapping entries = value.as_mapping();
                        if (options_.sort_keys)
                        {
                            std::stable_sort(entries.begin(), entries.end(),
                                             [](const auto &lhs, const auto &rhs)
                                             { return lhs.first < rhs.first; });
                        }
                        output_.push_back('{');
                        bool first = true;
                        for (const auto &entry : entries)
                        {
                            if (!first)
                            {
                                output_ += ", ";
                            }
                            output_ += key_text(entry.first);
                            output_ += ": ";
                            emit_flow(entry.second);
                            first = false;
                        }
                        output_.push_back('}');
                        break;
                    }
                    case node_t::sequence:
                    {
                        output_.push_back('[');
                        bool first = true;
                        for (const yaml &item : value.as_sequence())
                        {
                            if (!first)
                            {
                                output_ += ", ";
                            }
                            emit_flow(item);
                            first = false;
                        }
                        output_.push_back(']');
                        break;
                    }
                    case node_t::string:
                        output_ += scalar_text(value, true);
                        break;
                    default:
                        output_ += scalar_text(value, true);
                        break;
                    }
                }

                const dump_options &options_;
                int step_{2};
                std::string output_;
            };
        } // namespace

        std::string yaml::dump(const dump_options &options) const
        {
            emitter writer(options);
            return writer.emit(*this);
        }

        void yaml::dump(std::ostream &stream, const dump_options &options) const
        {
            const std::string text = dump(options);
            stream.write(text.data(), static_cast<std::streamsize>(text.size()));
            if (!stream.good())
            {
                throw io_error("failed while writing YAML to output stream");
            }
        }

        std::ostream &operator<<(std::ostream &stream, const yaml &value)
        {
            value.dump(stream);
            return stream;
        }

#if MYYAML_READER_ENABLED
        std::istream &operator>>(std::istream &stream, yaml &value)
        {
            try
            {
                value = yaml::parse(stream);
            }
            catch (...)
            {
                stream.setstate(std::ios::failbit);
            }
            return stream;
        }
#endif

        //---------------------------------------------------------------------
        // [SECTION] Stringification
        //---------------------------------------------------------------------

        const char *string(encoding value) noexcept
        {
            switch (value)
            {
            case encoding::utf8:
                return "utf8";
            case encoding::utf16:
                return "utf16";
            case encoding::utf16le:
                return "utf16le";
            case encoding::utf16be:
                return "utf16be";
            case encoding::utf32:
                return "utf32";
            case encoding::utf32le:
                return "utf32le";
            case encoding::utf32be:
                return "utf32be";
            case encoding::unspecified:
            default:
                return "unspecified";
            }
        }

        const char *string(node_t value) noexcept
        {
            switch (value)
            {
            case node_t::null:
                return "null";
            case node_t::boolean:
                return "boolean";
            case node_t::integer:
                return "integer";
            case node_t::floating:
                return "floating";
            case node_t::string:
                return "string";
            case node_t::sequence:
                return "sequence";
            case node_t::mapping:
                return "mapping";
            default:
                return "unknown";
            }
        }

        std::ostream &operator<<(std::ostream &stream, const encoding &value)
        {
            stream << string(value);
            return stream;
        }

        std::ostream &operator<<(std::ostream &stream, const node_t &value)
        {
            stream << string(value);
            return stream;
        }
    } // namespace v0_2_0
} // namespace myyaml
