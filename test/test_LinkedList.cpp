#include "LinkedList.hpp"
#include "catch.hpp"

TEST_CASE("LinkedList push and pop", "[linkedlist]") {
  links::LinkedList<int> list;
  list.push(1);
  list.push(2);
  REQUIRE(list.size() == 2);
  REQUIRE(list.peek() == 2);
  REQUIRE(list.pop() == 2);
  REQUIRE(list.size() == 1);
}

TEST_CASE("LinkedList invert", "[linkedlist]") {
  links::LinkedList<int> list;
  list.push(1);
  list.push(2);
  list.invert();
  REQUIRE(list.peek() == 1);
}
