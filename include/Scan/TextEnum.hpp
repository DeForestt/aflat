#pragma once
#include <cstddef> // size_t
#include <optional>
#include <string_view>

// Helper mappers (defined at top-level, NOT inside another macro)
#define _ENUMTEXT_ONLY_NAME(Name, Text) Name,
#define _ENUMTEXT_ONLY_STR(Name, Text) std::string_view(Text),

// DEFINE_ENUM_WITH_CONVERSIONS:
// - STRUCT: enclosing struct name (e.g., Keyword)
// - ENUM:   enum class name (e.g., Type)
// - LIST:   X-macro list: calls like LIST(M) where M(Name, "text")
// - PREFIX: symbol prefix for generated functions/arrays
//
// Generates:
//   struct STRUCT { enum class ENUM { ... } type; };
//   inline constexpr std::string_view PREFIX##_strings[] = { ... };
//   constexpr std::string_view PREFIX##_to_string(STRUCT::ENUM);
//   inline std::optional<STRUCT::ENUM> PREFIX##_from_string(std::string_view);
#define DEFINE_ENUM_WITH_CONVERSIONS(STRUCT, ENUM, LIST, PREFIX)               \
  struct STRUCT {                                                              \
    enum class ENUM { LIST(_ENUMTEXT_ONLY_NAME) } type;                        \
  };                                                                           \
  inline constexpr std::string_view PREFIX##_strings[] = {                     \
      LIST(_ENUMTEXT_ONLY_STR)};                                               \
  constexpr std::string_view PREFIX##_to_string(STRUCT::ENUM v) noexcept {     \
    return PREFIX##_strings[static_cast<std::size_t>(v)];                      \
  }                                                                            \
  inline std::optional<STRUCT::ENUM> PREFIX##_from_string(                     \
      std::string_view s) noexcept {                                           \
    for (std::size_t i = 0;                                                    \
         i < sizeof(PREFIX##_strings) / sizeof(PREFIX##_strings[0]); ++i) {    \
      if (PREFIX##_strings[i] == s)                                            \
        return static_cast<STRUCT::ENUM>(i);                                   \
    }                                                                          \
    return std::nullopt;                                                       \
  }
