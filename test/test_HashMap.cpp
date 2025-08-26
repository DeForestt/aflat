#include "HashMap.hpp"
#include "catch.hpp"

TEST_CASE("HashMap insert and retrieve", "[hashmap]") {
  HashMap<int *> map;
  int a = 5;
  map.insert("a", &a);
  REQUIRE(map.size() == 1);
  REQUIRE(map.contains("a"));
  REQUIRE(*(map.get("a")) == 5);
  map.remove("a");
  REQUIRE(map.size() == 0);
  REQUIRE_FALSE(map.contains("a"));
}

TEST_CASE("HashMap copy and move", "[hashmap]") {
  HashMap<int *> map;
  int a = 1, b = 2;
  map.insert("a", &a);
  map.insert("b", &b);

  HashMap<int *> copy(map);
  REQUIRE(copy.size() == map.size());
  REQUIRE(*(copy.get("a")) == 1);
  REQUIRE(*(copy.get("b")) == 2);

  HashMap<int *> assigned;
  assigned = map;
  REQUIRE(assigned.size() == map.size());
  REQUIRE(*(assigned.get("a")) == 1);

  HashMap<int *> moved(std::move(map));
  REQUIRE(moved.size() == 2);
  REQUIRE(map.size() == 0);

  HashMap<int *> moveAssigned;
  moveAssigned = std::move(moved);
  REQUIRE(moveAssigned.size() == 2);
  REQUIRE(moved.size() == 0);
}
