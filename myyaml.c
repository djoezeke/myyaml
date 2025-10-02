/**
 * @file myyaml.c
 * @brief YAML parser library header for C/C++.
 * This is a  C/C++ Yaml Parser Library Source @c myyaml.h.
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
 *
 *  Internal:
 *
 *  [SECTION] Includes
 *  [SECTION] Macro Defines
 *  [SECTION] Data Structures
 *  [SECTION] C Only Functions
 *    - [SECTION] Declarations
 *    - [SECTION] Definations
 *  [SECTION] C++ Only Classes
 *    - [SECTION] Declarations
 *    - [SECTION] Definations
 *
 *  Myyaml:
 *
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

//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#include "myyaml.h"

#pragma region Internal

//-------------------------------------------------------------------------
// [SECTION] Defines
//-----------------------------------------------------------------------------

/**
 * @def LOG_ERR
 * @brief Macro to log error message to stderr.
 * @note It also specifies which file, line and function the error was raised in.
 */
#define LOG_ERR(...)                           \
    do                                         \
    {                                          \
        fprintf(stderr, "%s:%d [%s]: ",        \
                __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__);          \
    } while (0)

/**
 * @def RETURN_IF_FAILED
 * @brief Macro to check `COND` and return if it fails.
 * @param COND expression to check.
 * @note returns NULL from the location where it is called.
 */
#define RETURN_IF_FAILED(COND, ...) \
    do                              \
    {                               \
        if (!(COND))                \
        {                           \
            LOG_ERR(__VA_ARGS__);   \
            return NULL;            \
        }                           \
    } while (0)

/**
 * @def FUNC_IF_FAILED
 * @brief Macro to check `COND` and calls `FUNC` with args if it fails.
 * @param COND expression to check.
 * @param FUNC fuction to call.
 * @note returns NULL from the location where it is called.
 */
#define FUNC_IF_FAILED(COND, FUNC, ...) \
    do                                  \
    {                                   \
        if (!(COND))                    \
        {                               \
            FUNC(__VA_ARGS__);          \
        }                               \
    } while (0)

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    //-----------------------------------------------------------------------------
    // [SECTION] Declarations
    //-----------------------------------------------------------------------------

    char *read_file(const char *filename);
    const char *skip_whitespace(const char *str);
    void append_to_buffer(char **buffer, size_t *size, const char *format, ...);
    void yaml_pretty_print_to_string(Yaml *value, int indent, char **buffer, size_t *size);
    int write_string_to_file(const char *filename, const char *content);

    //-----------------------------------------------------------------------------
    // [SECTION] Definations
    //-----------------------------------------------------------------------------

    // Function to read the contents of a file into a string
    char *read_file(const char *filename)
    {
        FILE *file = fopen(filename, "r");
        if (!file)
        {
            perror("Failed to open file");
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *content = (char *)malloc(length + 1);
        if (!content)
        {
            perror("Failed to allocate memory");
            fclose(file);
            return NULL;
        }

        fread(content, 1, length, file);
        content[length] = '\0';

        fclose(file);
        return content;
    }

    // Function to skip whitespace characters
    const char *skip_whitespace(const char *str)
    {
        while (isspace(*str))
            str++;
        return str;
    }

    // Helper function to append formatted text to a string buffer
    void append_to_buffer(char **buffer, size_t *size, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        size_t needed = vsnprintf(NULL, 0, format, args) + 1;
        va_end(args);

        *buffer = (char *)realloc(*buffer, *size + needed);
        va_start(args, format);
        vsnprintf(*buffer + *size, needed, format, args);
        va_end(args);

        *size += needed - 1;
    }

    // Function to pretty print a YAML value to a string
    void yaml_pretty_print_to_string(Yaml *value, int indent, char **buffer, size_t *size)
    {
        for (int i = 0; i < indent; i++)
            append_to_buffer(buffer, size, "  ");
        switch (value->type)
        {
        case SCALAR:
            append_to_buffer(buffer, size, "%s\n", value->data.scalar_value);
            break;
        case SEQUENCE:
            for (size_t i = 0; i < value->data.sequence_value.size; i++)
            {
                for (int j = 0; j < indent; j++)
                    append_to_buffer(buffer, size, "  ");
                append_to_buffer(buffer, size, "- ");
                yaml_pretty_print_to_string(value->data.sequence_value.items[i], indent + 1, buffer, size);
            }
            break;
        case MAPPING:
            for (size_t i = 0; i < value->data.mapping_value.size; i++)
            {
                for (int j = 0; j < indent; j++)
                    append_to_buffer(buffer, size, "  ");
                append_to_buffer(buffer, size, "%s: ", value->data.mapping_value.items[i]->key);
                yaml_pretty_print_to_string(value->data.mapping_value.items[i]->value, indent + 1, buffer, size);
            }
            break;
        }
    }

    // Function to write a string to a file
    int write_string_to_file(const char *filename, const char *content)
    {
        FILE *file = fopen(filename, "w");
        if (!file)
        {
            perror("Failed to open file");
            return -1;
        }

        fprintf(file, "%s", content);
        fclose(file);
        return 0;
    }

    inline const char *to_string(YamlType t)
    {
        switch (t)
        {
        case SEQUENCE:
            return "SEQUENCE";
        case MAPPING:
            return "MAPPING";
        case NIL:
            return "NULL";
        case BOOLEAN:
            return "BOOLEAN";
        case INTEGER:
            return "INTEGER";
        case DOUBLE:
            return "DOUBLE";
        case STRING:
            return "STRING";
        default: // LCOV_EXCL_LINE
        }
    }
#ifdef __cplusplus
}
#endif // __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Definations
//-----------------------------------------------------------------------------

