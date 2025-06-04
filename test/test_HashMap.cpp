#include "catch.hpp"
#include "HashMap.hpp"

TEST_CASE("HashMap insert and retrieve", "[hashmap]") {
    HashMap<int*> map;
    int a = 5;
    map.insert("a", &a);
    REQUIRE(map.size() == 1);
    REQUIRE(map.contains("a"));
    REQUIRE(*(map.get("a")) == 5);
    map.remove("a");
    REQUIRE(map.size() == 0);
    REQUIRE_FALSE(map.contains("a"));
}
