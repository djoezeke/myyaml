// This tells Catch2 to provide a main()
#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "myyaml/myyaml.h"

TEST_CASE("version information")
{
    SECTION("MetaInfo")
    {
        YamlDocument meta;
        MyYaml_MetaInfo(&meta);

    }
}