#endif //__cplusplus

#pragma endregion

#pragma region Myyaml

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // Function to parse a scalar value
    Yaml *parse_scalar(const char **str)
    {
        const char *start = *str;
        while (**str && !isspace(**str) && **str != ':' && **str != '-')
            (*str)++;
        size_t length = *str - start;
        char *scalar_value = (char *)malloc(length + 1);
        strncpy(scalar_value, start, length);
        scalar_value[length] = '\0';
        return yaml_create_scalar(scalar_value);
    }

    // Function to parse a sequence
    Yaml *parse_sequence(const char **str)
    {
        Yaml *sequence = yaml_create_sequence();
        while (**str)
        {
            *str = skip_whitespace(*str);
            if (**str != '-')
                break;
            (*str)++;
            *str = skip_whitespace(*str);
            Yaml *item = parse_yaml(str);
            yaml_sequence_add(sequence, item);
        }
        return sequence;
    }

    // Function to parse a mapping
    Yaml *parse_mapping(const char **str)
    {
        Yaml *mapping = yaml_create_mapping();
        while (**str)
        {
            *str = skip_whitespace(*str);
            if (**str == '\0' || **str == '-')
                break;
            const char *key_start = *str;
            while (**str && **str != ':')
                (*str)++;
            size_t key_length = *str - key_start;
            char *key = (char *)malloc(key_length + 1);
            strncpy(key, key_start, key_length);
            key[key_length] = '\0';
            (*str)++;
            *str = skip_whitespace(*str);
            Yaml *value = parse_yaml(str);
            yaml_mapping_add(mapping, key, value);
        }
        return mapping;
    }

    // Function to parse a YAML value
    Yaml *parse_yaml(const char **str)
    {
        *str = skip_whitespace(*str);
        if (**str == '-')
            return parse_sequence(str);
        if (strchr(*str, ':'))
            return parse_mapping(str);
        return parse_scalar(str);
    }

    // Function to read a YAML file and convert it to a Yaml structure
    Yaml *read_yaml_file(const char *filename)
    {
        char *content = read_file(filename);
        if (!content)
            return NULL;
        const char *str = content;
        Yaml *root = parse_yaml(&str);
        free(content);
        return root;
    }

    // Function to create a YAML scalar value
    Yaml *yaml_create_scalar(const char *scalar_value)
    {
        Yaml *value = (Yaml *)malloc(sizeof(Yaml));
        value->type = SCALAR;
        value->data.scalar_value = strdup(scalar_value);
        return value;
    }

    // Function to create a YAML sequence value
    Yaml *yaml_create_sequence()
    {
        Yaml *value = (Yaml *)malloc(sizeof(Yaml));
        value->type = SEQUENCE;
        value->data.sequence_value.items = NULL;
        value->data.sequence_value.size = 0;
        return value;
    }

    // Function to add an item to a YAML sequence
    void yaml_sequence_add(Yaml *sequence, Yaml *item)
    {
        if (sequence->type != SEQUENCE)
            return;
        sequence->data.sequence_value.items = (Yaml **)realloc(sequence->data.sequence_value.items, (sequence->data.sequence_value.size + 1) * sizeof(Yaml *));
        sequence->data.sequence_value.items[sequence->data.sequence_value.size] = item;
        sequence->data.sequence_value.size++;
    }

    // Function to create a YAML mapping value
    Yaml *yaml_create_mapping()
    {
        Yaml *value = (Yaml *)malloc(sizeof(Yaml));
        value->type = MAPPING;
        value->data.mapping_value.items = NULL;
        value->data.mapping_value.size = 0;
        return value;
    }

    // Function to add a key-value pair to a YAML mapping
    void yaml_mapping_add(Yaml *mapping, const char *key, Yaml *value)
    {
        if (mapping->type != MAPPING)
            return;
        YamlKeyValuePair *pair = (YamlKeyValuePair *)malloc(sizeof(YamlKeyValuePair));
        pair->key = strdup(key);
        pair->value = value;
        mapping->data.mapping_value.items = (YamlKeyValuePair **)realloc(mapping->data.mapping_value.items, (mapping->data.mapping_value.size + 1) * sizeof(YamlKeyValuePair *));
        mapping->data.mapping_value.items[mapping->data.mapping_value.size] = pair;
        mapping->data.mapping_value.size++;
    }

    // Function to print a YAML value (for demonstration purposes)
    void yaml_print(Yaml *value, int indent)
    {
        for (int i = 0; i < indent; i++)
            printf("  ");
        switch (value->type)
        {
        case SCALAR:
            printf("%s\n", value->data.scalar_value);
            break;
        case SEQUENCE:
            printf("-\n");
            for (size_t i = 0; i < value->data.sequence_value.size; i++)
            {
                yaml_print(value->data.sequence_value.items[i], indent + 1);
            }
            break;
        case MAPPING:
            for (size_t i = 0; i < value->data.mapping_value.size; i++)
            {
                for (int j = 0; j < indent; j++)
                    printf("  ");
                printf("%s:\n", value->data.mapping_value.items[i]->key);
                yaml_print(value->data.mapping_value.items[i]->value, indent + 1);
            }
            break;
        }
    }

    // Function to pretty print a YAML value
    void yaml_pretty_print(Yaml *value, int indent)
    {
        for (int i = 0; i < indent; i++)
            printf("  ");
        switch (value->type)
        {
        case SCALAR:
            printf("%s\n", value->data.scalar_value);
            break;
        case SEQUENCE:
            for (size_t i = 0; i < value->data.sequence_value.size; i++)
            {
                for (int j = 0; j < indent; j++)
                    printf("  ");
                printf("- ");
                yaml_pretty_print(value->data.sequence_value.items[i], indent + 1);
            }
            break;
        case MAPPING:
            for (size_t i = 0; i < value->data.mapping_value.size; i++)
            {
                for (int j = 0; j < indent; j++)
                    printf("  ");
                printf("%s: ", value->data.mapping_value.items[i]->key);
                yaml_pretty_print(value->data.mapping_value.items[i]->value, indent + 1);
            }
            break;
        }
    }

    // Function to free a YAML value
    void yaml_free(Yaml *value)
    {
        if (!value)
            return;
        switch (value->type)
        {
        case SCALAR:
            free(value->data.scalar_value);
            break;
        case SEQUENCE:
            for (size_t i = 0; i < value->data.sequence_value.size; i++)
            {
                yaml_free(value->data.sequence_value.items[i]);
            }
            free(value->data.sequence_value.items);
            break;
        case MAPPING:
            for (size_t i = 0; i < value->data.mapping_value.size; i++)
            {
                free(value->data.mapping_value.items[i]->key);
                yaml_free(value->data.mapping_value.items[i]->value);
                free(value->data.mapping_value.items[i]);
            }
            free(value->data.mapping_value.items);
            break;
        default:
            break;
        }
        free(value);
    }

#ifdef __cplusplus
}
#endif // __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif //__cplusplus

#pragma endregion
