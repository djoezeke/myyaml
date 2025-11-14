#define FRUIT_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../myyaml.h"
#include "fruit.h"

int main(int argc, char *argv[]) {
  YamlEmitter emitter;
  YamlEvent event;
  Fruit_t *fruits = NULL;
  Variety_t *varieties = NULL;
  FILE *file = fopen("file.yml", "wb");

  /* Create our list of lists. */
  varieties = NULL;
  add_variety(&varieties, "macintosh", "red", false);
  add_variety(&varieties, "granny smith", "green", false);
  add_variety(&varieties, "red delicious", "red", false);
  add_fruit(&fruits, "apple", "red", 12, varieties);

  varieties = NULL;
  add_variety(&varieties, "naval", "orange", false);
  add_variety(&varieties, "clementine", "orange", true);
  add_variety(&varieties, "valencia", "orange", false);
  add_fruit(&fruits, "orange", "orange", 3, varieties);

  varieties = NULL;
  add_variety(&varieties, "cavendish", "yellow", true);
  add_variety(&varieties, "plantain", "green", true);
  add_fruit(&fruits, "bannana", "yellow", 4, varieties);

  varieties = NULL;
  add_variety(&varieties, "honey", "yellow", false);
  add_fruit(&fruits, "mango", "green", 1, varieties);

  /* Emit list of lists as yaml. */
  yaml_emitter_initialize(&emitter);
  yaml_emitter_set_output_file(&emitter, file);

  yaml_event_initialize_stream_start(&event, YAML_UTF8_ENCODING);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_document_start(&event, NULL, NULL, NULL, 0);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_mapping_start(&event, NULL, (YamlChar_t *)YAML_MAP_TAG,
                                      1, YAML_ANY_MAPPING_STYLE);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                               (YamlChar_t *)"Fruit_t", strlen("Fruit_t"), 1, 0,
                               YAML_PLAIN_SCALAR_STYLE);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_sequence_start(&event, NULL, (YamlChar_t *)YAML_SEQ_TAG,
                                       1, YAML_ANY_SEQUENCE_STYLE);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  for (struct Fruit_t *f = fruits; f; f = f->next) {
    char buffer[80];

    yaml_event_initialize_mapping_start(
        &event, NULL, (YamlChar_t *)YAML_MAP_TAG, 1, YAML_ANY_MAPPING_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                 (YamlChar_t *)"name", strlen("name"), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                 (YamlChar_t *)f->name, strlen(f->name), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                 (YamlChar_t *)"color", strlen("color"), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                 (YamlChar_t *)f->color, strlen(f->color), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                 (YamlChar_t *)"count", strlen("count"), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    if (snprintf(buffer, sizeof(buffer), "%d", f->count) >= sizeof(buffer)) {
      bail("buffer truncation");
    }
    yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_INT_TAG,
                                 (YamlChar_t *)buffer, strlen(buffer), 1, 0,
                                 YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    if (f->varieties) {
      yaml_event_initialize_scalar(
          &event, NULL, (YamlChar_t *)YAML_STR_TAG, (YamlChar_t *)"varieties",
          strlen("varieties"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
      if (!yaml_emitter_emit(&emitter, &event)) goto error;

      yaml_event_initialize_sequence_start(
          &event, NULL, (YamlChar_t *)YAML_SEQ_TAG, 1, YAML_ANY_SEQUENCE_STYLE);
      if (!yaml_emitter_emit(&emitter, &event)) goto error;

      for (struct Variety_t *v = f->varieties; v; v = v->next) {
        yaml_event_initialize_mapping_start(&event, NULL,
                                            (YamlChar_t *)YAML_MAP_TAG, 1,
                                            YAML_ANY_MAPPING_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                     (YamlChar_t *)"name", strlen("name"), 1, 0,
                                     YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                     (YamlChar_t *)v->name, strlen(v->name), 1,
                                     0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                     (YamlChar_t *)"color", strlen("color"), 1,
                                     0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(&event, NULL, (YamlChar_t *)YAML_STR_TAG,
                                     (YamlChar_t *)v->color, strlen(v->color),
                                     1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(
            &event, NULL, (YamlChar_t *)YAML_STR_TAG, (YamlChar_t *)"seedless",
            strlen("seedless"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_scalar(
            &event, NULL, (YamlChar_t *)YAML_INT_TAG,
            (YamlChar_t *)(v->seedless ? "true" : "false"),
            strlen(v->seedless ? "true" : "false"), 1, 0,
            YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_event_initialize_mapping_end(&event);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;
      }
      yaml_event_initialize_sequence_end(&event);
      if (!yaml_emitter_emit(&emitter, &event)) goto error;
    }

    yaml_event_initialize_mapping_end(&event);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;
  }

  yaml_event_initialize_sequence_end(&event);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_mapping_end(&event);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_document_end(&event, 0);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_event_initialize_stream_end(&event);
  if (!yaml_emitter_emit(&emitter, &event)) goto error;

  yaml_emitter_delete(&emitter);
  destroy_fruits(&fruits);
  return EXIT_SUCCESS;

error:
  fprintf(stderr, "Failed to emit event %d: %s\n", event.type, emitter.problem);
  yaml_emitter_delete(&emitter);
  destroy_fruits(&fruits);
  return EXIT_FAILURE;
}
