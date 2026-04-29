#include "LSP.hpp"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "ErrorReporter.hpp"
#include "Exceptions.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"

namespace {

struct JsonValue {
  using Array = std::vector<JsonValue>;
  using Object = std::map<std::string, JsonValue>;
  using Value =
      std::variant<std::nullptr_t, bool, double, std::string, Array, Object>;

  Value value;

  JsonValue() : value(nullptr) {}
  JsonValue(std::nullptr_t) : value(nullptr) {}
  JsonValue(bool v) : value(v) {}
  JsonValue(int v) : value(static_cast<double>(v)) {}
  JsonValue(double v) : value(v) {}
  JsonValue(std::string v) : value(std::move(v)) {}
  JsonValue(const char *v) : value(std::string(v)) {}
  JsonValue(Array v) : value(std::move(v)) {}
  JsonValue(Object v) : value(std::move(v)) {}
};

class JsonParser {
public:
  explicit JsonParser(const std::string &text) : text(text) {}

  JsonValue parse() {
    skipWhitespace();
    JsonValue value = parseValue();
    skipWhitespace();
    return value;
  }

private:
  const std::string &text;
  size_t pos = 0;

  void skipWhitespace() {
    while (pos < text.size() &&
           std::isspace(static_cast<unsigned char>(text[pos]))) {
      ++pos;
    }
  }

  char peek() const { return pos < text.size() ? text[pos] : '\0'; }

  char get() { return pos < text.size() ? text[pos++] : '\0'; }

  bool consume(char c) {
    if (peek() == c) {
      ++pos;
      return true;
    }
    return false;
  }

  JsonValue parseValue() {
    skipWhitespace();
    switch (peek()) {
    case '{':
      return parseObject();
    case '[':
      return parseArray();
    case '"':
      return JsonValue(parseString());
    case 't':
      expect("true");
      return JsonValue(true);
    case 'f':
      expect("false");
      return JsonValue(false);
    case 'n':
      expect("null");
      return JsonValue(nullptr);
    default:
      return JsonValue(parseNumber());
    }
  }

  void expect(const char *literal) {
    while (*literal != '\0') {
      if (get() != *literal++) {
        throw std::runtime_error("invalid JSON payload");
      }
    }
  }

  std::string parseString() {
    if (!consume('"'))
      throw std::runtime_error("invalid JSON string");
    std::string out;
    while (pos < text.size()) {
      char c = get();
      if (c == '"')
        return out;
      if (c == '\\') {
        char esc = get();
        switch (esc) {
        case '"':
          out.push_back('"');
          break;
        case '\\':
          out.push_back('\\');
          break;
        case '/':
          out.push_back('/');
          break;
        case 'b':
          out.push_back('\b');
          break;
        case 'f':
          out.push_back('\f');
          break;
        case 'n':
          out.push_back('\n');
          break;
        case 'r':
          out.push_back('\r');
          break;
        case 't':
          out.push_back('\t');
          break;
        case 'u': {
          int codePoint = parseHex4();
          appendCodePoint(out, codePoint);
          break;
        }
        default:
          throw std::runtime_error("invalid JSON escape");
        }
      } else {
        out.push_back(c);
      }
    }
    throw std::runtime_error("unterminated JSON string");
  }

  int parseHex4() {
    int value = 0;
    for (int i = 0; i < 4; ++i) {
      char c = get();
      value <<= 4;
      if (c >= '0' && c <= '9')
        value |= (c - '0');
      else if (c >= 'a' && c <= 'f')
        value |= (10 + c - 'a');
      else if (c >= 'A' && c <= 'F')
        value |= (10 + c - 'A');
      else
        throw std::runtime_error("invalid JSON unicode escape");
    }
    return value;
  }

  static void appendCodePoint(std::string &out, int codePoint) {
    if (codePoint <= 0x7F) {
      out.push_back(static_cast<char>(codePoint));
    } else if (codePoint <= 0x7FF) {
      out.push_back(static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F)));
      out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else {
      out.push_back(static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F)));
      out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
      out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }
  }

  double parseNumber() {
    size_t start = pos;
    if (peek() == '-')
      ++pos;
    while (std::isdigit(static_cast<unsigned char>(peek())))
      ++pos;
    if (peek() == '.') {
      ++pos;
      while (std::isdigit(static_cast<unsigned char>(peek())))
        ++pos;
    }
    if (peek() == 'e' || peek() == 'E') {
      ++pos;
      if (peek() == '+' || peek() == '-')
        ++pos;
      while (std::isdigit(static_cast<unsigned char>(peek())))
        ++pos;
    }
    return std::stod(text.substr(start, pos - start));
  }

  JsonValue parseArray() {
    consume('[');
    JsonValue::Array values;
    skipWhitespace();
    if (consume(']'))
      return JsonValue(std::move(values));
    while (true) {
      values.push_back(parseValue());
      skipWhitespace();
      if (consume(']'))
        return JsonValue(std::move(values));
      if (!consume(','))
        throw std::runtime_error("invalid JSON array");
      skipWhitespace();
    }
  }

  JsonValue parseObject() {
    consume('{');
    JsonValue::Object values;
    skipWhitespace();
    if (consume('}'))
      return JsonValue(std::move(values));
    while (true) {
      skipWhitespace();
      std::string key = parseString();
      skipWhitespace();
      if (!consume(':'))
        throw std::runtime_error("invalid JSON object");
      skipWhitespace();
      values.emplace(std::move(key), parseValue());
      skipWhitespace();
      if (consume('}'))
        return JsonValue(std::move(values));
      if (!consume(','))
        throw std::runtime_error("invalid JSON object");
      skipWhitespace();
    }
  }
};

std::string jsonEscape(const std::string &input) {
  std::string out;
  out.reserve(input.size() + 8);
  for (char c : input) {
    switch (c) {
    case '\\':
      out += "\\\\";
      break;
    case '"':
      out += "\\\"";
      break;
    case '\b':
      out += "\\b";
      break;
    case '\f':
      out += "\\f";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      if (static_cast<unsigned char>(c) < 0x20) {
        std::ostringstream oss;
        oss << "\\u" << std::hex << std::uppercase;
        oss.width(4);
        oss.fill('0');
        oss << static_cast<int>(static_cast<unsigned char>(c));
        out += oss.str();
      } else {
        out.push_back(c);
      }
      break;
    }
  }
  return out;
}

std::string toJson(const JsonValue &value);

std::string toJsonObject(const JsonValue::Object &object) {
  std::string out = "{";
  bool first = true;
  for (const auto &entry : object) {
    if (!first)
      out += ",";
    first = false;
    out += "\"";
    out += jsonEscape(entry.first);
    out += "\":";
    out += toJson(entry.second);
  }
  out += "}";
  return out;
}

std::string toJsonArray(const JsonValue::Array &array) {
  std::string out = "[";
  bool first = true;
  for (const auto &entry : array) {
    if (!first)
      out += ",";
    first = false;
    out += toJson(entry);
  }
  out += "]";
  return out;
}

std::string toJson(const JsonValue &value) {
  return std::visit(
      [](const auto &entry) -> std::string {
        using T = std::decay_t<decltype(entry)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
          return "null";
        } else if constexpr (std::is_same_v<T, bool>) {
          return entry ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double>) {
          std::ostringstream oss;
          oss << entry;
          return oss.str();
        } else if constexpr (std::is_same_v<T, std::string>) {
          return "\"" + jsonEscape(entry) + "\"";
        } else if constexpr (std::is_same_v<T, JsonValue::Array>) {
          return toJsonArray(entry);
        } else {
          return toJsonObject(entry);
        }
      },
      value.value);
}

const JsonValue *findMember(const JsonValue &value, const std::string &key) {
  const auto *object = std::get_if<JsonValue::Object>(&value.value);
  if (object == nullptr)
    return nullptr;
  auto it = object->find(key);
  if (it == object->end())
    return nullptr;
  return &it->second;
}

const JsonValue *findPath(const JsonValue &value,
                          std::initializer_list<std::string> path) {
  const JsonValue *current = &value;
  for (const auto &segment : path) {
    current = findMember(*current, segment);
    if (current == nullptr)
      return nullptr;
  }
  return current;
}

std::optional<std::string> asString(const JsonValue *value) {
  if (value == nullptr)
    return std::nullopt;
  if (const auto *str = std::get_if<std::string>(&value->value))
    return *str;
  return std::nullopt;
}

std::optional<std::string> asString(const JsonValue &value) {
  return asString(&value);
}

std::optional<double> asNumber(const JsonValue *value) {
  if (value == nullptr)
    return std::nullopt;
  if (const auto *number = std::get_if<double>(&value->value))
    return *number;
  return std::nullopt;
}

std::optional<int> asInt(const JsonValue *value) {
  auto number = asNumber(value);
  if (!number)
    return std::nullopt;
  return static_cast<int>(*number);
}

std::optional<bool> asBool(const JsonValue *value) {
  if (value == nullptr)
    return std::nullopt;
  if (const auto *flag = std::get_if<bool>(&value->value))
    return *flag;
  return std::nullopt;
}

