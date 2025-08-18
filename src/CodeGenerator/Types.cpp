#include "CodeGenerator/Types.hpp"

namespace gen {

bool Type::compare(Type *t, std::string ident) { return ident == t->Ident; }

bool Enum::compareEnum(Enum::EnumValue e, std::string ident) {
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
