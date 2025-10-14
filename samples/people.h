#ifndef MYYAML_PEOPLE_H
#define MYYAML_PEOPLE_H

#include <../myyaml.h>

typedef struct Hobby_t {
    struct Hobby_t *next;
    char *m_Name;
} Hobby_t;

typedef struct Person_t {
    Hobby_t *hobbies;
    Person_t *next;
    char *m_Name;
    int count;
    int m_Age;
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

#include <iostream>
#include <vector>

class Hobbies {
   public:
    Hobbies(std::vector<std::string> hobbies);

   private:
    std::vector<std::string> m_Hobbies;
};

struct Person {
    std::string m_Name;
    int m_Age;
};

// Provide Serializer/Deserializer specializations for Person
namespace myyaml {
template <>
class Serializer<Person> {
   public:
    static void to_yaml(myyaml::yaml &doc, const Person &p);
};

template <>
class Deserializer<Person> {
   public:
    static void from_yaml(const myyaml::yaml &doc, Person &out);
};

};  // namespace myyaml

#endif  //__cplusplus

#endif  // MYYAML_PEOPLE_H

#define PEOPLE_IMPLEMENTATION
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

namespace myyaml {

void Serializer<Person>::to_yaml(myyaml::yaml &doc, const Person &p) {
    std::map<std::string, std::string> m;
    m["Name"] = p.m_Name;
    m["Age"] = std::to_string(p.m_Age);
    Serializer<std::map<std::string, std::string>>::to_yaml(doc, m);
};

void Deserializer<Person>::from_yaml(const myyaml::yaml &doc, Person &out) {
    std::map<std::string, std::string> m;
    Deserializer<std::map<std::string, std::string>>::from_yaml(doc, m);
    out.m_Name = m["Name"];
    out.m_Age = std::stoi(m["Age"]);
};

};  // namespace myyaml

#endif  //__cplusplus

#endif  // PEOPLE_IMPLEMENTATION
