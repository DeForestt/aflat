#include "catch.hpp"
#include "LinkedListS.hpp"

static bool comp(int a, int b) { return a == b; }

TEST_CASE("SLinkedList basic operations", "[slinkedlist]") {
    links::SLinkedList<int,int> list;
    list.foo = comp;
    list.push(1);
    list.push(2);
    REQUIRE(list.peek() == 2);
    REQUIRE(list.pop() == 2);
    REQUIRE(list.count == 1);
    REQUIRE(*(list[1]) == 1);
}
