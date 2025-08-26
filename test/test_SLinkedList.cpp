#include "LinkedListS.hpp"
#include "catch.hpp"

static bool comp(int a, int b) { return a == b; }

TEST_CASE("SLinkedList basic operations", "[slinkedlist]") {
  links::SLinkedList<int, int> list;
  list.foo = comp;
  list.push(1);
  list.push(2);
  REQUIRE(list.peek() == 2);
  REQUIRE(list.pop() == 2);
  REQUIRE(list.count == 1);
  REQUIRE(*(list[1]) == 1);
}

TEST_CASE("SLinkedList copy and move", "[slinkedlist]") {
  links::SLinkedList<int, int> list;
  list.foo = comp;
  list.push(1);
  list.push(2);

  links::SLinkedList<int, int> copy(list);
  REQUIRE(copy.count == list.count);
  REQUIRE(copy.peek() == list.peek());

  links::SLinkedList<int, int> assigned;
  assigned = list;
  REQUIRE(assigned.count == list.count);
  REQUIRE(assigned.peek() == list.peek());

  links::SLinkedList<int, int> moved(std::move(list));
  REQUIRE(moved.count == 2);
  REQUIRE(list.count == 0);

  links::SLinkedList<int, int> moveAssigned;
  moveAssigned = std::move(moved);
  REQUIRE(moveAssigned.count == 2);
  REQUIRE(moved.count == 0);
}