std::string trimRight(std::string value) {
  while (!value.empty() &&
         (value.back() == '\r' || value.back() == '\n' ||
          std::isspace(static_cast<unsigned char>(value.back())))) {
    value.pop_back();
  }
  return value;
}

std::string readFile(const std::filesystem::path &path) {
  std::ifstream input(path);
  if (!input.is_open())
    return {};
  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

std::string getExePath() {
  char result[1024];
  auto count = readlink("/proc/self/exe", result, sizeof(result));
  if (count <= 0)
    return {};
  return std::string(result, static_cast<size_t>(count));
}

std::string libraryHeadPath() {
  auto exe = getExePath();
  if (exe.empty())
    return "./libraries/std/head/";
  auto exeDir = std::filesystem::path(exe).parent_path();
  auto root = exeDir.parent_path();
  return (root / "libraries" / "std" / "head").string() + "/";
}

std::string decodeUri(std::string uri) {
  constexpr char prefix[] = "file://";
  if (uri.rfind(prefix, 0) == 0)
    uri = uri.substr(sizeof(prefix) - 1);
  std::string out;
  out.reserve(uri.size());
  for (size_t i = 0; i < uri.size(); ++i) {
    if (uri[i] == '%' && i + 2 < uri.size()) {
      std::string hex = uri.substr(i + 1, 2);
      char *end = nullptr;
      long value = std::strtol(hex.c_str(), &end, 16);
      if (end != hex.c_str() + 2) {
        out.push_back(uri[i]);
      } else {
        out.push_back(static_cast<char>(value));
        i += 2;
      }
    } else {
      out.push_back(uri[i]);
    }
  }
  return out;
}

std::string encodeUriPath(const std::string &path) {
  return std::filesystem::path(path).lexically_normal().string();
}

std::string pathFromUri(const std::string &uri) {
  return encodeUriPath(decodeUri(uri));
}

std::string lineText(const std::string &text, int lineNumber) {
  if (lineNumber <= 0)
    return {};
  std::istringstream input(text);
  std::string line;
  for (int i = 1; i <= lineNumber && std::getline(input, line); ++i) {
  }
  return line;
}

int lineLength(const std::string &text, int lineNumber) {
  return static_cast<int>(lineText(text, lineNumber).size());
}

int lineFromOffset(const std::string &text, size_t offset) {
  int line = 1;
  for (size_t i = 0; i < offset && i < text.size(); ++i) {
    if (text[i] == '\n')
      ++line;
  }
  return line;
}

JsonValue makePosition(int line, int character) {
  return JsonValue(JsonValue::Object{
      {"line", JsonValue(std::max(0, line))},
      {"character", JsonValue(std::max(0, character))},
  });
}

JsonValue makeRange(int line, int startCharacter, int endCharacter) {
  return JsonValue(JsonValue::Object{
      {"start", makePosition(line, startCharacter)},
      {"end", makePosition(line, endCharacter)},
  });
}

JsonValue makeDiagnostic(int line, const std::string &message,
                         const std::string &source) {
  int zeroBasedLine = std::max(0, line - 1);
  int endCharacter = std::max(1, lineLength(source, line));
  return JsonValue(JsonValue::Object{
      {"range", makeRange(zeroBasedLine, 0, endCharacter)},
      {"severity", JsonValue(1)},
      {"source", JsonValue("aflat")},
      {"message", JsonValue(message)},
  });
}

void destroyTokens(links::LinkedList<lex::Token *> &tokens) {
  while (tokens.count > 0) {
    delete tokens.pop();
  }
}

std::string currentLinePrefix(const std::string &text, int line,
                              int character) {
  if (line <= 0)
    return {};
  std::istringstream input(text);
  std::string current;
  for (int i = 1; i <= line && std::getline(input, current); ++i) {
  }
  if (character < 0)
    character = 0;
  if (static_cast<size_t>(character) > current.size())
    character = static_cast<int>(current.size());
  size_t start = static_cast<size_t>(character);
  while (start > 0) {
    char c = current[start - 1];
    if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
      --start;
    } else {
      break;
    }
  }
  return current.substr(start, static_cast<size_t>(character) - start);
}

std::vector<std::string> gatherCompletions(const std::string &text) {
  static const std::unordered_set<std::string> keywords = {
      "fn",      "class",    "struct", "union",   "import",    "return",
      "if",      "else",     "for",    "while",   "break",     "continue",
      "match",   "when",     "let",    "mutable", "immutable", "public",
      "private", "static",   "export", "const",   "types",     "safe",
      "dynamic", "pedantic", "unique", "delete",  "true",      "false",
      "None",    "Some",     "new",    "as",      "under"};

  std::unordered_set<std::string> seen;
  std::vector<std::string> items;

  for (const auto &keyword : keywords) {
    if (seen.insert(keyword).second)
      items.push_back(keyword);
  }

  static const std::regex ident(R"([A-Za-z_][A-Za-z0-9_]*)");
  auto begin = std::sregex_iterator(text.begin(), text.end(), ident);
  auto end = std::sregex_iterator();
  for (auto it = begin; it != end; ++it) {
    const auto word = it->str();
    if (keywords.count(word) != 0)
      continue;
    if (seen.insert(word).second)
      items.push_back(word);
  }

  std::sort(items.begin(), items.end());
  return items;
}

std::vector<JsonValue> completionItems(const std::vector<std::string> &items,
                                       const std::string &prefix) {
  std::vector<JsonValue> out;
  for (const auto &item : items) {
    if (!prefix.empty() && item.rfind(prefix, 0) != 0)
      continue;
    JsonValue::Object value{
        {"label", JsonValue(item)},
    };
    if (std::all_of(item.begin(), item.end(), [](unsigned char c) {
          return std::islower(c) || c == '_';
        })) {
      value.emplace("kind", JsonValue(14));
    } else {
      value.emplace("kind", JsonValue(6));
    }
    out.emplace_back(std::move(value));
  }
  return out;
}

struct SemanticSymbolSets {
  std::unordered_set<std::string> types;
  std::unordered_set<std::string> classes;
  std::unordered_set<std::string> structs;
  std::unordered_set<std::string> enums;
  std::unordered_set<std::string> functions;
  std::unordered_set<std::string> variables;
  std::unordered_set<std::string> parameters;
  std::unordered_set<std::string> enumMembers;
};

std::string stripGenerics(const std::string &name) {
  auto pos = name.find('<');
  if (pos == std::string::npos)
    return name;
  return name.substr(0, pos);
}

