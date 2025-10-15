#ifndef MYYAML_FRUIT_H
#define MYYAML_FRUIT_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Variety_t {
  struct Variety_t *next;
  bool seedless;
  char *color;
  char *name;
} Variety_t;

typedef struct Fruit_t {
  Variety_t *varieties;
  struct Fruit_t *next;
  char *color;
  char *name;
  int count;
} Fruit_t;

//-----------------------------------------------------------------------------
// [SECTION] C Functions Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

void bail(const char *msg);
void *bail_alloc(size_t size);
char *bail_strdup(const char *s);

void add_fruit(Fruit_t **fruits, char *name, char *color, int count,
               Variety_t *varieties);
void add_variety(Variety_t **Variety_t, char *name, char *color, bool seedless);

void destroy_fruits(Fruit_t **fruits);
void destroy_varieties(Variety_t **varieties);

#ifdef __cplusplus
}
#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Classes Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif  //__cplusplus

#endif  // MYYAML_FRUIT_H

#ifdef FRUIT_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// [SECTION] C Functions Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

/* Helper to bail on error. */
void bail(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

/* Helper to allocate memory or bail. */
void *bail_alloc(size_t size) {
  void *p = calloc(1, size);
  if (!p) {
    bail("out of memory");
  }
  return p;
}

/* Helper to copy a string or bail. */
char *bail_strdup(const char *s) {
  char *c = strdup(s ? s : "");
  if (!c) {
    bail("out of memory");
  }
  return c;
}

void add_fruit(Fruit_t **fruits, char *name, char *color, int count,
               Variety_t *varieties) {
  /* Create Fruit_t object. */
  Fruit_t *f = bail_alloc(sizeof(*f));
  f->name = bail_strdup(name);
  f->color = bail_strdup(color);
  f->count = count;
  f->varieties = varieties;

  /* Append to list. */
  if (!*fruits) {
    *fruits = f;
  } else {
    Fruit_t *tail = *fruits;
    while (tail->next) {
      tail = tail->next;
    }
    tail->next = f;
  }
}

void add_variety(Variety_t **varieties, char *name, char *color,
                 bool seedless) {
  /* Create Variety_t object. */
  Variety_t *v = bail_alloc(sizeof(*v));
  v->name = bail_strdup(name);
  v->color = bail_strdup(color);
  v->seedless = seedless;

  /* Append to list. */
  if (!*varieties) {
    *varieties = v;
  } else {
    struct Variety_t *tail = *varieties;
    while (tail->next) {
      tail = tail->next;
    }
    tail->next = v;
  }
}

void destroy_fruits(Fruit_t **fruits) {
  for (Fruit_t *f = *fruits; f; f = *fruits) {
    *fruits = f->next;
    free(f->name);
    free(f->color);
    destroy_varieties(&f->varieties);
    free(f);
  }
}

void destroy_varieties(Variety_t **varieties) {
  for (Variety_t *v = *varieties; v; v = *varieties) {
    *varieties = v->next;
    free(v->name);
    free(v->color);
    free(v);
  }
}

#ifdef __cplusplus
}
#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Classes Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif  //__cplusplus

#endif  // FRUIT_IMPLEMENTATION
