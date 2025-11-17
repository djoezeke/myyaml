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
  const unsigned char *path1[] = {(const unsigned char *)"fruit",
                                  (const unsigned char *)"0",
                                  (const unsigned char *)"name"};
  const YamlChar_t *name1 =
      yaml_document_get_value_by_path(&document, (const YamlChar_t **)path1, 3);
  assert(name1 && strcmp((const char *)name1, "apple") == 0);

  /* Deeper: first variety name => macintosh */
  const unsigned char *path2[] = {
      (const unsigned char *)"fruit", (const unsigned char *)"0",
      (const unsigned char *)"varieties", (const unsigned char *)"0",
      (const unsigned char *)"name"};
  const YamlChar_t *varname =
      yaml_document_get_value_by_path(&document, (const YamlChar_t **)path2, 5);
  assert(varname && strcmp((const char *)varname, "macintosh") == 0);

  /* Access count value as scalar */
  const unsigned char *path3[] = {(const unsigned char *)"fruit",
                                  (const unsigned char *)"0",
                                  (const unsigned char *)"count"};
  const YamlChar_t *count =
      yaml_document_get_value_by_path(&document, (const YamlChar_t **)path3, 3);
  assert(count && strcmp((const char *)count, "12") == 0);

  printf("load_from_file test passed\n");

  yaml_document_delete(&document);
  yaml_parser_delete(&parser);
  fclose(file);

  return 0;
}
