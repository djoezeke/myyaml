// This tells Catch2 to provide a main()
#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

TEST_CASE("My first test", "[tag]") {
    int a = 1;
    REQUIRE(a == 1);
}