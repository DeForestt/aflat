#include "CodeGenerator/Types.hpp"

namespace gen {

bool Type::compare(Type* t, std::string ident) {
  return ident == t->Ident;
}

bool Enum::compareEnum(Enum::EnumValue e, std::string ident) {
  return e.name == ident;
}

Enum::Enum() {
  this->values.foo = Enum::compareEnum;
}

} // namespace gen
