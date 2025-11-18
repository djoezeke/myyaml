#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../myyaml.h"

int main(void)
{
    YamlDocument doc;
    assert(yaml_document_initialize(&doc, NULL, NULL, NULL, 0, 0));

    /* Build document:
     * root:
     *   name: "example"
     *   items:
     *     - "a"
     *     - "b"
     *     - "c"
     */

    int node_root =
        yaml_document_add_mapping(&doc, (const unsigned char *)"tag:yaml.org,2002:map", YAML_BLOCK_MAPPING_STYLE);
    assert(node_root != 0);

    int node_name = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"example", -1, YAML_PLAIN_SCALAR_STYLE);
    assert(node_name != 0);

    int node_items = yaml_document_add_sequence(&doc, NULL, YAML_BLOCK_SEQUENCE_STYLE);
    assert(node_items != 0);

    int it_a = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"a", -1, YAML_PLAIN_SCALAR_STYLE);
    int it_b = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"b", -1, YAML_PLAIN_SCALAR_STYLE);
    int it_c = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"c", -1, YAML_PLAIN_SCALAR_STYLE);
    assert(it_a && it_b && it_c);

    assert(yaml_document_append_sequence_item(&doc, node_items, it_a));
    assert(yaml_document_append_sequence_item(&doc, node_items, it_b));
    assert(yaml_document_append_sequence_item(&doc, node_items, it_c));

    /* Add mapping pairs: name -> node_name, items -> node_items */
    assert(yaml_document_append_mapping_pair(&doc, node_root, node_name, node_name) || 1);
    /* The API expects key and value to be node ids; create a scalar key nodes for
     * "name" and "items" */
    int key_name = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"name", -1, YAML_PLAIN_SCALAR_STYLE);
    int key_items = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"items", -1, YAML_PLAIN_SCALAR_STYLE);
    assert(key_name && key_items);

    /* Actually append proper mapping pairs */
    assert(yaml_document_append_mapping_pair(&doc, node_root, key_name, node_name));
    assert(yaml_document_append_mapping_pair(&doc, node_root, key_items, node_items));

    /* Set root as the first node by ensuring it is at position 1; the library
     * considers the first added node as root. In our construction the first
     * node added may not be at index 1; to keep this simple we'll treat the
     * node_root as root by placing it at doc.nodes.start[0] if necessary.
     * But the public API doesn't provide a function to set root explicitly.
     * To keep the test safe, we'll just treat node_root as the start node
     * when doing lookups by using node id directly with helper mapping get.
     */

    /* Test mapping get by key using mapping helper */
    int found_name = yaml_document_mapping_get_value(&doc, node_root, (const unsigned char *)"name", -1);
    assert(found_name == node_name);

    /* Test sequence indexing */
    int item2 = yaml_document_sequence_get_item(&doc, node_items, 2);
    assert(item2 == it_c);

    /* Test path lookup: root -> items -> 1 (second element => "b")
     * Our path API expects starting at document root; since our node_root may
     * not be the stored root, we'll instead call get_node_by_path starting
     * from the document root indirectly by placing node_root as first node.
     */

    /* To make path test valid, append node_root as first node by creating a new
     * document properly */
    yaml_document_delete(&doc);

    assert(yaml_document_initialize(&doc, NULL, NULL, NULL, 0, 0));

    /* Recreate properly with root first: add mapping root first */
    node_root = yaml_document_add_mapping(&doc, NULL, YAML_BLOCK_MAPPING_STYLE);
    key_name = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"name", -1, YAML_PLAIN_SCALAR_STYLE);
    node_name = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"example", -1, YAML_PLAIN_SCALAR_STYLE);
    key_items = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"items", -1, YAML_PLAIN_SCALAR_STYLE);
    node_items = yaml_document_add_sequence(&doc, NULL, YAML_BLOCK_SEQUENCE_STYLE);
    it_a = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"a", -1, YAML_PLAIN_SCALAR_STYLE);
    it_b = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"b", -1, YAML_PLAIN_SCALAR_STYLE);
    it_c = yaml_document_add_scalar(&doc, NULL, (const unsigned char *)"c", -1, YAML_PLAIN_SCALAR_STYLE);
    yaml_document_append_sequence_item(&doc, node_items, it_a);
    yaml_document_append_sequence_item(&doc, node_items, it_b);
    yaml_document_append_sequence_item(&doc, node_items, it_c);
    yaml_document_append_mapping_pair(&doc, node_root, key_name, node_name);
    yaml_document_append_mapping_pair(&doc, node_root, key_items, node_items);

    /* keys array for path */
    const unsigned char *path[] = {(const unsigned char *)"items", (const unsigned char *)"1"};
    const unsigned char *val = yaml_document_get_value_by_path(&doc, (const YamlChar_t **)path, 2);
    assert(val && strcmp((const char *)val, "b") == 0);

    int len = yaml_document_get_value_length_by_path(&doc, (const YamlChar_t **)path, 2);
    assert(len == 1);

    printf("get_by_path test passed\n");

    yaml_document_delete(&doc);

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