#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../myyaml.hxx"

int main(void) {
    FILE *file = fopen("../yaml/fruit.yaml", "rb");
    assert(file && "cannot open sample YAML file ../yaml/fruit.yaml");

    YamlParser parser;
    YamlDocument document;

    assert(yaml_parser_initialize(&parser));
    yaml_parser_set_input_file(&parser, file);

    /* Load one document from the file */
    assert(yaml_parser_load(&parser, &document));

    /* Root should be a mapping with key "fruit" */
    const unsigned char *path1[] = {(const unsigned char *)"fruit", (const unsigned char *)"0", (const unsigned char *)"name"};
    const YamlChar_t *name1 = yaml_document_get_value_by_path(&document, (const YamlChar_t **)path1, 3);
    assert(name1 && strcmp((const char *)name1, "apple") == 0);

    /* Deeper: first variety name => macintosh */
    const unsigned char *path2[] = {(const unsigned char *)"fruit", (const unsigned char *)"0", (const unsigned char *)"varieties",
                                    (const unsigned char *)"0", (const unsigned char *)"name"};
    const YamlChar_t *varname = yaml_document_get_value_by_path(&document, (const YamlChar_t **)path2, 5);
    assert(varname && strcmp((const char *)varname, "macintosh") == 0);

    /* Access count value as scalar */
    const unsigned char *path3[] = {(const unsigned char *)"fruit", (const unsigned char *)"0", (const unsigned char *)"count"};
    const YamlChar_t *count = yaml_document_get_value_by_path(&document, (const YamlChar_t **)path3, 3);
    assert(count && strcmp((const char *)count, "12") == 0);

    printf("load_from_file test passed\n");

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    fclose(file);

    return 0;
}

/**
 * LICENSE: Public Domain (www.unlicense.org)
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */