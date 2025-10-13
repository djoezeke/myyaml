#ifndef MYYAML_PEOPLE_H
#define MYYAML_PEOPLE_H

typedef struct Hobby_t {
  struct Hobby_t *next;
  char *name;
} Hobby_t;

typedef struct Person_t {
  Hobby_t *hobbies;
  Person_t *next;
  char *name;
  int count;
  int age;
} Person_t;

//-----------------------------------------------------------------------------
// [SECTION] C Functions Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifdef __cplusplus
}
#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Classes Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif  //__cplusplus

#endif  // MYYAML_PEOPLE_H

#ifdef PEOPLE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// [SECTION] C Functions Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

#ifdef __cplusplus
}
#endif  //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Classes Only
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif  //__cplusplus

#endif  // PEOPLE_IMPLEMENTATION