void collectStatementSymbols(ast::Statement *stmt, SemanticSymbolSets &symbols,
                             bool inParams = false) {
  if (stmt == nullptr)
    return;

  if (auto *seq = dynamic_cast<ast::Sequence *>(stmt)) {
    collectStatementSymbols(seq->Statement1, symbols, inParams);
    collectStatementSymbols(seq->Statement2, symbols, inParams);
    return;
  }

  if (auto *func = dynamic_cast<ast::Function *>(stmt)) {
    if (!func->ident.ident.empty())
      symbols.functions.insert(func->ident.ident);
    collectStatementSymbols(func->args, symbols, true);
    collectStatementSymbols(func->statement, symbols, false);
    return;
  }

  if (auto *cls = dynamic_cast<ast::Class *>(stmt)) {
    if (!cls->ident.ident.empty())
      symbols.classes.insert(cls->ident.ident);
    collectStatementSymbols(cls->contract, symbols, inParams);
    collectStatementSymbols(cls->statement, symbols, inParams);
    return;
  }

  if (auto *strct = dynamic_cast<ast::Struct *>(stmt)) {
    if (!strct->ident.ident.empty())
      symbols.structs.insert(strct->ident.ident);
    collectStatementSymbols(strct->statement, symbols, inParams);
    return;
  }

  if (auto *uni = dynamic_cast<ast::Union *>(stmt)) {
    if (!uni->ident.ident.empty())
      symbols.classes.insert(uni->ident.ident);
    for (const auto *alias : uni->aliases) {
      if (alias != nullptr && !alias->name.empty())
        symbols.enumMembers.insert(alias->name);
    }
    collectStatementSymbols(uni->statement, symbols, inParams);
    return;
  }

  if (auto *enm = dynamic_cast<ast::Enum *>(stmt)) {
    if (!enm->Ident.empty())
      symbols.enums.insert(enm->Ident);
    for (const auto &value : enm->values) {
      symbols.enumMembers.insert(value);
    }
    return;
  }

  if (auto *decl = dynamic_cast<ast::Declare *>(stmt)) {
    if (!decl->ident.empty()) {
      if (inParams) {
        symbols.parameters.insert(decl->ident);
      } else {
        symbols.variables.insert(decl->ident);
      }
    }
    return;
  }

  if (auto *argument = dynamic_cast<ast::Argument *>(stmt)) {
    if (!argument->Ident.empty()) {
      if (inParams) {
        symbols.parameters.insert(argument->Ident);
      } else {
        symbols.variables.insert(argument->Ident);
      }
    }
    return;
  }

  if (auto *dec = dynamic_cast<ast::Dec *>(stmt)) {
    if (!dec->ident.empty())
      symbols.variables.insert(dec->ident);
    return;
  }

  if (auto *decArr = dynamic_cast<ast::DecArr *>(stmt)) {
    if (!decArr->ident.empty())
      symbols.variables.insert(decArr->ident);
    return;
  }

  if (auto *decAssign = dynamic_cast<ast::DecAssign *>(stmt)) {
    if (decAssign->declare != nullptr && !decAssign->declare->ident.empty())
      symbols.variables.insert(decAssign->declare->ident);
    return;
  }

  if (auto *decAssignArr = dynamic_cast<ast::DecAssignArr *>(stmt)) {
    if (decAssignArr->declare != nullptr &&
        !decAssignArr->declare->ident.empty()) {
      symbols.variables.insert(decAssignArr->declare->ident);
    }
    return;
  }

  if (auto *destructure = dynamic_cast<ast::Destructure *>(stmt)) {
    for (const auto &name : destructure->identifiers)
      symbols.variables.insert(name);
    return;
  }

  if (auto *fe = dynamic_cast<ast::ForEach *>(stmt)) {
    if (!fe->binding_identifier.empty())
      symbols.parameters.insert(fe->binding_identifier);
    collectStatementSymbols(fe->implementation, symbols, false);
    return;
  }

  if (auto *f = dynamic_cast<ast::For *>(stmt)) {
    collectStatementSymbols(f->declare, symbols, false);
    collectStatementSymbols(f->increment, symbols, false);
    collectStatementSymbols(f->Run, symbols, false);
    return;
  }

  if (auto *iff = dynamic_cast<ast::If *>(stmt)) {
    collectStatementSymbols(iff->statement, symbols, false);
    collectStatementSymbols(iff->elseStatement, symbols, false);
    collectStatementSymbols(iff->elseIf, symbols, false);
    return;
  }

  if (auto *wh = dynamic_cast<ast::While *>(stmt)) {
    collectStatementSymbols(wh->stmt, symbols, false);
    return;
  }

  if (auto *ret = dynamic_cast<ast::Return *>(stmt)) {
    (void)ret;
    return;
  }

  if (auto *call = dynamic_cast<ast::Call *>(stmt)) {
    if (!call->ident.empty())
      symbols.functions.insert(call->ident);
    return;
  }

  if (auto *transform = dynamic_cast<ast::Transform *>(stmt)) {
    if (!transform->ident.empty())
      symbols.functions.insert(transform->ident);
    return;
  }

  if (auto *match = dynamic_cast<ast::Match *>(stmt)) {
    for (const auto &c : match->cases) {
      if (!c.pattern.aliasName.empty())
        symbols.enumMembers.insert(c.pattern.aliasName);
      if (c.pattern.veriableName && !c.pattern.veriableName->empty())
        symbols.variables.insert(*c.pattern.veriableName);
      collectStatementSymbols(c.statement, symbols, false);
    }
    return;
  }
}

void collectTypeNames(parse::Parser &parser, SemanticSymbolSets &symbols) {
  for (const auto &type : parser.getTypeList()) {
    auto name = stripGenerics(type.typeName);
    if (!name.empty())
      symbols.types.insert(name);
  }
}

struct SemanticToken {
  int line = 0;
  int column = 0;
  int length = 1;
  int type = 0;
  int modifiers = 0;
};

struct TokenSpan {
  int line = 0;
  int column = 0;
  int length = 1;
};

struct SignatureParameter {
  std::string label;
};

struct SignatureInfo {
  std::string name;
  std::string label;
  std::string documentation;
  std::vector<SignatureParameter> parameters;
  int activeParameter = 0;
  std::string uri;
  int line = 0;
};

struct IndexedSymbol {
  std::string name;
  std::string uri;
  int line = 0;
  int column = 0;
  int length = 1;
  int kind = 13;
  std::string detail;
  std::string signature;
  bool isFunction = false;
};

struct DocumentIndex {
  std::vector<IndexedSymbol> symbols;
  std::vector<SignatureInfo> signatures;
};

constexpr int LSP_SYMBOL_CLASS = 5;
constexpr int LSP_SYMBOL_ENUM = 10;
constexpr int LSP_SYMBOL_FUNCTION = 12;
constexpr int LSP_SYMBOL_VARIABLE = 13;
constexpr int LSP_SYMBOL_ENUM_MEMBER = 22;
constexpr int LSP_SYMBOL_STRUCT = 23;

std::vector<lex::Token *>
tokenVector(const links::LinkedList<lex::Token *> &tokens);

TokenSpan defaultSpanForName(const std::string &name) {
  return {0, 0, std::max(1, static_cast<int>(name.size()))};
}

std::string typeName(const ast::Type &type) {
  return type.typeName.empty() ? "void" : type.typeName;
}

std::string trimSignatureWhitespace(std::string value) {
  while (!value.empty() &&
         std::isspace(static_cast<unsigned char>(value.back())))
    value.pop_back();
  size_t pos = 0;
  while (pos < value.size() &&
         std::isspace(static_cast<unsigned char>(value[pos]))) {
    ++pos;
  }
  return value.substr(pos);
}

void appendParamLabel(std::vector<SignatureParameter> &params,
                      const std::string &label) {
  if (!label.empty())
    params.push_back(SignatureParameter{label});
}

void collectSignatureParameters(ast::Statement *stmt,
                                std::vector<SignatureParameter> &params);

std::string makeFunctionLabel(const ast::Function *func,
                              std::vector<SignatureParameter> &params) {
  collectSignatureParameters(func ? func->args : nullptr, params);

  std::string label = "fn ";
  label += func ? func->ident.ident : "";
  label += "(";
  for (size_t i = 0; i < params.size(); ++i) {
    if (i != 0)
      label += ", ";
    label += params[i].label;
  }
  label += ")";
  auto ret = func ? typeName(func->type) : std::string();
  if (!ret.empty() && ret != "void") {
    label += " -> ";
    label += ret;
  }
  return label;
}

void addSymbol(DocumentIndex &index, const std::string &name,
               const std::string &uri, const TokenSpan &span, int kind,
               const std::string &detail = "",
               const std::string &signature = "", bool isFunction = false) {
  index.symbols.push_back(IndexedSymbol{name, uri, span.line, span.column,
                                        span.length, kind, detail, signature,
                                        isFunction});
}

void addSignature(DocumentIndex &index, const SignatureInfo &signature) {
  index.signatures.push_back(signature);
}

std::string statementLabel(ast::Statement *stmt) {
  if (stmt == nullptr)
    return {};
  if (auto *decl = dynamic_cast<ast::Declare *>(stmt)) {
    std::string label = decl->ident;
    std::string typ =
        decl->TypeName.empty() ? typeName(decl->type) : decl->TypeName;
    if (!typ.empty())
      label += ": " + typ;
    return label;
  }
  if (auto *arg = dynamic_cast<ast::Argument *>(stmt)) {
    std::string label = arg->Ident;
    std::string typ = typeName(arg->type);
    if (!typ.empty())
      label += ": " + typ;
    return label;
  }
  if (auto *seq = dynamic_cast<ast::Sequence *>(stmt))
    return statementLabel(seq->Statement1) + ", " +
           statementLabel(seq->Statement2);
  return stmt->toString();
}

void collectSignatureParameters(ast::Statement *stmt,
                                std::vector<SignatureParameter> &params) {
  if (stmt == nullptr)
    return;
  if (auto *seq = dynamic_cast<ast::Sequence *>(stmt)) {
    collectSignatureParameters(seq->Statement1, params);
    collectSignatureParameters(seq->Statement2, params);
    return;
  }
  if (auto *decl = dynamic_cast<ast::Declare *>(stmt)) {
    appendParamLabel(params, statementLabel(decl));
    return;
  }
  if (auto *arg = dynamic_cast<ast::Argument *>(stmt)) {
    appendParamLabel(params, statementLabel(arg));
    return;
  }
}

