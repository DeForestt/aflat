#include "Scanner.hpp"
#include "catch.hpp"

namespace {
class TrackedToken : public lex::Token {
public:
  explicit TrackedToken(int &destructions) : destructions(destructions) {}
  ~TrackedToken() override { ++destructions; }

private:
  int &destructions;
};
} // namespace

TEST_CASE("token assignment preserves scoped ownership", "[scanner][memory]") {
  int destructions = 0;
  {
    lex::TokenAllocationScope allocations;
    auto *tracked = new TrackedToken(destructions);
    lex::Token source;
    tracked->lex::Token::operator=(source);
  }
  REQUIRE(destructions == 1);
}

TEST_CASE("Lexer scans identifiers and ints", "[scanner]") {
  lex::Lexer l;
  auto tokens = l.Scan("foo 123");
  REQUIRE(tokens.size() >= 3); // includes trailing semicolon token
  auto *first = dynamic_cast<lex::LObj *>(tokens.get(2));
  auto *second = dynamic_cast<lex::INT *>(tokens.get(1));
  REQUIRE(first != nullptr);
  REQUIRE(first->meta == "foo");
  REQUIRE(second != nullptr);
  REQUIRE(second->value == "123");
}
