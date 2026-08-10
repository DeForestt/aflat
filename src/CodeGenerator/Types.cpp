#include "CodeGenerator/Types.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct TypeAllocations {
  std::vector<gen::Type *> objects;
  void *pending = nullptr;
  std::size_t pendingSize = 0;
  unsigned int activeScopes = 0;
};

thread_local TypeAllocations typeAllocations;

void trackType(gen::Type *type) {
  if (typeAllocations.pending == nullptr)
    return;
  const auto address = reinterpret_cast<std::uintptr_t>(type);
  const auto begin = reinterpret_cast<std::uintptr_t>(typeAllocations.pending);
  if (address >= begin && address < begin + typeAllocations.pendingSize)
    typeAllocations.objects.push_back(type);
  typeAllocations.pending = nullptr;
  typeAllocations.pendingSize = 0;
}

void untrackType(gen::Type *type) {
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

namespace gen {

TypeAllocationScope::TypeAllocationScope()
    : checkpoint(typeAllocations.objects.size()) {
  ++typeAllocations.activeScopes;
}

TypeAllocationScope::~TypeAllocationScope() {
  while (typeAllocations.objects.size() > checkpoint)
    delete typeAllocations.objects.back();
  --typeAllocations.activeScopes;
}

void *Type::operator new(std::size_t size) {
  void *allocation = ::operator new(size);
  if (typeAllocations.activeScopes > 0) {
    typeAllocations.pending = allocation;
    typeAllocations.pendingSize = size;
  }
  return allocation;
}

void Type::operator delete(void *ptr) noexcept { ::operator delete(ptr); }

void Type::operator delete(void *ptr, std::size_t) noexcept {
  ::operator delete(ptr);
}

Type::Type() { trackType(this); }

Type::Type(const Type &other)
    : Ident(other.Ident), hidden(other.hidden),
      publicSymbols(other.publicSymbols), SymbolTable(other.SymbolTable),
      size(other.size), uniqueType(other.uniqueType) {
  trackType(this);
}

Type::~Type() { untrackType(this); }

bool Type::compare(Type *const &t, const std::string &ident) {
  return ident == t->Ident;
}

bool Enum::compareEnum(const Enum::EnumValue &e, const std::string &ident) {
  return e.name == ident;
}

void apply(FieldState &fs, EffectKind effect, uint32_t siteId) {
  switch (effect) {
  case EffectKind::WriteField:
  case EffectKind::RevalidateField:
    bump(fs, siteId);
    fs.status = FieldState::Status::Valid;
    return;
  case EffectKind::MoveField:
    bump(fs, siteId);
    fs.status = FieldState::Status::Moved;
    return;
  case EffectKind::InvalidateField:
    bump(fs, siteId);
    fs.status = FieldState::Status::Invalid;
    return;
  }
}

Enum::Enum() { this->values.foo = Enum::compareEnum; }

} // namespace gen