void collectIndexFromStatement(ast::Statement *stmt, const std::string &uri,
                               DocumentIndex &index) {
  if (stmt == nullptr)
    return;

  if (auto *seq = dynamic_cast<ast::Sequence *>(stmt)) {
    collectIndexFromStatement(seq->Statement1, uri, index);
    collectIndexFromStatement(seq->Statement2, uri, index);
    return;
  }

  if (auto *func = dynamic_cast<ast::Function *>(stmt)) {
    std::vector<SignatureParameter> params;
    SignatureInfo signature;
    signature.name = func->ident.ident;
    signature.label = makeFunctionLabel(func, params);
    signature.parameters = params;
    signature.documentation = typeName(func->type);
    signature.uri = uri;
    signature.line = std::max(0, func->logicalLine - 1);
    addSignature(index, signature);
    addSymbol(
        index, func->ident.ident, uri,
        TokenSpan{std::max(0, func->logicalLine - 1), 0,
                  std::max(1, static_cast<int>(func->ident.ident.size()))},
        LSP_SYMBOL_FUNCTION, signature.documentation, signature.label, true);
    collectIndexFromStatement(func->args, uri, index);
    collectIndexFromStatement(func->statement, uri, index);
    return;
  }

  if (auto *cls = dynamic_cast<ast::Class *>(stmt)) {
    addSymbol(index, cls->ident.ident, uri,
              TokenSpan{std::max(0, cls->logicalLine - 1), 0,
                        std::max(1, static_cast<int>(cls->ident.ident.size()))},
              LSP_SYMBOL_CLASS);
    collectIndexFromStatement(cls->contract, uri, index);
    collectIndexFromStatement(cls->statement, uri, index);
    return;
  }

  if (auto *strct = dynamic_cast<ast::Struct *>(stmt)) {
    addSymbol(
        index, strct->ident.ident, uri,
        TokenSpan{std::max(0, strct->logicalLine - 1), 0,
                  std::max(1, static_cast<int>(strct->ident.ident.size()))},
        LSP_SYMBOL_STRUCT);
    collectIndexFromStatement(strct->statement, uri, index);
    return;
  }

  if (auto *uni = dynamic_cast<ast::Union *>(stmt)) {
    addSymbol(index, uni->ident.ident, uri,
              TokenSpan{std::max(0, uni->logicalLine - 1), 0,
                        std::max(1, static_cast<int>(uni->ident.ident.size()))},
              LSP_SYMBOL_CLASS);
    collectIndexFromStatement(uni->statement, uri, index);
    return;
  }

  if (auto *enm = dynamic_cast<ast::Enum *>(stmt)) {
    addSymbol(index, enm->Ident, uri,
              TokenSpan{std::max(0, enm->logicalLine - 1), 0,
                        std::max(1, static_cast<int>(enm->Ident.size()))},
              LSP_SYMBOL_ENUM);
    for (const auto &value : enm->values) {
      addSymbol(index, value, uri,
                TokenSpan{std::max(0, enm->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(value.size()))},
                LSP_SYMBOL_ENUM_MEMBER);
    }
    return;
  }

  if (auto *decl = dynamic_cast<ast::Declare *>(stmt)) {
    if (!decl->ident.empty()) {
      addSymbol(index, decl->ident, uri,
                TokenSpan{std::max(0, decl->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(decl->ident.size()))},
                LSP_SYMBOL_VARIABLE,
                decl->TypeName.empty() ? typeName(decl->type) : decl->TypeName);
    }
    return;
  }

  if (auto *arg = dynamic_cast<ast::Argument *>(stmt)) {
    if (!arg->Ident.empty()) {
      addSymbol(index, arg->Ident, uri,
                TokenSpan{std::max(0, arg->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(arg->Ident.size()))},
                LSP_SYMBOL_VARIABLE, typeName(arg->type));
    }
    return;
  }

  if (auto *dec = dynamic_cast<ast::Dec *>(stmt)) {
    if (!dec->ident.empty()) {
      addSymbol(index, dec->ident, uri,
                TokenSpan{std::max(0, dec->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(dec->ident.size()))},
                LSP_SYMBOL_VARIABLE);
    }
    return;
  }

  if (auto *decArr = dynamic_cast<ast::DecArr *>(stmt)) {
    if (!decArr->ident.empty()) {
      addSymbol(index, decArr->ident, uri,
                TokenSpan{std::max(0, decArr->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(decArr->ident.size()))},
                LSP_SYMBOL_VARIABLE);
    }
    return;
  }

  if (auto *decAssign = dynamic_cast<ast::DecAssign *>(stmt)) {
    if (decAssign->declare != nullptr && !decAssign->declare->ident.empty()) {
      addSymbol(index, decAssign->declare->ident, uri,
                TokenSpan{std::max(0, decAssign->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(
                                          decAssign->declare->ident.size()))},
                LSP_SYMBOL_VARIABLE, decAssign->declare->TypeName);
    }
    collectIndexFromStatement(decAssign->expr, uri, index);
    return;
  }

  if (auto *decAssignArr = dynamic_cast<ast::DecAssignArr *>(stmt)) {
    if (decAssignArr->declare != nullptr &&
        !decAssignArr->declare->ident.empty()) {
      addSymbol(
          index, decAssignArr->declare->ident, uri,
          TokenSpan{std::max(0, decAssignArr->logicalLine - 1), 0,
                    std::max(1, static_cast<int>(
                                    decAssignArr->declare->ident.size()))},
          LSP_SYMBOL_VARIABLE, typeName(decAssignArr->declare->type));
    }
    collectIndexFromStatement(decAssignArr->expr, uri, index);
    return;
  }

  if (auto *destructure = dynamic_cast<ast::Destructure *>(stmt)) {
    for (const auto &name : destructure->identifiers) {
      addSymbol(index, name, uri,
                TokenSpan{std::max(0, destructure->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(name.size()))},
                LSP_SYMBOL_VARIABLE);
    }
    collectIndexFromStatement(destructure->expr, uri, index);
    return;
  }

  if (auto *fe = dynamic_cast<ast::ForEach *>(stmt)) {
    if (!fe->binding_identifier.empty()) {
      addSymbol(index, fe->binding_identifier, uri,
                TokenSpan{std::max(0, fe->logicalLine - 1), 0,
                          std::max(1, static_cast<int>(
                                          fe->binding_identifier.size()))},
                LSP_SYMBOL_VARIABLE);
    }
    collectIndexFromStatement(fe->iterator, uri, index);
    collectIndexFromStatement(fe->implementation, uri, index);
    return;
  }

  if (auto *f = dynamic_cast<ast::For *>(stmt)) {
    collectIndexFromStatement(f->declare, uri, index);
    collectIndexFromStatement(f->increment, uri, index);
    collectIndexFromStatement(f->Run, uri, index);
    return;
  }

  if (auto *iff = dynamic_cast<ast::If *>(stmt)) {
    collectIndexFromStatement(iff->expr, uri, index);
    collectIndexFromStatement(iff->statement, uri, index);
    collectIndexFromStatement(iff->elseStatement, uri, index);
    collectIndexFromStatement(iff->elseIf, uri, index);
    return;
  }

  if (auto *wh = dynamic_cast<ast::While *>(stmt)) {
    collectIndexFromStatement(wh->expr, uri, index);
    collectIndexFromStatement(wh->stmt, uri, index);
    return;
  }

  if (auto *ret = dynamic_cast<ast::Return *>(stmt)) {
    collectIndexFromStatement(ret->expr, uri, index);
    return;
  }

  if (auto *call = dynamic_cast<ast::Call *>(stmt)) {
    for (auto *arg : call->Args) {
      collectIndexFromStatement(arg, uri, index);
    }
    return;
  }

  if (auto *transform = dynamic_cast<ast::Transform *>(stmt)) {
    if (!transform->ident.empty()) {
      addSymbol(
          index, transform->ident, uri,
          TokenSpan{std::max(0, transform->logicalLine - 1), 0,
                    std::max(1, static_cast<int>(transform->ident.size()))},
          LSP_SYMBOL_FUNCTION);
    }
    return;
  }

  if (auto *match = dynamic_cast<ast::Match *>(stmt)) {
    collectIndexFromStatement(match->expr, uri, index);
    for (const auto &c : match->cases) {
      if (!c.pattern.aliasName.empty()) {
        addSymbol(index, c.pattern.aliasName, uri,
                  TokenSpan{std::max(0, match->logicalLine - 1), 0,
                            std::max(1, static_cast<int>(
                                            c.pattern.aliasName.size()))},
                  LSP_SYMBOL_VARIABLE);
      }
      if (c.pattern.veriableName && !c.pattern.veriableName->empty()) {
        addSymbol(index, *c.pattern.veriableName, uri,
                  TokenSpan{std::max(0, match->logicalLine - 1), 0,
                            std::max(1, static_cast<int>(
                                            c.pattern.veriableName->size()))},
                  LSP_SYMBOL_VARIABLE);
      }
      collectIndexFromStatement(c.statement, uri, index);
    }
    return;
  }
}

TokenSpan findTokenSpan(const std::vector<lex::Token *> &tokens, int line,
                        const std::string &name) {
  for (auto *token : tokens) {
    auto *word = dynamic_cast<lex::LObj *>(token);
    if (word == nullptr)
      continue;
    if (token->lineCount - 1 == line && word->meta == name) {
      return {line, std::max(0, token->column - 1), std::max(1, token->length)};
    }
  }
  for (auto *token : tokens) {
    auto *word = dynamic_cast<lex::LObj *>(token);
    if (word != nullptr && word->meta == name) {
      return {std::max(0, token->lineCount - 1), std::max(0, token->column - 1),
              std::max(1, token->length)};
    }
  }
  return defaultSpanForName(name);
}

void indexFileRecursive(const std::string &uri, const std::string &text,
                        const std::string &libPath,
                        std::unordered_set<std::string> &visited,
                        DocumentIndex &index) {
  auto path = pathFromUri(uri);
  if (visited.count(path) != 0)
    return;
  visited.insert(path);

  lex::Lexer scanner;
  PreProcessor pp;
  std::string currentDir = std::filesystem::path(path).parent_path().string();

  try {
    auto preprocessed = pp.PreProcess(text, libPath, currentDir);
    auto tokens = scanner.Scan(preprocessed);
    tokens.invert();
    auto tokenList = tokenVector(tokens);
    parse::Parser parser;
    auto *stmt = parser.parseStmt(tokens);
    collectIndexFromStatement(stmt, uri, index);
    for (auto &symbol : index.symbols) {
      if (symbol.uri == uri) {
        auto span = findTokenSpan(tokenList, symbol.line, symbol.name);
        symbol.column = span.column;
        symbol.length = span.length;
      }
    }
    destroyTokens(tokens);

    for (const auto &include : pp.getIncludes()) {
      std::string includeText = readFile(include);
      if (!includeText.empty()) {
        indexFileRecursive(encodeUriPath(include), includeText, libPath,
                           visited, index);
      }
    }
  } catch (...) {
    // Best effort indexing. Missing or malformed files should not block LSP.
  }
}

