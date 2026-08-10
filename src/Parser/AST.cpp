#include "Parser/AST.hpp"

#include <algorithm>
#include <cstdint>
#include <new>
#include <string>
#include <vector>

#include "Parser/Parser.hpp"

using namespace ast;

namespace {

struct StatementAllocations {
  std::vector<ast::Statement *> objects;
  void *pending = nullptr;
  std::size_t pendingSize = 0;
  unsigned int activeScopes = 0;
};

thread_local StatementAllocations allocations;

struct TypeAllocations {
  std::vector<ast::Type *> objects;
  unsigned int activeScopes = 0;
};

thread_local TypeAllocations typeAllocations;

void trackStatement(ast::Statement *statement) {
  if (allocations.pending == nullptr)
    return;
  const auto address = reinterpret_cast<std::uintptr_t>(statement);
  const auto begin = reinterpret_cast<std::uintptr_t>(allocations.pending);
  if (address >= begin && address < begin + allocations.pendingSize) {
    statement->allocationIndex =
        static_cast<std::uint32_t>(allocations.objects.size());
    allocations.objects.push_back(statement);
  }
  allocations.pending = nullptr;
  allocations.pendingSize = 0;
}

void untrackStatement(ast::Statement *statement) {
  if (statement->allocationIndex == ast::Statement::UntrackedAllocation)
    return;
  allocations.objects[statement->allocationIndex] = nullptr;
  statement->allocationIndex = ast::Statement::UntrackedAllocation;
}

void untrackType(ast::Type *type) {
  if (typeAllocations.objects.empty())
    return;
  if (typeAllocations.objects.back() == type) {
    typeAllocations.objects.pop_back();
    return;
  }
  auto found = std::find(typeAllocations.objects.begin(),
                         typeAllocations.objects.end(), type);
  if (found != typeAllocations.objects.end())
    typeAllocations.objects.erase(found);
}

} // namespace

ast::StatementAllocationScope::StatementAllocationScope()
    : checkpoint(allocations.objects.size()) {
  ++allocations.activeScopes;
}

ast::StatementAllocationScope::~StatementAllocationScope() {
  while (allocations.objects.size() > checkpoint) {
    auto *statement = allocations.objects.back();
    if (statement == nullptr)
      allocations.objects.pop_back();
    else
      delete statement;
  }
  --allocations.activeScopes;
}

void *ast::Statement::operator new(std::size_t size) {
  void *allocation = ::operator new(size);
  if (allocations.activeScopes > 0) {
    allocations.pending = allocation;
    allocations.pendingSize = size;
  }
  return allocation;
}

void ast::Statement::operator delete(void *ptr) noexcept {
  ::operator delete(ptr);
}

void ast::Statement::operator delete(void *ptr, std::size_t) noexcept {
  ::operator delete(ptr);
}

ast::Statement::Statement() { trackStatement(this); }

ast::Statement::Statement(const Statement &other)
    : when(other.when), sourceLocation(other.sourceLocation),
      locked(other.locked), logicalLine(other.logicalLine) {
  trackStatement(this);
}

ast::Statement &ast::Statement::operator=(const Statement &other) {
  if (this == &other)
    return *this;
  when = other.when;
  sourceLocation = other.sourceLocation;
  locked = other.locked;
  coveragePoint = other.coveragePoint;
  coverageLine = other.coverageLine;
  logicalLine = other.logicalLine;
  return *this;
}

ast::Statement::~Statement() { untrackStatement(this); }

ast::TypeAllocationScope::TypeAllocationScope()
    : checkpoint(typeAllocations.objects.size()) {
  ++typeAllocations.activeScopes;
}

ast::TypeAllocationScope::~TypeAllocationScope() {
  while (typeAllocations.objects.size() > checkpoint)
    delete typeAllocations.objects.back();
  --typeAllocations.activeScopes;
}

void *ast::Type::operator new(std::size_t size) {
  void *allocation = ::operator new(size);
  if (typeAllocations.activeScopes > 0) {
    typeAllocations.objects.push_back(static_cast<ast::Type *>(allocation));
  }
  return allocation;
}

void ast::Type::operator delete(void *ptr) noexcept {
  untrackType(static_cast<ast::Type *>(ptr));
  ::operator delete(ptr);
}

void ast::Type::operator delete(void *ptr, std::size_t) noexcept {
  untrackType(static_cast<ast::Type *>(ptr));
  ::operator delete(ptr);
}

ast::Type::Type() = default;

ast::Type::Type(const Type &other) { *this = other; }

ast::Type::Type(Type &&other) noexcept { *this = std::move(other); }

ast::Type::Type(const std::string &typeName, const asmc::Size &size)
    : typeName(typeName), size(size) {}

ast::Type::~Type() { untrackType(this); }

bool Type::compare(const ast::Type &t, const std::string &name) {
  if (t.typeName == name)
    return true;
  return false;
}
