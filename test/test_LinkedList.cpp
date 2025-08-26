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

TEST_CASE("LinkedList copy and move", "[linkedlist]") {
  links::LinkedList<int> list;
  list.push(1);
  list.push(2);

  links::LinkedList<int> copy(list);
  REQUIRE(copy.size() == list.size());
  REQUIRE(copy.peek() == list.peek());

  links::LinkedList<int> assigned;
  assigned = list;
  REQUIRE(assigned.size() == list.size());
  REQUIRE(assigned.peek() == list.peek());

  links::LinkedList<int> moved(std::move(list));
  REQUIRE(moved.size() == 2);
  REQUIRE(list.size() == 0);

  links::LinkedList<int> moveAssigned;
  moveAssigned = std::move(moved);
  REQUIRE(moveAssigned.size() == 2);
  REQUIRE(moved.size() == 0);
}