DocumentIndex buildDocumentIndex(const std::string &uri,
                                 const std::string &text,
                                 const std::string &libPath) {
  DocumentIndex index;
  std::unordered_set<std::string> visited;
  indexFileRecursive(uri, text, libPath, visited, index);
  return index;
}

std::optional<IndexedSymbol>
findBestSymbol(const DocumentIndex &index, const std::string &name,
               std::optional<int> kind = std::nullopt) {
  std::optional<IndexedSymbol> best;
  for (const auto &symbol : index.symbols) {
    if (symbol.name != name)
      continue;
    if (kind.has_value() && symbol.kind != kind.value())
      continue;
    if (!best.has_value() || symbol.line < best->line ||
        (symbol.line == best->line && symbol.column < best->column)) {
      best = symbol;
    }
  }
  return best;
}

std::vector<IndexedSymbol> findSymbols(const DocumentIndex &index,
                                       const std::string &name) {
  std::vector<IndexedSymbol> matches;
  for (const auto &symbol : index.symbols) {
    if (symbol.name == name)
      matches.push_back(symbol);
  }
  std::sort(matches.begin(), matches.end(),
            [](const IndexedSymbol &a, const IndexedSymbol &b) {
              if (a.uri != b.uri)
                return a.uri < b.uri;
              if (a.line != b.line)
                return a.line < b.line;
              return a.column < b.column;
            });
  return matches;
}

JsonValue makeLocation(const std::string &uri, int line, int column,
                       int length) {
  return JsonValue(JsonValue::Object{
      {"uri", JsonValue(uri)},
      {"range",
       JsonValue(JsonValue::Object{
           {"start", JsonValue(JsonValue::Object{
                         {"line", JsonValue(std::max(0, line))},
                         {"character", JsonValue(std::max(0, column))},
                     })},
           {"end", JsonValue(JsonValue::Object{
                       {"line", JsonValue(std::max(0, line))},
                       {"character",
                        JsonValue(std::max(0, column + std::max(1, length)))},
                   })},
       })},
  });
}

JsonValue makeSymbolInformation(const IndexedSymbol &symbol) {
  return JsonValue(JsonValue::Object{
      {"name", JsonValue(symbol.name)},
      {"kind", JsonValue(symbol.kind)},
      {"location",
       makeLocation(symbol.uri, symbol.line, symbol.column, symbol.length)},
  });
}

JsonValue makeSignatureHelp(const DocumentIndex &index, const std::string &name,
                            int activeParameter) {
  JsonValue::Array signatures;
  for (const auto &sig : index.signatures) {
    if (sig.name != name)
      continue;
    JsonValue::Array params;
    for (const auto &param : sig.parameters) {
      params.push_back(JsonValue(JsonValue::Object{
          {"label", JsonValue(param.label)},
      }));
    }
    signatures.push_back(JsonValue(JsonValue::Object{
        {"label", JsonValue(sig.label)},
        {"documentation", JsonValue(sig.documentation)},
        {"parameters", JsonValue(std::move(params))},
        {"activeParameter", JsonValue(std::max(0, activeParameter))},
    }));
  }

  if (signatures.empty()) {
    return JsonValue(JsonValue::Object{
        {"signatures", JsonValue(JsonValue::Array{})},
        {"activeSignature", JsonValue(0)},
        {"activeParameter", JsonValue(0)},
    });
  }

  return JsonValue(JsonValue::Object{
      {"signatures", JsonValue(std::move(signatures))},
      {"activeSignature", JsonValue(0)},
      {"activeParameter", JsonValue(std::max(0, activeParameter))},
  });
}

std::optional<size_t>
tokenIndexAtPosition(const std::vector<lex::Token *> &tokens, int line,
                     int character) {
  for (size_t i = 0; i < tokens.size(); ++i) {
    const auto *token = tokens[i];
    int tokenLine = token->lineCount - 1;
    int start = std::max(0, token->column - 1);
    int end = start + std::max(1, token->length);
    if (tokenLine == line && character >= start && character < end)
      return i;
  }
  return std::nullopt;
}

std::optional<std::string>
identifierAtPosition(const std::vector<lex::Token *> &tokens, int line,
                     int character) {
  auto index = tokenIndexAtPosition(tokens, line, character);
  if (!index.has_value())
    return std::nullopt;
  if (auto *word = dynamic_cast<lex::LObj *>(tokens[index.value()])) {
    return word->meta;
  }
  if (index.value() > 0) {
    if (auto *word = dynamic_cast<lex::LObj *>(tokens[index.value() - 1])) {
      return word->meta;
    }
  }
  return std::nullopt;
}

struct CallContext {
  std::string name;
  int activeParameter = 0;
};

std::optional<CallContext>
findCallContext(const std::vector<lex::Token *> &tokens, int line,
                int character) {
  auto cursorIndex = tokenIndexAtPosition(tokens, line, character);
  if (!cursorIndex.has_value())
    return std::nullopt;

  int depth = 0;
  std::optional<size_t> openParen;
  for (size_t i = 0; i <= cursorIndex.value() && i < tokens.size(); ++i) {
    auto *token = tokens[i];
    auto *op = dynamic_cast<lex::OpSym *>(token);
    auto *sym = dynamic_cast<lex::Symbol *>(token);
    if (op != nullptr) {
      if (op->Sym == '(') {
        openParen = i;
        ++depth;
      } else if (op->Sym == ')') {
        if (depth > 0)
          --depth;
        if (depth == 0)
          openParen.reset();
      }
    } else if (sym != nullptr) {
      if (sym->meta == "(") {
        openParen = i;
        ++depth;
      } else if (sym->meta == ")") {
        if (depth > 0)
          --depth;
        if (depth == 0)
          openParen.reset();
      }
    }
  }

  if (!openParen.has_value() || openParen.value() == 0)
    return std::nullopt;

  size_t nameIndex = openParen.value() - 1;
  while (nameIndex > 0) {
    if (dynamic_cast<lex::LObj *>(tokens[nameIndex]) != nullptr)
      break;
    auto *op = dynamic_cast<lex::OpSym *>(tokens[nameIndex]);
    auto *sym = dynamic_cast<lex::Symbol *>(tokens[nameIndex]);
    if (op == nullptr && sym == nullptr)
      break;
    if (op != nullptr && op->Sym != '.' && op->Sym != ':')
      break;
    if (sym != nullptr && sym->meta != "::")
      break;
    --nameIndex;
  }

  auto *nameToken = dynamic_cast<lex::LObj *>(tokens[nameIndex]);
  if (nameToken == nullptr)
    return std::nullopt;

  int nested = 0;
  int activeParameter = 0;
  for (size_t i = openParen.value() + 1;
       i <= cursorIndex.value() && i < tokens.size(); ++i) {
    auto *token = tokens[i];
    auto *op = dynamic_cast<lex::OpSym *>(token);
    auto *sym = dynamic_cast<lex::Symbol *>(token);
    bool isOpen =
        (op && (op->Sym == '(' || op->Sym == '[' || op->Sym == '{')) ||
        (sym && (sym->meta == "(" || sym->meta == "[" || sym->meta == "{"));
    bool isClose =
        (op && (op->Sym == ')' || op->Sym == ']' || op->Sym == '}')) ||
        (sym && (sym->meta == ")" || sym->meta == "]" || sym->meta == "}"));
    bool isComma = (op && op->Sym == ',');
    if (isOpen) {
      ++nested;
    } else if (isClose) {
      if (nested > 0)
        --nested;
    } else if (isComma && nested == 0) {
      ++activeParameter;
    }
  }

  return CallContext{nameToken->meta, activeParameter};
}

enum SemanticTokenKind {
  TK_NAMESPACE = 0,
  TK_TYPE = 1,
  TK_CLASS = 2,
  TK_ENUM = 3,
  TK_INTERFACE = 4,
  TK_STRUCT = 5,
  TK_TYPE_PARAMETER = 6,
  TK_PARAMETER = 7,
  TK_VARIABLE = 8,
  TK_PROPERTY = 9,
  TK_ENUM_MEMBER = 10,
  TK_EVENT = 11,
  TK_FUNCTION = 12,
  TK_METHOD = 13,
  TK_MACRO = 14,
  TK_KEYWORD = 15,
  TK_MODIFIER = 16,
  TK_COMMENT = 17,
  TK_STRING = 18,
  TK_NUMBER = 19,
  TK_OPERATOR = 20,
  TK_DECORATOR = 21,
};

std::vector<lex::Token *>
tokenVector(const links::LinkedList<lex::Token *> &tokens) {
  std::vector<lex::Token *> out;
  out.reserve(tokens.count);
  for (auto *token : tokens)
    out.push_back(token);
  return out;
}

bool isKeyword(const std::string &word) {
  static const std::unordered_set<std::string> keywords = {
      "fn",      "class",    "struct", "union",   "import",    "return",
      "if",      "else",     "for",    "while",   "break",     "continue",
      "match",   "when",     "let",    "mutable", "immutable", "public",
      "private", "static",   "export", "const",   "types",     "safe",
      "dynamic", "pedantic", "unique", "delete",  "new",       "as",
      "under"};
  return keywords.count(word) != 0;
}

int semanticKindForToken(const std::vector<lex::Token *> &tokens, size_t index,
                         const SemanticSymbolSets &symbols) {
  auto *token = tokens[index];
  if (dynamic_cast<lex::StringObj *>(token) ||
      dynamic_cast<lex::FStringObj *>(token) ||
      dynamic_cast<lex::CharObj *>(token)) {
    return TK_STRING;
  }
  if (dynamic_cast<lex::INT *>(token) || dynamic_cast<lex::Long *>(token) ||
      dynamic_cast<lex::FloatLit *>(token)) {
    return TK_NUMBER;
  }

  if (auto *sym = dynamic_cast<lex::Symbol *>(token)) {
    if (sym->meta == "->" || sym->meta == "==" || sym->meta == "!=" ||
        sym->meta == "<=" || sym->meta == ">=" || sym->meta == "||" ||
        sym->meta == "::" || sym->meta == "<" || sym->meta == ">") {
      return TK_OPERATOR;
    }
  }

  if (auto *op = dynamic_cast<lex::OpSym *>(token)) {
    switch (op->Sym) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '^':
    case '&':
    case '|':
    case '!':
    case '=':
    case '<':
    case '>':
    case ':':
    case '.':
      return TK_OPERATOR;
    default:
      break;
    }
  }

  auto *word = dynamic_cast<lex::LObj *>(token);
  if (word == nullptr)
    return -1;

  const std::string &text = word->meta;
  if (isKeyword(text))
    return TK_KEYWORD;
  if (symbols.parameters.count(text) != 0)
    return TK_PARAMETER;
  if (symbols.variables.count(text) != 0)
    return TK_VARIABLE;
  if (symbols.functions.count(text) != 0)
    return TK_FUNCTION;
  if (symbols.enumMembers.count(text) != 0)
    return TK_ENUM_MEMBER;
  if (symbols.structs.count(text) != 0)
    return TK_STRUCT;
  if (symbols.enums.count(text) != 0)
    return TK_ENUM;
  if (symbols.classes.count(text) != 0)
    return TK_CLASS;
  if (symbols.types.count(text) != 0)
    return TK_TYPE;

  if (index + 1 < tokens.size()) {
    if (auto *next = dynamic_cast<lex::OpSym *>(tokens[index + 1])) {
      if (next->Sym == '(')
        return TK_FUNCTION;
    } else if (auto *nextSymbol =
                   dynamic_cast<lex::Symbol *>(tokens[index + 1])) {
      if (nextSymbol->meta == "::")
        return TK_NAMESPACE;
    }
  }

  return TK_VARIABLE;
}

JsonValue makeSemanticPosition(int line, int character) {
  return JsonValue(JsonValue::Object{
      {"line", JsonValue(std::max(0, line))},
      {"character", JsonValue(std::max(0, character))},
  });
}

JsonValue buildSemanticTokens(const std::string &text,
                              const std::string &libPath,
                              const std::string &currentDir) {
  std::vector<SemanticToken> semanticTokens;
  SemanticSymbolSets symbols;
  PreProcessor pp;
  lex::Lexer scanner;
  links::LinkedList<lex::Token *> parseTokens;
  links::LinkedList<lex::Token *> highlightTokens;

  try {
    highlightTokens = scanner.Scan(text);
    highlightTokens.invert();
  } catch (const err::Exception &e) {
    destroyTokens(parseTokens);
    destroyTokens(highlightTokens);
    return JsonValue(
        JsonValue::Object{{"data", JsonValue(JsonValue::Array{})}});
  } catch (const std::exception &e) {
    destroyTokens(parseTokens);
    destroyTokens(highlightTokens);
    return JsonValue(
        JsonValue::Object{{"data", JsonValue(JsonValue::Array{})}});
  } catch (int offset) {
    destroyTokens(parseTokens);
    destroyTokens(highlightTokens);
    return JsonValue(
        JsonValue::Object{{"data", JsonValue(JsonValue::Array{})}});
  } catch (...) {
    destroyTokens(parseTokens);
    destroyTokens(highlightTokens);
    return JsonValue(
        JsonValue::Object{{"data", JsonValue(JsonValue::Array{})}});
  }

  try {
    auto preprocessed = pp.PreProcess(text, libPath, currentDir);
    parseTokens = scanner.Scan(preprocessed);
    parseTokens.invert();

    parse::Parser parser;
    auto *stmt = parser.parseStmt(parseTokens);
    collectTypeNames(parser, symbols);
    collectStatementSymbols(stmt, symbols);
  } catch (const err::Exception &e) {
  } catch (const std::exception &e) {
  } catch (int offset) {
  } catch (...) {
  }

  auto tokenList = tokenVector(highlightTokens);
  semanticTokens.reserve(tokenList.size());
  for (size_t i = 0; i < tokenList.size(); ++i) {
    int kind = semanticKindForToken(tokenList, i, symbols);
    if (kind < 0)
      continue;
    auto *token = tokenList[i];
    int line = token->lineCount - 1;
    int column = std::max(0, token->column - 1);
    int length = std::max(1, token->length);

    if (kind == TK_OPERATOR) {
      size_t j = i;
      while (j + 1 < tokenList.size()) {
        auto *current = tokenList[j];
        auto *next = tokenList[j + 1];
        if (next->lineCount != current->lineCount)
          break;
        if (next->column != current->column + current->length)
          break;
        auto *currentOp = dynamic_cast<lex::OpSym *>(current);
        auto *nextOp = dynamic_cast<lex::OpSym *>(next);
        if (currentOp == nullptr || nextOp == nullptr)
          break;
        if (currentOp->Sym != nextOp->Sym)
          break;
        if (semanticKindForToken(tokenList, j + 1, symbols) != TK_OPERATOR)
          break;
        length += std::max(1, next->length);
        ++j;
      }
      i = j;
    }

    semanticTokens.push_back({line, column, length, kind, 0});
  }

  destroyTokens(parseTokens);
  destroyTokens(highlightTokens);

  std::sort(semanticTokens.begin(), semanticTokens.end(),
            [](const SemanticToken &a, const SemanticToken &b) {
              if (a.line != b.line)
                return a.line < b.line;
              return a.column < b.column;
            });

  JsonValue::Array data;
  int prevLine = 0;
  int prevColumn = 0;
  bool first = true;
  for (const auto &tok : semanticTokens) {
    int deltaLine = first ? tok.line : tok.line - prevLine;
    int deltaStart =
        first || deltaLine != 0 ? tok.column : tok.column - prevColumn;
    data.push_back(JsonValue(deltaLine));
    data.push_back(JsonValue(deltaStart));
    data.push_back(JsonValue(tok.length));
    data.push_back(JsonValue(tok.type));
    data.push_back(JsonValue(tok.modifiers));
    prevLine = tok.line;
    prevColumn = tok.column;
    first = false;
  }

  return JsonValue(JsonValue::Object{{"data", JsonValue(std::move(data))}});
}

std::vector<JsonValue> analyzeDocument(const std::string &uri,
                                       const std::string &text,
                                       const std::string &libPath) {
  std::vector<JsonValue> diagnostics;
  std::string path = pathFromUri(uri);
  std::string currentDir = std::filesystem::path(path).parent_path().string();
  PreProcessor pp;
  lex::Lexer scanner;
  links::LinkedList<lex::Token *> tokens;
  std::string preprocessed;

  try {
    preprocessed = pp.PreProcess(text, libPath, currentDir);
    tokens = scanner.Scan(preprocessed);
    tokens.invert();
    parse::Parser parser;
    auto stmt = parser.parseStmt(tokens);
    (void)stmt;
    destroyTokens(tokens);
    return diagnostics;
  } catch (int offset) {
    destroyTokens(tokens);
    diagnostics.push_back(makeDiagnostic(lineFromOffset(text, offset),
                                         "unparsable character", text));
  } catch (const err::Exception &e) {
    destroyTokens(tokens);
    int line = error::extractLine(e.errorMsg);
    if (line < 0)
      line = 1;
    diagnostics.push_back(makeDiagnostic(line, e.errorMsg, text));
  } catch (const std::exception &e) {
    destroyTokens(tokens);
    diagnostics.push_back(
        makeDiagnostic(1, std::string("internal error: ") + e.what(), text));
  } catch (...) {
    destroyTokens(tokens);
    diagnostics.push_back(
        makeDiagnostic(1, "internal error: unknown exception", text));
  }

  return diagnostics;
}

class LanguageServer {
public:
  void run() {
    while (running) {
      auto message = readMessage();
      if (!message)
        break;
      handleRequest(message->payload);
    }
  }

private:
  struct Request {
    JsonValue payload;
    std::unordered_map<std::string, std::string> headers;
  };

  bool running = true;
  std::unordered_map<std::string, std::string> documents;
  const std::string libPath = libraryHeadPath();

  std::optional<Request> readMessage() {
    std::string header;
    Request request;
    size_t contentLength = 0;

    while (std::getline(std::cin, header)) {
      header = trimRight(header);
      if (header.empty())
        break;
      auto colon = header.find(':');
      if (colon == std::string::npos)
        continue;
      std::string name = header.substr(0, colon);
      std::string value = header.substr(colon + 1);
      while (!value.empty() &&
             std::isspace(static_cast<unsigned char>(value.front()))) {
        value.erase(value.begin());
      }
      request.headers[name] = value;
      if (name == "Content-Length")
        contentLength = static_cast<size_t>(std::stoul(value));
    }

    if (!std::cin || contentLength == 0)
      return std::nullopt;

    std::string body(contentLength, '\0');
    std::cin.read(body.data(), static_cast<std::streamsize>(contentLength));
    if (static_cast<size_t>(std::cin.gcount()) != contentLength)
      return std::nullopt;

    JsonParser parser(body);
    request.payload = parser.parse();
    return request;
  }

  void writeMessage(const JsonValue &payload) {
    std::string body = toJson(payload);
    std::cout << "Content-Length: " << body.size() << "\r\n\r\n" << body;
    std::cout.flush();
  }

  void sendResponse(const JsonValue &id, const JsonValue &result) {
    writeMessage(JsonValue(JsonValue::Object{
        {"jsonrpc", JsonValue("2.0")},
        {"id", id},
        {"result", result},
    }));
  }

  void sendError(const JsonValue &id, int code, const std::string &message) {
    writeMessage(JsonValue(JsonValue::Object{
        {"jsonrpc", JsonValue("2.0")},
        {"id", id},
        {"error",
         JsonValue(JsonValue::Object{{"code", JsonValue(code)},
                                     {"message", JsonValue(message)}})},
    }));
  }

  void sendNotification(const std::string &method, const JsonValue &params) {
    writeMessage(JsonValue(JsonValue::Object{
        {"jsonrpc", JsonValue("2.0")},
        {"method", JsonValue(method)},
        {"params", params},
    }));
  }

  std::string documentText(const std::string &uri) const {
    auto it = documents.find(uri);
    if (it != documents.end())
      return it->second;
    auto path = pathFromUri(uri);
    return readFile(path);
  }

  void publishDiagnostics(const std::string &uri, const std::string &text) {
    JsonValue::Array diagnostics;
    for (const auto &diag : analyzeDocument(uri, text, libPath)) {
      diagnostics.push_back(diag);
    }
    sendNotification("textDocument/publishDiagnostics",
                     JsonValue(JsonValue::Object{
                         {"uri", JsonValue(uri)},
                         {"diagnostics", JsonValue(std::move(diagnostics))},
                     }));
  }

  void handleDidOpen(const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    if (textDocument == nullptr)
      return;
    const auto *uri = findMember(*textDocument, "uri");
    const auto *text = findMember(*textDocument, "text");
    auto uriValue = asString(uri);
    auto textValue = asString(text);
    if (!uriValue || !textValue)
      return;
    documents[*uriValue] = *textValue;
    publishDiagnostics(*uriValue, *textValue);
  }

  void handleDidChange(const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    if (textDocument == nullptr)
      return;
    const auto *changes = findMember(params, "contentChanges");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || changes == nullptr)
      return;
    const auto *array = std::get_if<JsonValue::Array>(&changes->value);
    if (array == nullptr || array->empty())
      return;
    const auto *text = findMember(array->front(), "text");
    auto textValue = asString(text ? *text : array->front());
    if (!textValue)
      return;
    documents[*uriValue] = *textValue;
  }

  void handleDidSave(const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    if (textDocument == nullptr)
      return;
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue)
      return;
    publishDiagnostics(*uriValue, documentText(*uriValue));
  }

  void handleDidClose(const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    if (textDocument == nullptr)
      return;
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue)
      return;
    documents.erase(*uriValue);
    sendNotification("textDocument/publishDiagnostics",
                     JsonValue(JsonValue::Object{
                         {"uri", JsonValue(*uriValue)},
                         {"diagnostics", JsonValue(JsonValue::Array{})},
                     }));
  }

  void handleCompletion(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    const auto *position = findMember(params, "position");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || position == nullptr) {
      sendResponse(id, JsonValue(JsonValue::Object{
                           {"isIncomplete", JsonValue(false)},
                           {"items", JsonValue(JsonValue::Array{})},
                       }));
      return;
    }

    auto line = asInt(findMember(*position, "line")).value_or(0);
    auto character = asInt(findMember(*position, "character")).value_or(0);
    std::string text = documentText(*uriValue);
    std::string prefix = currentLinePrefix(text, line + 1, character);
    auto words = gatherCompletions(text);
    auto items = completionItems(words, prefix);
    sendResponse(id, JsonValue(JsonValue::Object{
                         {"isIncomplete", JsonValue(false)},
                         {"items", JsonValue(std::move(items))},
                     }));
  }

  void handleSemanticTokens(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    if (textDocument == nullptr) {
      sendResponse(id, JsonValue(JsonValue::Object{
                           {"data", JsonValue(JsonValue::Array{})},
                       }));
      return;
    }

    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue) {
      sendResponse(id, JsonValue(JsonValue::Object{
                           {"data", JsonValue(JsonValue::Array{})},
                       }));
      return;
    }

    std::string path = pathFromUri(*uriValue);
    std::string currentDir = std::filesystem::path(path).parent_path().string();
    auto result =
        buildSemanticTokens(documentText(*uriValue), libPath, currentDir);
    sendResponse(id, result);
  }

  void handleDefinition(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    const auto *position = findMember(params, "position");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || position == nullptr) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
      return;
    }

    auto line = asInt(findMember(*position, "line")).value_or(0);
    auto character = asInt(findMember(*position, "character")).value_or(0);
    try {
      std::string text = documentText(*uriValue);
      lex::Lexer scanner;
      auto tokens = scanner.Scan(text);
      tokens.invert();
      auto tokenList = tokenVector(tokens);
      auto index = buildDocumentIndex(*uriValue, text, libPath);

      auto name = identifierAtPosition(tokenList, line, character);
      if (!name.has_value()) {
        destroyTokens(tokens);
        sendResponse(id, JsonValue(JsonValue::Array{}));
        return;
      }

      std::optional<int> kind = std::nullopt;
      if (auto ctx = findCallContext(tokenList, line, character)) {
        kind = LSP_SYMBOL_FUNCTION;
      }

      JsonValue::Array locations;
      auto matches = findSymbols(index, *name);
      for (const auto &symbol : matches) {
        if (kind.has_value() && symbol.kind != kind.value())
          continue;
        locations.push_back(makeLocation(symbol.uri, symbol.line, symbol.column,
                                         symbol.length));
      }
      destroyTokens(tokens);
      sendResponse(id, JsonValue(std::move(locations)));
    } catch (...) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
    }
  }

  void handleHover(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    const auto *position = findMember(params, "position");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || position == nullptr) {
      sendResponse(id, JsonValue(nullptr));
      return;
    }

    auto line = asInt(findMember(*position, "line")).value_or(0);
    auto character = asInt(findMember(*position, "character")).value_or(0);
    try {
      std::string text = documentText(*uriValue);
      lex::Lexer scanner;
      auto tokens = scanner.Scan(text);
      tokens.invert();
      auto tokenList = tokenVector(tokens);
      auto index = buildDocumentIndex(*uriValue, text, libPath);

      auto name = identifierAtPosition(tokenList, line, character);
      if (!name.has_value()) {
        destroyTokens(tokens);
        sendResponse(id, JsonValue(nullptr));
        return;
      }

      auto symbol = findBestSymbol(index, *name);
      if (!symbol.has_value()) {
        destroyTokens(tokens);
        sendResponse(id, JsonValue(nullptr));
        return;
      }

      std::string contents =
          symbol->signature.empty() ? symbol->name : symbol->signature;
      if (!symbol->detail.empty()) {
        contents += "\n";
        contents += symbol->detail;
      }

      destroyTokens(tokens);
      sendResponse(
          id,
          JsonValue(JsonValue::Object{
              {"contents", JsonValue(JsonValue::Object{
                               {"kind", JsonValue("markdown")},
                               {"value", JsonValue(contents)},
                           })},
              {"range",
               JsonValue(JsonValue::Object{
                   {"start", JsonValue(JsonValue::Object{
                                 {"line", JsonValue(symbol->line)},
                                 {"character", JsonValue(symbol->column)},
                             })},
                   {"end",
                    JsonValue(JsonValue::Object{
                        {"line", JsonValue(symbol->line)},
                        {"character", JsonValue(symbol->column +
                                                std::max(1, symbol->length))},
                    })},
               })},
          }));
    } catch (...) {
      sendResponse(id, JsonValue(nullptr));
    }
  }

  void handleSignatureHelp(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    const auto *position = findMember(params, "position");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || position == nullptr) {
      sendResponse(id, JsonValue(JsonValue::Object{
                           {"signatures", JsonValue(JsonValue::Array{})},
                           {"activeSignature", JsonValue(0)},
                           {"activeParameter", JsonValue(0)},
                       }));
      return;
    }

    auto line = asInt(findMember(*position, "line")).value_or(0);
    auto character = asInt(findMember(*position, "character")).value_or(0);
    try {
      std::string text = documentText(*uriValue);
      lex::Lexer scanner;
      auto tokens = scanner.Scan(text);
      tokens.invert();
      auto tokenList = tokenVector(tokens);
      auto index = buildDocumentIndex(*uriValue, text, libPath);

      auto context = findCallContext(tokenList, line, character);
      if (!context.has_value()) {
        destroyTokens(tokens);
        sendResponse(id, JsonValue(JsonValue::Object{
                             {"signatures", JsonValue(JsonValue::Array{})},
                             {"activeSignature", JsonValue(0)},
                             {"activeParameter", JsonValue(0)},
                         }));
        return;
      }

      destroyTokens(tokens);
      sendResponse(id, makeSignatureHelp(index, context->name,
                                         context->activeParameter));
    } catch (...) {
      sendResponse(id, JsonValue(JsonValue::Object{
                           {"signatures", JsonValue(JsonValue::Array{})},
                           {"activeSignature", JsonValue(0)},
                           {"activeParameter", JsonValue(0)},
                       }));
    }
  }

  void handleReferences(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    const auto *position = findMember(params, "position");
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue || position == nullptr) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
      return;
    }

    auto line = asInt(findMember(*position, "line")).value_or(0);
    auto character = asInt(findMember(*position, "character")).value_or(0);
    try {
      std::string text = documentText(*uriValue);
      lex::Lexer scanner;
      auto tokens = scanner.Scan(text);
      tokens.invert();
      auto tokenList = tokenVector(tokens);
      auto index = buildDocumentIndex(*uriValue, text, libPath);

      auto name = identifierAtPosition(tokenList, line, character);
      if (!name.has_value()) {
        destroyTokens(tokens);
        sendResponse(id, JsonValue(JsonValue::Array{}));
        return;
      }

      JsonValue::Array locations;
      for (const auto &symbol : findSymbols(index, *name)) {
        locations.push_back(makeLocation(symbol.uri, symbol.line, symbol.column,
                                         symbol.length));
      }
      destroyTokens(tokens);
      sendResponse(id, JsonValue(std::move(locations)));
    } catch (...) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
    }
  }

  void handleDocumentSymbols(const JsonValue &id, const JsonValue &params) {
    const auto *textDocument = findPath(params, {"textDocument"});
    auto uriValue = asString(findMember(*textDocument, "uri"));
    if (!uriValue) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
      return;
    }

    try {
      std::string text = documentText(*uriValue);
      auto index = buildDocumentIndex(*uriValue, text, libPath);
      JsonValue::Array symbols;
      for (const auto &symbol : index.symbols) {
        if (symbol.uri == *uriValue) {
          symbols.push_back(makeSymbolInformation(symbol));
        }
      }
      sendResponse(id, JsonValue(std::move(symbols)));
    } catch (...) {
      sendResponse(id, JsonValue(JsonValue::Array{}));
    }
  }

  void handleRequest(const JsonValue &message) {
    const auto *method = findMember(message, "method");
    auto methodValue = asString(method);
    if (!methodValue)
      return;

    const auto *id = findMember(message, "id");
    const auto *params = findMember(message, "params");

    if (*methodValue == "initialize") {
      JsonValue::Object completionProvider{
          {"triggerCharacters", JsonValue(JsonValue::Array{
                                    JsonValue("."),
                                    JsonValue(":"),
                                    JsonValue("<"),
                                    JsonValue(">"),
                                })},
      };
      JsonValue::Object semanticTokensProvider{
          {"full", JsonValue(true)},
          {"range", JsonValue(false)},
          {"legend",
           JsonValue(JsonValue::Object{
               {"tokenTypes",
                JsonValue(JsonValue::Array{
                    JsonValue("namespace"),     JsonValue("type"),
                    JsonValue("class"),         JsonValue("enum"),
                    JsonValue("interface"),     JsonValue("struct"),
                    JsonValue("typeParameter"), JsonValue("parameter"),
                    JsonValue("variable"),      JsonValue("property"),
                    JsonValue("enumMember"),    JsonValue("event"),
                    JsonValue("function"),      JsonValue("method"),
                    JsonValue("macro"),         JsonValue("keyword"),
                    JsonValue("modifier"),      JsonValue("comment"),
                    JsonValue("string"),        JsonValue("number"),
                    JsonValue("operator"),      JsonValue("decorator"),
                })},
               {"tokenModifiers", JsonValue(JsonValue::Array{})},
           })},
      };
      JsonValue::Object capabilities{
          {"textDocumentSync", JsonValue(1)},
          {"completionProvider", JsonValue(std::move(completionProvider))},
          {"definitionProvider", JsonValue(true)},
          {"hoverProvider", JsonValue(true)},
          {"referencesProvider", JsonValue(true)},
          {"documentSymbolProvider", JsonValue(true)},
          {"signatureHelpProvider",
           JsonValue(JsonValue::Object{
               {"triggerCharacters", JsonValue(JsonValue::Array{
                                         JsonValue("("),
                                         JsonValue(","),
                                     })},
               {"retriggerCharacters", JsonValue(JsonValue::Array{
                                           JsonValue(","),
                                           JsonValue(")"),
                                       })},
           })},
          {"semanticTokensProvider",
           JsonValue(std::move(semanticTokensProvider))},
          {"documentFormattingProvider", JsonValue(false)},
      };
      sendResponse(*id,
                   JsonValue(JsonValue::Object{
                       {"capabilities", JsonValue(std::move(capabilities))}}));
      return;
    }

    if (*methodValue == "shutdown") {
      if (id != nullptr)
        sendResponse(*id, JsonValue(nullptr));
      return;
    }

    if (*methodValue == "exit") {
      running = false;
      return;
    }

    if (*methodValue == "textDocument/didOpen") {
      if (params != nullptr)
        handleDidOpen(*params);
      return;
    }

    if (*methodValue == "textDocument/didChange") {
      if (params != nullptr)
        handleDidChange(*params);
      return;
    }

    if (*methodValue == "textDocument/didSave") {
      if (params != nullptr)
        handleDidSave(*params);
      return;
    }

    if (*methodValue == "textDocument/didClose") {
      if (params != nullptr)
        handleDidClose(*params);
      return;
    }

    if (*methodValue == "textDocument/completion") {
      if (id != nullptr && params != nullptr) {
        handleCompletion(*id, *params);
      } else if (id != nullptr) {
        sendResponse(*id, JsonValue(JsonValue::Object{
                              {"isIncomplete", JsonValue(false)},
                              {"items", JsonValue(JsonValue::Array{})},
                          }));
      }
      return;
    }

    if (*methodValue == "textDocument/definition") {
      if (id != nullptr && params != nullptr)
        handleDefinition(*id, *params);
      else if (id != nullptr)
        sendResponse(*id, JsonValue(JsonValue::Array{}));
      return;
    }

    if (*methodValue == "textDocument/hover") {
      if (id != nullptr && params != nullptr)
        handleHover(*id, *params);
      else if (id != nullptr)
        sendResponse(*id, JsonValue(nullptr));
      return;
    }

    if (*methodValue == "textDocument/signatureHelp") {
      if (id != nullptr && params != nullptr)
        handleSignatureHelp(*id, *params);
      else if (id != nullptr)
        sendResponse(*id, JsonValue(JsonValue::Object{
                              {"signatures", JsonValue(JsonValue::Array{})},
                              {"activeSignature", JsonValue(0)},
                              {"activeParameter", JsonValue(0)},
                          }));
      return;
    }

    if (*methodValue == "textDocument/references") {
      if (id != nullptr && params != nullptr)
        handleReferences(*id, *params);
      else if (id != nullptr)
        sendResponse(*id, JsonValue(JsonValue::Array{}));
      return;
    }

    if (*methodValue == "textDocument/documentSymbol") {
      if (id != nullptr && params != nullptr)
        handleDocumentSymbols(*id, *params);
      else if (id != nullptr)
        sendResponse(*id, JsonValue(JsonValue::Array{}));
      return;
    }

    if (*methodValue == "textDocument/semanticTokens/full") {
      if (id != nullptr && params != nullptr) {
        handleSemanticTokens(*id, *params);
      } else if (id != nullptr) {
        sendResponse(*id, JsonValue(JsonValue::Object{
                              {"data", JsonValue(JsonValue::Array{})},
                          }));
      }
      return;
    }

    if (id != nullptr) {
      sendError(*id, -32601, "Method not found");
    }
  }
};

} // namespace

void startLanguageServer() {
  LanguageServer server;
  server.run();
}
