#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "ASM.hpp"
#include "CLI.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CompilerUtils.hpp"
#include "Configs.hpp"
#include "ErrorReporter.hpp"
#include "Exceptions.hpp"
#include "LinkedList.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Progress.hpp"
#include "Scanner.hpp"

std::string preProcess(std::string input);
std::string getExePath();
void buildTemplate(std::string value);
void libTemplate(std::string value);
bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

bool compileCFile(const std::string &path, bool debug);
bool objectUpToDate(const std::string &src, const std::string &obj,
                    const std::string &libPath);
struct ModuleResult {
  std::string objPath;
  bool success;
};
struct DocEntry {
  std::string signature;
  std::vector<std::string> doc;
};

struct ClassDoc {
  std::string name;
  std::vector<std::string> doc;
  std::vector<DocEntry> methods;
};

struct UnionDoc {
  std::string name;
  std::vector<std::string> doc;
  std::vector<std::string> variants;
  std::vector<DocEntry> methods;
};

struct DocInfo {
  std::map<std::string, ClassDoc> classes;
  std::map<std::string, UnionDoc> unions;
  std::vector<DocEntry> enums;
  std::vector<DocEntry> transforms;
  std::vector<DocEntry> functions;
};

ModuleResult compileModule(const std::string &mod, const cfg::Config &config,
                           const std::string &libPath);
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);
bool runConfig(cfg::Config &config, const std::string &libPath);

static std::string trim(const std::string &value) {
  size_t start = 0;
  while (start < value.size() &&
         std::isspace(static_cast<unsigned char>(value[start]))) {
    ++start;
  }
  size_t end = value.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(value[end - 1]))) {
    --end;
  }
  return value.substr(start, end - start);
}

struct ParsedLine {
  std::string text;
  std::vector<std::string> doc;
};

static bool isNameChar(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == ':' ||
         c == '<' || c == '>' || c == '.';
}

static std::string extractIdentifier(const std::string &line, size_t start) {
  while (start < line.size() &&
         std::isspace(static_cast<unsigned char>(line[start]))) {
    ++start;
  }
  size_t end = start;
  while (end < line.size() && isNameChar(line[end])) {
    ++end;
  }
  if (end <= start)
    return "";
  return trim(line.substr(start, end - start));
}

static std::string extractClassName(const std::string &line) {
  size_t classPos = line.find("class ");
  if (classPos == std::string::npos)
    return "";
  return extractIdentifier(line, classPos + 6);
}

static std::string extractUnionName(const std::string &line) {
  size_t pos = line.find("union");
  while (pos != std::string::npos) {
    bool beforeOk =
        pos == 0 || (!std::isalnum(static_cast<unsigned char>(line[pos - 1])) &&
                     line[pos - 1] != '_');
    size_t after = pos + 5;
    bool afterOk = after >= line.size() ||
                   (!std::isalnum(static_cast<unsigned char>(line[after])) &&
                    line[after] != '_');
    if (beforeOk && afterOk) {
      return extractIdentifier(line, after);
    }
    pos = line.find("union", after);
  }
  return "";
}

static std::string extractEnumName(const std::string &line) {
  size_t pos = line.find("enum");
  while (pos != std::string::npos) {
    bool beforeOk =
        pos == 0 || (!std::isalnum(static_cast<unsigned char>(line[pos - 1])) &&
                     line[pos - 1] != '_');
    size_t after = pos + 4;
    bool afterOk = after >= line.size() ||
                   (!std::isalnum(static_cast<unsigned char>(line[after])) &&
                    line[after] != '_');
    if (beforeOk && afterOk) {
      std::string name = extractIdentifier(line, after);
      if (name == "class") {
        size_t classPos = line.find("class", after);
        if (classPos != std::string::npos)
          return extractIdentifier(line, classPos + 5);
      }
      return name;
    }
    pos = line.find("enum", after);
  }
  return "";
}

static std::string trimDocLine(const std::string &line) {
  std::string result = trim(line);
  if (!result.empty() && result[0] == '*') {
    result.erase(result.begin());
    result = trim(result);
  }
  return result;
}

static void finalizeDocBuffer(std::vector<std::string> &docs,
                              std::string &buffer) {
  std::string cleaned = trimDocLine(buffer);
  if (!cleaned.empty())
    docs.push_back(cleaned);
  buffer.clear();
}

static std::vector<ParsedLine> preprocessLines(const std::string &content) {
  std::vector<ParsedLine> lines;
  std::istringstream stream(content);
  bool inBlockComment = false;
  bool blockIsDoc = false;
  std::string docBuffer;
  std::vector<std::string> pendingDoc;
  std::string line;

  while (std::getline(stream, line)) {
    std::string codeLine;
    size_t i = 0;
    while (i < line.size()) {
      char c = line[i];
      char next = (i + 1 < line.size()) ? line[i + 1] : '\0';

      if (inBlockComment) {
        if (blockIsDoc) {
          if (c == '*' && next == '/') {
            finalizeDocBuffer(pendingDoc, docBuffer);
            inBlockComment = false;
            blockIsDoc = false;
            i += 2;
            continue;
          }
          docBuffer += c;
          ++i;
          continue;
        }

        if (c == '*' && next == '/') {
          inBlockComment = false;
          i += 2;
          continue;
        }
        ++i;
        continue;
      }

      if (c == '/' && next == '*') {
        std::string before = line.substr(0, i);
        bool atLineStart = trim(before).empty();
        bool isDoc = atLineStart;
        inBlockComment = true;
        blockIsDoc = isDoc;
        if (blockIsDoc)
          docBuffer.clear();
        size_t advance = 2;
        if (isDoc && i + 2 < line.size() && line[i + 2] == '*')
          advance = 3;
        i += advance;
        continue;
      }

      if (c == '/' && next == '/') {
        std::string before = line.substr(0, i);
        if (trim(before).empty()) {
          std::string docText = trim(line.substr(i + 2));
          if (!docText.empty())
            pendingDoc.push_back(docText);
        }
        break;
      }

      codeLine += c;
      ++i;
    }

    if (inBlockComment && blockIsDoc) {
      finalizeDocBuffer(pendingDoc, docBuffer);
    }

    std::string trimmedCode = trim(codeLine);
    if (!trimmedCode.empty()) {
      ParsedLine info;
      info.text = trimmedCode;
      info.doc = pendingDoc;
      lines.push_back(std::move(info));
      pendingDoc.clear();
    }
  }

  return lines;
}

static std::string extractSignature(const std::string &line) {
  std::string signature = trim(line);
  size_t blockPos = signature.find('{');
  size_t lambdaPos = signature.find("=>");
  if (blockPos != std::string::npos)
    signature = trim(signature.substr(0, blockPos));
  else if (lambdaPos != std::string::npos)
    signature = trim(signature.substr(0, lambdaPos));

  while (!signature.empty() &&
         (signature.back() == ';' || signature.back() == ',')) {
    signature.pop_back();
  }
  return trim(signature);
}

static void appendUnique(std::vector<DocEntry> &items,
                         const std::string &signature,
                         const std::vector<std::string> &doc) {
  if (signature.empty())
    return;
  for (auto &item : items) {
    if (item.signature == signature) {
      if (item.doc.empty() && !doc.empty())
        item.doc = doc;
      return;
    }
  }
  DocEntry entry;
  entry.signature = signature;
  entry.doc = doc;
  items.push_back(std::move(entry));
}

static void appendUniqueVariant(std::vector<std::string> &items,
                                const std::string &variant) {
  if (variant.empty())
    return;
  for (const auto &item : items) {
    if (item == variant)
      return;
  }
  items.push_back(variant);
}

static bool isLikelySignature(const std::string &line) {
  if (line.find('(') == std::string::npos ||
      line.find(')') == std::string::npos) {
    return false;
  }
  std::string lowered;
  lowered.reserve(line.size());
  for (char c : line) {
    lowered += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  const std::vector<std::string> blocked = {"if ",    "for ",   "while ",
                                            "switch", "return", "match "};
  for (const auto &word : blocked) {
    if (lowered.find(word) != std::string::npos)
      return false;
  }
  if (lowered.find("class ") != std::string::npos)
    return false;
  if (!line.empty() && line[0] == '.')
    return false;
  return true;
}

static bool containsWord(const std::string &line, const std::string &word) {
  size_t pos = line.find(word);
  while (pos != std::string::npos) {
    bool beforeOk =
        pos == 0 || (!std::isalnum(static_cast<unsigned char>(line[pos - 1])) &&
                     line[pos - 1] != '_');
    size_t end = pos + word.size();
    bool afterOk = end >= line.size() ||
                   (!std::isalnum(static_cast<unsigned char>(line[end])) &&
                    line[end] != '_');
    if (beforeOk && afterOk)
      return true;
    pos = line.find(word, end);
  }
  return false;
}

static bool beginsWithWord(const std::string &line, const std::string &word) {
  if (line.size() < word.size())
    return false;
  if (line.compare(0, word.size(), word) != 0)
    return false;
  if (line.size() == word.size())
    return true;
  char c = line[word.size()];
  return std::isspace(static_cast<unsigned char>(c));
}

static bool isDecoratorLine(const std::string &line) {
  return !line.empty() && line[0] == '@';
}

static DocInfo parseDocsFromContent(const std::string &content) {
  DocInfo info;
  std::vector<ParsedLine> lines = preprocessLines(content);

  std::string currentClass;
  std::string pendingClass;
  std::vector<std::string> pendingClassDoc;
  std::string currentUnion;
  std::string pendingUnion;
  std::vector<std::string> pendingUnionDoc;
  int braceDepth = 0;
  int classBraceDepth = -1;
  int unionBraceDepth = -1;
  bool collectingUnionVariants = false;
  std::vector<std::string> carryDoc;

  for (const auto &lineInfo : lines) {
    std::vector<std::string> docLines;
    if (!lineInfo.doc.empty()) {
      docLines = lineInfo.doc;
    } else {
      docLines = carryDoc;
    }
    bool docUsed = false;
    std::string trimmed = lineInfo.text;
    if (trimmed.empty())
      continue;
    if (currentClass.empty() && pendingClass.empty()) {
      std::string className = extractClassName(trimmed);
      if (!className.empty()) {
        pendingClass = className;
        pendingClassDoc = docLines;
        docLines.clear();
        docUsed = true;
      }
    }

    if (currentUnion.empty() && pendingUnion.empty()) {
      std::string unionName = extractUnionName(trimmed);
      if (!unionName.empty()) {
        pendingUnion = unionName;
        pendingUnionDoc = docLines;
        docLines.clear();
        docUsed = true;
      }
    }

    if (!pendingClass.empty() && trimmed.find('{') != std::string::npos) {
      currentClass = pendingClass;
      pendingClass.clear();
      classBraceDepth = braceDepth + 1;
      ClassDoc &cls = info.classes[currentClass];
      cls.name = currentClass;
      if (cls.doc.empty())
        cls.doc = pendingClassDoc;
      pendingClassDoc.clear();
    }

    if (!pendingUnion.empty() && trimmed.find('{') != std::string::npos) {
      currentUnion = pendingUnion;
      pendingUnion.clear();
      unionBraceDepth = braceDepth + 1;
      UnionDoc &uni = info.unions[currentUnion];
      uni.name = currentUnion;
      if (uni.doc.empty())
        uni.doc = pendingUnionDoc;
      pendingUnionDoc.clear();
      collectingUnionVariants = true;
    }

    bool decoratorLine = isDecoratorLine(trimmed);

    if (!currentClass.empty() && braceDepth == classBraceDepth) {
      if (!decoratorLine && isLikelySignature(trimmed)) {
        std::string signature = extractSignature(trimmed);
        appendUnique(info.classes[currentClass].methods, signature, docLines);
        docLines.clear();
        docUsed = true;
      }
    }

    if (!currentUnion.empty() && braceDepth == unionBraceDepth) {
      if (!decoratorLine && containsWord(trimmed, "fn") &&
          isLikelySignature(trimmed)) {
        collectingUnionVariants = false;
        std::string signature = extractSignature(trimmed);
        appendUnique(info.unions[currentUnion].methods, signature, docLines);
        docLines.clear();
        docUsed = true;
      } else if (collectingUnionVariants) {
        std::string variant = trim(trimmed);
        while (!variant.empty() &&
               (variant.back() == ',' || variant.back() == ';')) {
          variant.pop_back();
        }
        variant = trim(variant);
        if (!variant.empty() && variant != "{" && variant != "}") {
          appendUniqueVariant(info.unions[currentUnion].variants, variant);
          docLines.clear();
          docUsed = true;
        }
      }
    }

    if (currentClass.empty() && braceDepth == 0) {
      if (!decoratorLine && isLikelySignature(trimmed)) {
        std::string signature = extractSignature(trimmed);
        appendUnique(info.functions, signature, docLines);
        docLines.clear();
        docUsed = true;
      } else if (containsWord(trimmed, "enum") &&
                 trimmed.find('{') != std::string::npos) {
        std::string enumName = extractEnumName(trimmed);
        if (enumName.empty())
          enumName = trimmed;
        appendUnique(info.enums, enumName, docLines);
        docLines.clear();
        docUsed = true;
      } else if (beginsWithWord(trimmed, "transform")) {
        appendUnique(info.transforms, trimmed, docLines);
        docLines.clear();
        docUsed = true;
      }
    }

    bool allowCarry = decoratorLine || trimmed.rfind("types(", 0) == 0;
    if (!docUsed && allowCarry) {
      carryDoc = docLines;
    } else {
      carryDoc.clear();
    }

    for (char c : trimmed) {
      if (c == '{') {
        braceDepth++;
      } else if (c == '}') {
        braceDepth--;
      }
    }

    if (!currentClass.empty() && braceDepth < classBraceDepth) {
      currentClass.clear();
      classBraceDepth = -1;
    }

    if (!currentUnion.empty() && braceDepth < unionBraceDepth) {
      currentUnion.clear();
      unionBraceDepth = -1;
      collectingUnionVariants = false;
    }
  }

  return info;
}

static std::string toLower(std::string value) {
  for (char &c : value) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return value;
}

static std::string resolveDocsPath(const std::string &input,
                                   const std::string &libRoot) {
  if (std::filesystem::exists(input))
    return input;

  std::vector<std::string> candidates;
  candidates.push_back(libRoot + "src/" + input);
  candidates.push_back(libRoot + "src/" + input + ".af");
  candidates.push_back(libRoot + "src/" + toLower(input) + ".af");
  candidates.push_back(libRoot + "head/" + input + ".gs");
  candidates.push_back(libRoot + "head/" + toLower(input) + ".gs");

  for (const auto &candidate : candidates) {
    if (std::filesystem::exists(candidate))
      return candidate;
  }
  return "";
}

static std::string generateReadmeContent(const std::string &projectName) {
  std::string displayName = projectName.empty() ? "AFlat Project" : projectName;
  std::ostringstream out;
  out << "# " << displayName << "\n\n";
  out << "This project was generated with the AFlat toolchain. Use the "
         "commands below to build, run, test, and explore the codebase.\n\n";
  out << "## Build & Run\n";
  out << "- `aflat build` - compile sources defined in `aflat.cfg`.\n";
  out << "- `aflat run` - build and execute the resulting binary.\n";
  out << "- `aflat test` - build and run the configured test target.\n\n";
  out << "## Inspecting Code\n";
  out << "- `aflat docs src/main.af` lists classes, unions, transforms, and "
         "functions in this project.\n";
  out << "- `aflat docs String` explores a standard "
         "library module.\n\n";
  out << "### Creating modules\n";
  out << "Use `aflat module <module name>` to scaffold `src/<module "
         "name>/mod.af` "
         "and automatically register it under [dependencies]. For single "
         "files, "
         "run `aflat file <name>`.\n\n";
  out << "### Standard library overview\n";
  out << "AFlat ships a batteries-included standard library under "
         "`libraries/std`. "
         "The modules below are the most commonly used building blocks:\n\n";
  out << "#### Runtime & strings\n";
  out << "- `std` - Arena allocator, `malloc`/`free`, `memcpy`, `panic`, "
         "`sleep`, and other intrinsics (libraries/std/src/std.af).\n";
  out << "- `std-cmp` - Compatibility shim that swaps in libc memory helpers "
         "plus panic/assert when the arena can't be used "
         "(libraries/std/src/std-cmp.af).\n";
  out << "- `Memory` - RefCounted base class alongside the generic "
         "`Box`/`wrap` "
         "helpers for owning values (libraries/std/src/Memory.af).\n";
  out << "- `String` - Assembly-backed string primitive covering allocation, "
         "slicing, iteration, comparisons, casing, and conversions "
         "(libraries/std/src/String.af).\n";
  out << "- `strings` - Standalone utilities such as `len`, concat, ASCII "
         "helpers, "
         "and int/float parsing/formatting (libraries/std/src/strings.af).\n";
  out << "- `System` - Thin syscall wrappers for `execve`, `exec`, shell "
         "execution, "
         "and environment-variable helpers (libraries/std/src/System.af).\n\n";
  out << "#### Numerics & IO\n";
  out << "- `math` - Numeric helpers including `Random`, integer/float "
         "exponentiation, "
         "square roots, conversions, and absolute values "
         "(libraries/std/src/math.af).\n";
  out << "- `io` - Terminal IO functions for reading strings, printing "
         "chars/ints/hex, "
         "and emitting ANSI-colored output (libraries/std/src/io.af).\n";
  out << "- `files` - File descriptor wrapper with `FileError`/`ReadError`, "
         "buffered reads, "
         "iterators, and write helpers (libraries/std/src/files.af).\n";
  out << "- `DateTime` - Epoch-based `DateTime` struct with parsing, getters, "
         "formatting, "
         "and day-of-week math (libraries/std/src/DateTime.af).\n\n";
  out << "#### Collections & iteration\n";
  out << "- `Collections` - Generic `List` implementation with push/pop/find "
         "helpers and "
         "bounds-checked access (libraries/std/src/Collections.af).\n";
  out << "- `Collections/Vector` - Type-parametric `vector` supporting "
         "copy/move semantics, "
         "iterators, sorting, and optional accessors "
         "(libraries/std/src/Collections/Vector.af).\n";
  out << "- `Collections/unordered_map` - Hash-tree-backed associative "
         "container with `set`/`get`/`keys` "
         "and Option-aware lookups "
         "(libraries/std/src/Collections/unordered_map.af).\n";
  out << "- `Collections/Iterator` - Base iterator abstraction plus "
         "`Next`/`Peek` decorators to wire callbacks "
         "(libraries/std/src/Collections/Iterator.af).\n";
  out << "- `Collections/Enumerator` - Adds index-aware enumeration and "
         "exposes the numeric `Range` iterator "
         "(libraries/std/src/Collections/Enumerator.af).\n";
  out << "- `Collections/Scroller` - List-backed iterator that yields "
         "sequential elements with peek support "
         "(libraries/std/src/Collections/Scroller.af).\n";
  out << "- `Collections/Tuple` - Value-type tuples with ownership-safe "
         "destruction and `make_tuple` helper "
         "(libraries/std/src/Collections/Tuple.af).\n\n";
  out << "#### Data modeling & serialization\n";
  out << "- `JSON` - Tagged union covering JSON primitives, casting helpers, "
         "mutation, and pretty printing "
         "(libraries/std/src/JSON.af).\n";
  out << "- `JSON/Parse` - Wrapper that routes to `JSON.parse` for turning "
         "strings into JSON values "
         "(libraries/std/src/JSON/Parse.af).\n";
  out << "- `JSON/Property` - Reserved module for future JSON/property "
         "bindings (libraries/std/src/JSON/Property.af).\n\n";
  out << "#### HTTP & networking\n";
  out << "- `HTTP` - HTTP errors, verb enum, request parser, "
         "`HTTPMessage`/`HTTPResponse`, and `listen` helper "
         "(libraries/std/src/HTTP.af).\n";
  out << "- `HTTP/Endpoint` - Endpoint wrapper/registry plus pluggable "
         "NotFound handler "
         "(libraries/std/src/HTTP/Endpoint.af).\n";
  out << "- `HTTP/Endpoints` - Sugar classes for registering "
         "GET/POST/PUT/DELETE/etc. handlers "
         "(libraries/std/src/HTTP/Endpoints.af).\n";
  out << "- `HTTP/Server` - Middleware-aware server that dispatches endpoints, "
         "supports wildcards, and formats "
         "error responses (libraries/std/src/HTTP/Server.af).\n";
  out << "- `HTTP/Middleware` - Before/after middleware registration helper "
         "applied per request "
         "(libraries/std/src/HTTP/Middleware.af).\n";
  out << "- `request.c` - C shim exposing `request`, `_aflat_server_spinUp`, "
         "and `serve` socket utilities "
         "(libraries/std/src/request.c).\n\n";
  out << "#### Utility types & error handling\n";
  out << "- `Utils/Option` - Ref-counted Option class with `resolve`, `match`, "
         "and defaulting helpers "
         "(libraries/std/src/Utils/Option.af).\n";
  out << "- `Utils/option` - Lightweight union mirroring Rust's Option with "
         "`Some`/`None` constructors "
         "(libraries/std/src/Utils/option.af).\n";
  out << "- `Utils/Result` - Class-based result value for bridging APIs that "
         "expect dynamic success/error payloads "
         "(libraries/std/src/Utils/Result.af).\n";
  out << "- `Utils/result` - Rust-style `result<T>` union with ergonomic "
         "constructors and unwrap helpers "
         "(libraries/std/src/Utils/result.af).\n";
  out << "- `Utils/Error` - Base `Error` class with type metadata, render "
         "hooks, and pattern matching "
         "(libraries/std/src/Utils/Error.af).\n";
  out << "- `Utils/Error/Render` - Decorator that lets errors plug in a render "
         "callback "
         "(libraries/std/src/Utils/Error/Render.af).\n\n";
  out << "#### Utility infrastructure & patterns\n";
  out << "- `Utils/Functions` - Capturing function wrapper that invokes stored "
         "callbacks with optional captures "
         "(libraries/std/src/Utils/Functions.af).\n";
  out << "- `Utils/Defer` - RAII helper that runs a `Function` when the object "
         "leaves scope unless dismissed "
         "(libraries/std/src/Utils/Defer.af).\n";
  out << "- `Utils/Map` - Tree-backed map with hashing, normalization, and "
         "Option-returning lookups "
         "(libraries/std/src/Utils/Map.af).\n";
  out << "- `Utils/Object` - Dynamic object base with type tagging, validation "
         "hooks, and match support "
         "(libraries/std/src/Utils/Object.af).\n";
  out << "- `Utils/Properties` - Property decorators (`computed`, `readonly`, "
         "`lazy`, `observable`, `clamped`, `with`) "
         "plus supporting classes (libraries/std/src/Utils/Properties.af).\n";
  out << "- `Utils/Observable` - Observer list that notifies subscribers when "
         "events fire "
         "(libraries/std/src/Utils/Observable.af).\n\n";
  out << "#### Tooling & frameworks\n";
  out << "- `ATest` - BDD-style testing DSL with fixtures, describe contexts, "
         "hooks, and reporting helpers "
         "(libraries/std/src/ATest.af).\n";
  out << "- `concurrency` - Process/thread abstractions (`Process`, "
         "`MProcess`), message pipes, and `AsyncResult` handles "
         "(libraries/std/src/concurrency.af).\n";
  out << "- `CLArgs` - Command-line parser with long/short flags, typed "
         "values, validation, and help text "
         "(libraries/std/src/CLArgs.af).\n";
  out << "- `Web/Content` - File-backed templating utility that applies "
         "bindings before rendering "
         "(libraries/std/src/Web/Content.af).\n";
  out << "- `Web/Content/Bind` - Binding helper that registers template "
         "placeholders and preprocessors "
         "(libraries/std/src/Web/Content/Bind.af).\n\n";
  out << "## Refreshing This README\n";
  out << "Run `aflat readme [path] [name]` to regenerate these instructions "
         "for any folder.\n";
  return out.str();
}

static void printDocLines(const std::vector<std::string> &doc,
                          const std::string &indent) {
  for (const auto &line : doc) {
    std::cout << indent;
    if (!line.empty())
      std::cout << line;
    std::cout << "\n";
  }
}

static void printDocEntries(const std::string &title,
                            const std::vector<DocEntry> &entries) {
  std::cout << title << ":\n";
  if (entries.empty()) {
    std::cout << "  (none)\n";
    return;
  }
  for (const auto &entry : entries) {
    std::cout << "  - " << entry.signature << "\n";
    if (!entry.doc.empty())
      printDocLines(entry.doc, "    ");
  }
}

static void printDocs(const std::string &source, const DocInfo &info) {
  std::cout << "Docs for " << source << ":\n";
  if (info.classes.empty()) {
    std::cout << "Classes:\n  (none)\n";
  } else {
    std::cout << "Classes:\n";
    for (const auto &entry : info.classes) {
      const ClassDoc &cls = entry.second;
      std::cout << "  " << cls.name << "\n";
      if (!cls.doc.empty())
        printDocLines(cls.doc, "    ");
      std::cout << "    Methods:\n";
      if (cls.methods.empty()) {
        std::cout << "      (none)\n";
      } else {
        for (const auto &method : cls.methods) {
          std::cout << "      - " << method.signature << "\n";
          if (!method.doc.empty())
            printDocLines(method.doc, "        ");
        }
      }
    }
  }

  std::cout << "Unions:\n";
  if (info.unions.empty()) {
    std::cout << "  (none)\n";
  } else {
    for (const auto &entry : info.unions) {
      const UnionDoc &uni = entry.second;
      std::cout << "  " << uni.name << "\n";
      if (!uni.doc.empty())
        printDocLines(uni.doc, "    ");
      std::cout << "    Variants:\n";
      if (uni.variants.empty()) {
        std::cout << "      (none)\n";
      } else {
        for (const auto &variant : uni.variants) {
          std::cout << "      - " << variant << "\n";
        }
      }
      std::cout << "    Methods:\n";
      if (uni.methods.empty()) {
        std::cout << "      (none)\n";
      } else {
        for (const auto &method : uni.methods) {
          std::cout << "      - " << method.signature << "\n";
          if (!method.doc.empty())
            printDocLines(method.doc, "        ");
        }
      }
    }
  }

  printDocEntries("Enums", info.enums);
  printDocEntries("Transforms", info.transforms);
  printDocEntries("Functions", info.functions);
}

static CompileProgress *gProgress = nullptr;
static bool gQuiet = false;
static bool gUseCache = true;
static bool gConcurrentBuild = false;

#ifndef AFLAT_TEST
int main(int argc, char *argv[]) {
  CommandLineOptions cli;
  if (!parseCommandLine(argc, argv, cli)) {
    return 1;
  }
  gQuiet = cli.quiet;
  gUseCache = !cli.noCache;
  gConcurrentBuild = cli.concurrent;
  if (cli.cleanCache)
    std::filesystem::remove_all(".cache");
  gen::CodeGenerator::enableAlertTrace(cli.traceAlerts);

  std::string filename = getExePath();
  std::string exepath = filename.substr(0, filename.find_last_of("/"));
  std::string libPathA =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/";

  std::string value = cli.command;

  if (value == "make") {
    if (cli.args.empty() || cli.args.size() > 2) {
      printUsage(argv[0]);
      return 1;
    }
    std::string pName = cli.args[0];
    if (cli.library || (cli.args.size() == 2 && cli.args[1] == "lib")) {
      libTemplate(pName);
    } else {
      buildTemplate(pName);
    }
    return 0;
  }
  if (value == "build") {
    cfg::Config config =
        cfg::loadConfig(cli.configFile, cli.updateDeps, cli.cleanDeps);
    config.debug = cli.debug;
    if (!cli.outputFile.empty())
      config.outPutFile = cli.outputFile;
    runConfig(config, libPathA, 'e');
    return 0;
  }
  if (value == "run") {
    cfg::Config config =
        cfg::loadConfig(cli.configFile, cli.updateDeps, cli.cleanDeps);
    config.debug = cli.debug;
    if (!cli.outputFile.empty())
      config.outPutFile = cli.outputFile;
    if (runConfig(config, libPathA, 'e')) {
      auto of = config.outPutFile;
      if (config.outPutFile[0] != '.' && config.outPutFile[1] != '/') {
        of = "./" + config.outPutFile;
      }
      [[maybe_unused]] int rc = system(of.c_str());
    }
    return 0;
  }
  if (value == "test") {
    cfg::Config config =
        cfg::loadConfig(cli.configFile, cli.updateDeps, cli.cleanDeps);
    config.debug = cli.debug;
    if (!cli.outputFile.empty())
      config.outPutFile = cli.outputFile;
    if (runConfig(config, libPathA, 't')) {
      [[maybe_unused]] int rc = system("./bin/a.test");
    }
    return 0;
  }
  if (value == "add" || value == "module" || value == "file") {
    if (cli.args.empty()) {
      printUsage(argv[0]);
      return 1;
    }
    std::string modualName = cli.args[0];
    std::string srcName = value == "module" ? "./src/" + modualName + "/mod.af"
                                            : "./src/" + modualName + ".af";
    std::string headerName = "./head/" + modualName + ".gs";

    if (value == "module") {
      std::filesystem::create_directories("./src/" + modualName);
    } else {
      std::string path = "";

      if (modualName.find("/") != std::string::npos) {
        path = modualName.substr(0, modualName.find_last_of("/"));
      }

      if (!path.empty() && !std::filesystem::exists("./src/" + path)) {
        std::filesystem::create_directories("./src/" + path);
      }

      if (value == "add" && !path.empty() &&
          !std::filesystem::exists("./head/" + path)) {
        std::filesystem::create_directories("./head/" + path);
      }
    }

    // create the files
    std::ofstream srcFile(srcName);

    if (value == "add") {
      std::ofstream headerFile(headerName);
      srcFile << ".root \"head\"\n";
      srcFile << ".needs \"" << modualName << ".gs\"\n";
      headerFile.close();
    }
    srcFile.close();

    std::ifstream cfgIn(cli.configFile);
    std::string cfgContent((std::istreambuf_iterator<char>(cfgIn)),
                           std::istreambuf_iterator<char>());
    cfgIn.close();

    const std::string depHeader = "[dependencies]";
    const std::string entry =
        modualName + (value == "module"
                          ? " = \"./src/" + modualName + "/mod.af\"\n"
                          : " = \"./src/" + modualName + ".af\"\n");

    if (cfgContent.find(depHeader) == std::string::npos) {
      if (!cfgContent.empty() && cfgContent.back() != '\n')
        cfgContent += '\n';
      cfgContent += depHeader + "\n" + entry;
    } else {
      if (!cfgContent.empty() && cfgContent.back() != '\n')
        cfgContent += '\n';
      cfgContent += entry;
    }

    std::ofstream cfgOut(cli.configFile, std::ios::trunc);
    cfgOut << cfgContent;
    cfgOut.close();
    return 0;
  }
  if (value == "update") {
    std::string update_command =
        "curl -s "
        "https://raw.githubusercontent.com/DeForestt/aflat/refs/heads/main/"
        "install.sh | bash";
    [[maybe_unused]] int rc = system(update_command.c_str());
    return 0;
  }
  if (value == "clean") {
    std::filesystem::remove_all("./bin");
    return 0;
  }
  if (value == "install") {
    if (cli.args.empty()) {
      printUsage(argv[0]);
      return 1;
    }
    std::string repoUrl = cli.args[0];
    std::string name = cli.installName;
    if (name.empty()) {
      size_t slash = repoUrl.find_last_of('/');
      name = slash == std::string::npos ? repoUrl : repoUrl.substr(slash + 1);
      if (name.size() > 4 && name.substr(name.size() - 4) == ".git")
        name = name.substr(0, name.size() - 4);
    }

    std::ifstream cfgIn(cli.configFile);
    std::string cfgContent((std::istreambuf_iterator<char>(cfgIn)),
                           std::istreambuf_iterator<char>());
    cfgIn.close();

    const std::string depHeader = "[dependencies]";
    const std::string entry = name + " = \"" + repoUrl + "\"\n";

    if (cfgContent.find(depHeader) == std::string::npos) {
      if (!cfgContent.empty() && cfgContent.back() != '\n')
        cfgContent += '\n';
      cfgContent += depHeader + "\n" + entry;
    } else {
      if (!cfgContent.empty() && cfgContent.back() != '\n')
        cfgContent += '\n';
      cfgContent += entry;
    }

    std::ofstream cfgOut(cli.configFile, std::ios::trunc);
    cfgOut << cfgContent;
    cfgOut.close();
    return 0;
  }
  if (value == "readme") {
    namespace fs = std::filesystem;
    std::string targetPath = cli.args.empty() ? "." : cli.args[0];
    std::string overrideName =
        cli.args.size() >= 2 ? cli.args[1] : std::string();
    fs::path dest(targetPath);
    if (dest.empty())
      dest = fs::current_path();
    bool treatAsDirectory = (fs::exists(dest) && fs::is_directory(dest)) ||
                            (!dest.has_extension() && !fs::exists(dest));
    fs::path filePath = dest;
    if (treatAsDirectory) {
      if (!fs::exists(dest))
        fs::create_directories(dest);
      filePath = dest / "README.md";
    } else {
      fs::path parent = filePath.parent_path();
      if (!parent.empty() && !fs::exists(parent))
        fs::create_directories(parent);
    }
    if (fs::exists(filePath)) {
      std::cout << "README already exists at " << filePath
                << ". Remove it or specify a new path to regenerate.\n";
      return 1;
    }
    std::string readmeName = overrideName;
    if (readmeName.empty()) {
      fs::path basePath =
          treatAsDirectory ? fs::path(dest) : filePath.parent_path();
      if (basePath.empty())
        basePath = fs::current_path();
      fs::path resolved = fs::absolute(basePath);
      std::string candidate = resolved.filename().string();
      if (candidate.empty() || candidate == "." || candidate == "..")
        candidate = "AFlat Project";
      readmeName = candidate;
    }
    std::ofstream readme(filePath);
    readme << generateReadmeContent(readmeName) << "\n";
    readme.close();
    std::cout << "Created README at " << filePath << "\n";
    return 0;
  }
  if (value == "docs") {
    if (cli.args.empty()) {
      printUsage(argv[0]);
      return 1;
    }
    std::string target = cli.args[0];
    std::string path = resolveDocsPath(target, libPathA);
    if (path.empty()) {
      std::cout << "Unable to find docs for: " << target << "\n";
      return 1;
    }
    std::ifstream input(path);
    if (!input.is_open()) {
      std::cout << "Unable to open file: " << path << "\n";
      return 1;
    }
    std::string content((std::istreambuf_iterator<char>(input)),
                        std::istreambuf_iterator<char>());
    input.close();
    DocInfo info = parseDocsFromContent(content);
    printDocs(path, info);
    return 0;
  }
  std::string outputFile = cli.outputFile;
  if (outputFile.empty() && !cli.args.empty()) {
    outputFile = cli.args[0];
  }
  if (outputFile.empty())
    outputFile = "out.s";
  if (std::filesystem::exists(value)) {
    build(value, outputFile, cfg::Mutability::Promiscuous, cli.debug);
  } else {
    std::cout << "File " << value << " does not exist\n";
    return 1;
  }
  return 0;
}
#endif

static std::string sanitizeGenerics(const std::string &input) {
  std::string result;
  bool inSingleQuote = false;
  bool inDoubleQuote = false;
  int genericDepth = 0;

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    // Handle quote toggling (ignoring escaped quotes)
    if (c == '"' && !inSingleQuote) {
      bool escaped = (i > 0 && input[i - 1] == '\\');
      if (!escaped)
        inDoubleQuote = !inDoubleQuote;
      result += c;
    } else if (c == '\'' && !inDoubleQuote) {
      bool escaped = (i > 0 && input[i - 1] == '\\');
      if (!escaped)
        inSingleQuote = !inSingleQuote;
      result += c;
    } else if (!inSingleQuote && !inDoubleQuote) {
      // Outside of quotes -> replace < and >
      if (c == '<') {
        result += "__std__generic__start__";
        genericDepth++;
      } else if (c == '>') {
        result += "__std__generic__end__";
        genericDepth--;
      } else if (c == ',' && genericDepth > 0) {
        result += "__std__generic__separator__";
      } else if (c != ' ') {
        result += c;
      }
    } else {
      // Inside quotes -> just append
      result += c;
    }
  }

  return result;
}

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug) {
  bool success = true;
  lex::Lexer scanner;
  links::LinkedList<lex::Token *> tokens;
  std::string origPath = path;
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(origPath, "parsing");
    else
      std::cout << "[parsing] " << origPath << std::endl;
  }

  auto filename = getExePath();
  auto wd = std::filesystem::current_path();
  auto exepath = filename.substr(0, filename.find_last_of("/"));
  auto libPath =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/head/";
  std::ifstream ifs(path);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  ifs.close();
  gen::scope::ScopeManager::getInstance()->reset();
  try {
    try {
      PreProcessor pp;
      tokens = scanner.Scan(
          pp.PreProcess(content, libPath,
                        std::filesystem::path(path).parent_path().string()));
    } catch (int x) {
      int line = 1;
      for (int i = 0; i < x && i < content.size(); ++i)
        if (content[i] == '\n')
          line++;
      error::report(path, line, "unparsable character", content);
      return false;
    }
    tokens.invert();
    parse::Parser parser(mutability);

    auto Prog = parser.parseStmt(tokens);

    auto lower = parse::lower::Lowerer(Prog);

    auto outputID = output.substr(0, output.find_last_of("."));
    if (outputID.find("/") != std::string::npos) {
      outputID = outputID.substr(outputID.find_last_of("/") + 1);
    }

    // if output id = mod then switch to the parent directory
    if (outputID == "mod") {
      outputID = output.substr(0, output.find_last_of("/"));
      outputID = outputID.substr(outputID.find_last_of("/") + 1);
    }

    gen::CodeGenerator genny(
        outputID, parser, content,
        std::filesystem::path(path).parent_path().string());
    genny.mutability() = mutability;
    auto file = genny.GenSTMT(Prog);
    if (!gQuiet) {
      if (gProgress)
        gProgress->update(origPath, "generating");
      else
        std::cout << "[generating] " << origPath << std::endl;
    }
    file.collect();
    if (genny.hasError()) {
      success = false;
      if (std::filesystem::exists(output))
        std::filesystem::remove(output);
      return success;
    }

    file.text.invert();
    file.bss.invert();
    file.linker.invert();
    file.data.invert();

    std::ofstream ofs;

    // if there is a dot as the first char of the path, remove it
    if (path[0] == '.') {
      path = path.substr(1);
    };

    if (path[0] != '/') {
      path = wd.string() + "/" + path;
    } else {
      path = wd.string() + path;
    }

    // assembly file output
    ofs.open(output);

    // output linker commands
    while (file.linker.head != nullptr) {
      ofs << sanitizeGenerics(file.linker.pop()->toString());
    }
    // text section output
    ofs << "\n\n.text\n\n";
    // write the .file directive if in debug mode
    if (debug) {
      ofs << ".file\t\"" << path << "\"\n";
    }
    int logicalLine = -1;

    while (file.text.head != nullptr) {
      auto inst = file.text.pop();
      if (inst->logicalLine != logicalLine && debug &&
          dynamic_cast<asmc::Label *>(inst) == nullptr &&
          inst->logicalLine > 0) {
        logicalLine = inst->logicalLine;
      }
      if (debug)
        if (inst->logicalLine > 0 &&
            dynamic_cast<asmc::Define *>(inst) == nullptr)
          ofs << ".line " << inst->logicalLine - 1 << "\n";
        else if (logicalLine > 0 &&
                 dynamic_cast<asmc::Define *>(inst) != nullptr)
          ofs << ".line " << logicalLine - 1 << "\n";
      auto str = sanitizeGenerics(inst->toString());
      // replace '\n' with "\n .line " + line number
      // while(str.find('\n') != std::string::npos){
      //   auto index = str.find('\n');
      //   str = str.substr(0, index) + "\n.line " +
      //   std::to_string(inst->logicalLine) + "\n" + str.substr(index +
      //   1);
      // }
      if (dynamic_cast<asmc::Define *>(inst) == nullptr)
        ofs << str;
      // if (debug && dynamic_cast<asmc::Define *>(inst) != nullptr) ofs <<
      // inst->toString();
    }

    // data section output
    ofs << "\n\n.data\n\n";
    while (file.data.head != nullptr) {
      ofs << sanitizeGenerics(file.data.pop()->toString());
    }

    // bss section output
    ofs << "\n\n.bss\n\n";
    while (file.bss.head != nullptr) {
      ofs << sanitizeGenerics(file.bss.pop()->toString());
    }
    ofs.close();
    if (!gQuiet) {
      if (gProgress)
        gProgress->update(origPath, "done");
      else
        std::cout << "[done] " << origPath << std::endl;
    }
  } catch (err::Exception &e) {
    success = false;
    int line = error::extractLine(e.errorMsg);
    error::report(path, line, e.errorMsg, content);
    if (std::filesystem::exists(output))
      std::filesystem::remove(output);
    if (!gQuiet && gProgress)
      gProgress->update(origPath, "done");
  }
  return success;
};

/*
 * function name:   getExePath
 * description:     gets the path of the executable
 * parameters:      none
 * return value:    string - the path of the executable
 */
std::string getExePath() {
  char result[200];
  auto count = readlink("/proc/self/exe", result, 200);
  return std::string(result, (count > 0) ? count : 0);
}

/*
 * function name:   buildTemplate
 * description:     creates a template file for the user to write their program
 * parameters:      std::string value - the name of the project to be created
 * return value:    void
 */
void buildTemplate(std::string value) {
  std::filesystem::create_directories(value);
  std::filesystem::create_directories(value + "/src");
  std::filesystem::create_directories(value + "/src/test");
  std::filesystem::create_directories(value + "/head");
  std::filesystem::create_directories(value + "/bin");

  std::ofstream outfile(value + "/src/main.af");
  outfile << ".needs <std>\n";
  outfile << "import {print} from \"io\" under io;\n\n";
  outfile
      << "int main(){\n\tio.print(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
  outfile.close();

  outfile = std::ofstream(value + "/src/test/test.af");
  outfile << R"(.needs <std>
.needs <test>

import {beforeAll, beforeEach, afterEach, afterAll, describe, it, itSkip, fix, getFixture, assertEqual, assertTrue, summary} from "ATest" under test;

class HookState {
    mutable bool beforeAllRan = false;
    mutable int beforeEachCount = 0;
    mutable int afterEachCount = 0;
    mutable bool afterAllRan = false;

    fn init() -> Self {
        return my;
    };
};

fn main() -> int {
    test.fix("hookState", fn () {
        return new HookState();
    });

    test.describe("Hooks and fixtures", fn () {
        test.beforeAll(fn () {
            HookState state = test.getFixture("hookState");
            state.beforeAllRan = true;
        });

        test.beforeEach(fn () {
            HookState state = test.getFixture("hookState");
            state.beforeEachCount = state.beforeEachCount + 1;
        });

        test.afterEach(fn () {
            HookState state = test.getFixture("hookState");
            state.afterEachCount = state.afterEachCount + 1;
        });

        test.afterAll(fn () {
            HookState state = test.getFixture("hookState");
            state.afterAllRan = true;
        });

        test.it("tracks hook counts", fn () {
            HookState state = test.getFixture("hookState");
            test.assertTrue(state.beforeAllRan);
            test.assertEqual(state.beforeEachCount, 1);
            test.assertEqual(state.afterEachCount, 0);
        });

        test.itSkip("unimplemented behavior", "demonstration");
    });

    test.describe("Simple assertions", fn () {
        test.it("compares strings", fn () {
            test.assertTrue(`aflat` == `aflat`);
        });

        test.it("adds numbers", fn () {
            test.assertEqual(2 + 2, 4);
        });
    });

    test.summary();
    return 0;
};
)";
  outfile.close();

  outfile = std::ofstream(value + "/aflat.cfg");

  // Write the standard Config file in INI format
  outfile << "[build]\n";
  outfile << "main = main\n";
  outfile << "test = test/test\n";
  outfile.close();

  outfile = std::ofstream(value + "/README.md");
  outfile << generateReadmeContent(value) << "\n";
  outfile.close();
}

/*
 * function name: LibTemplate
 * description:   creates a template file for the user to write their library
 * parameters:    std::string value - the name of the library to be created
 * return value:  void
 */
void libTemplate(std::string value) {
  std::filesystem::create_directories(value);
  std::filesystem::create_directories(value + "/src");
  std::filesystem::create_directories(value + "/src/test");
  std::filesystem::create_directories(value + "/bin");

  std::ofstream outfile(value + "/src/mod.af");
  outfile << ".needs <std>\n";
  outfile << "export int " << value << "(int a, int b){\n\treturn a + b;\n};\n";
  outfile.close();

  outfile = std::ofstream(value + "/aflat.cfg");
  outfile << "[build]\n";
  outfile << "main = mod\n";
  outfile << "test = test/test\n";
  outfile << "\n[dependencies]\n";
  outfile << "mod = \"./src/mod.af\"\n";
  outfile.close();

  outfile = std::ofstream(value + "/src/test/test.af");
  outfile << ".needs <std>\n.needs <test>\n\n";
  outfile << "import {beforeAll, beforeEach, afterEach, afterAll, describe, "
             "it, itSkip, fix, getFixture, assertEqual, assertTrue, summary} "
             "from \"ATest\" under test;\n";
  outfile << "import {" << value << "} from \"src/mod\";\n\n";
  outfile << R"(class HookState {
    bool beforeAllRan;
    int beforeEachCount;
    int afterEachCount;
    bool afterAllRan;

    fn init() -> Self {
        my.reset();
        return my;
    };

    fn reset() -> void {
        my.beforeAllRan = false;
        my.beforeEachCount = 0;
        my.afterEachCount = 0;
        my.afterAllRan = false;
    };
};

mutable HookState sharedHookState = NULL;

fn hookState() -> HookState {
    if sharedHookState == NULL {
        sharedHookState = new HookState();
    };
    return sharedHookState;
};

fn main() -> int {
    test.fix("hookState", fn () -> HookState {
        return hookState();
    });

    hookState().reset();

    test.describe("Hooks and fixtures", fn () {
        test.beforeAll(fn () {
            HookState state = test.getFixture("hookState");
            state.beforeAllRan = true;
        });

        test.beforeEach(fn () {
            HookState state = test.getFixture("hookState");
            state.beforeEachCount = state.beforeEachCount + 1;
        });

        test.afterEach(fn () {
            HookState state = test.getFixture("hookState");
            state.afterEachCount = state.afterEachCount + 1;
        });

        test.afterAll(fn () {
            HookState state = test.getFixture("hookState");
            state.afterAllRan = true;
        });

        test.it("tracks hook counts", fn () {
            HookState state = test.getFixture("hookState");
            test.assertTrue(state.beforeAllRan);
            test.assertEqual(state.beforeEachCount, 1);
            test.assertEqual(state.afterEachCount, 0);
        });

        test.it("accumulates across tests", fn () {
            HookState state = test.getFixture("hookState");
            test.assertEqual(state.beforeEachCount, 2);
            test.assertEqual(state.afterEachCount, 1);
        });

        test.itSkip("unimplemented behavior", "demonstration");
    });
)";
  outfile << "    test.describe(\"Library function\", fn () {\n";
  outfile << "        test.it(\"adds numbers\", fn () {\n";
  outfile << "            test.assertEqual(" << value << "(1, 2), 3);\n";
  outfile << "        });\n";
  outfile << "    });\n\n";
  outfile << R"(    test.summary();
    return 0;
};
)";

  outfile.close();

  outfile = std::ofstream(value + "/README.md");
  outfile << generateReadmeContent(value) << "\n";
  outfile.close();
}

bool objectUpToDate(const std::string &src, const std::string &obj,
                    const std::string &libPath) {
  namespace fs = std::filesystem;
  if (!fs::exists(obj))
    return false;
  auto objTime = fs::last_write_time(obj);
  if (objTime < fs::last_write_time(src))
    return false;
  std::ifstream ifs(src);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      std::istreambuf_iterator<char>());
  PreProcessor pp;
  pp.PreProcess(content, libPath, fs::path(src).parent_path().string());
  for (const auto &inc : pp.getIncludes()) {
    if (fs::exists(inc) && objTime < fs::last_write_time(inc))
      return false;
  }
  return true;
}

bool compileCFile(const std::string &path, bool debug) {
  namespace fs = std::filesystem;
  std::string src = "./src/" + path + ".c";
  std::string obj = "./.cache/" + path + ".o";

  if (gUseCache && fs::exists(obj) &&
      fs::last_write_time(obj) >= fs::last_write_time(src)) {
    if (!gQuiet) {
      if (gProgress)
        gProgress->update(src, "cached");
      else
        std::cout << "[cached] " << src << std::endl;
    }
    return true;
  }

  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "parsing");
    else
      std::cout << "[parsing] " << src << std::endl;
  }

  fs::create_directories(fs::path(obj).parent_path());
  std::string flags;
  if (debug)
    flags = "-g -c -no-pie -z noexecstack ";
  else
    flags = "-O3 -march=native -c -no-pie -z noexecstack ";
  std::string cmd = "gcc " + flags + src + " -o " + obj;

  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "generating");
    else
      std::cout << "[generating] " << src << std::endl;
  }
  bool result = system(cmd.c_str()) == 0;
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "done");
    else if (result)
      std::cout << "[done] " << src << std::endl;
  }
  return result;
}

ModuleResult compileModule(const std::string &mod, const cfg::Config &config,
                           const std::string &libPath) {
  namespace fs = std::filesystem;
  std::string src = "./src/" + mod + ".af";
  std::string asmPath = "./.cache/" + mod + ".s";
  std::string objPath = "./.cache/" + mod + ".o";

  bool useCached = gUseCache && objectUpToDate(src, objPath, libPath + "head/");
  auto objTime = fs::exists(objPath) ? fs::last_write_time(objPath)
                                     : fs::file_time_type::min();
  for (const auto &dep : config.modules) {
    if (dep == mod)
      continue;
    std::string depSrc = "./src/" + dep + ".af";
    if (fs::exists(depSrc) && fs::last_write_time(depSrc) > objTime)
      useCached = false;
  }
  if (useCached && !gQuiet) {
    if (gProgress)
      gProgress->update(src, "cached");
    else
      std::cout << "[cached] " << src << std::endl;
  }

  if (!useCached) {
    fs::create_directories(fs::path(asmPath).parent_path());
    if (build(src, asmPath, config.mutability, config.debug)) {
      std::string cmd = "gcc -c " + asmPath + " -o " + objPath;
      if (system(cmd.c_str()) != 0) {
        return {objPath, false};
      }
      if (!config.asm_)
        fs::remove(asmPath);
    } else {
      return {objPath, false};
    }
  }
  return {objPath, true};
}

bool runConfig(cfg::Config &config, const std::string &libPath) {
  return runConfig(config, libPath, 'e');
}

bool runConfig(cfg::Config &config, const std::string &libPath, char pmode) {
  namespace fs = std::filesystem;
  std::vector<std::string> linker;
  bool hasError = false;

  std::string ofile = config.outPutFile;

  if (pmode == 'e') {
    const auto entryPoint = config.entryPoint;
    config.modules.push_back(entryPoint);
  } else if (pmode == 't') {
    config.modules.push_back(config.testFile);
  }

  std::vector<std::string> sources;
  for (const auto &mod : config.modules)
    sources.push_back("./src/" + mod + ".af");
  for (const auto &file : config.cFiles)
    sources.push_back("./src/" + file + ".c");

  CompileProgress progress(sources, gQuiet);
  if (!gQuiet)
    gProgress = &progress;

  if (gConcurrentBuild) {
    std::vector<std::future<ModuleResult>> futures;
    for (const auto &mod : config.modules) {
      futures.emplace_back(std::async(std::launch::async, compileModule, mod,
                                      std::cref(config), libPath));
    }
    for (auto &f : futures) {
      ModuleResult r = f.get();
      if (!r.success)
        hasError = true;
      linker.push_back(r.objPath);
    }
  } else {
    for (const auto &mod : config.modules) {
      ModuleResult r = compileModule(mod, config, libPath);
      if (!r.success) {
        hasError = true;
        continue;
      }
      linker.push_back(r.objPath);
    }
  }

  for (auto file : config.cFiles) {
    if (compileCFile(file, config.debug)) {
      linker.push_back("./.cache/" + file + ".o");
    } else {
      hasError = true;
    }
  }

  std::vector<std::string> libs = {
      "io.s",
      "Collections.s",
      "math.s",
      "strings.s",
      config.compatibility ? "std-cmp.s" : "std.s",
      "concurrency.s",
      "files.s",
      "asm.s",
      "String.s",
      "DateTime.s",
      "HTTP.s",
      "request.s",
      "ATest.s",
      "CLArgs.s",
      "System.s",
      "Utils_Result.s",
      "Utils_result.s",
      "Utils_Option.s",
      "Utils_Functions.s",
      "Utils_Map.s",
      "Utils_Properties.s",
      "Utils_Object.s",
      "Utils_Error.s",
      "Error_Render.s",
      "HTTP_Endpoint.s",
      "HTTP_Server.s",
      "HTTP_Endpoints.s",
      "HTTP_Middleware.s",
      "Web_Content.s",
      "Web_Content_Bind.s",
      "JSON.s",
      "JSON_Parse.s",
      "JSON_Property.s",
      "Iterator.s",
      "Enumerator.s",
      "Scroller.s",
      "vector.s",
      "Utils_Defer.s",
      "Memory.s",
      "Utils_Observable.s",
      "unordered_map.s",
      "Tuple.s",
  };

  for (const auto &lib : libs) {
    linker.insert(linker.begin(), libPath + lib);
  }

  if (hasError) {
    std::cout << "Errors detected. Skipping linking." << std::endl;
    gProgress = nullptr;
    return false;
  }

  // run gcc on the linkerList
  std::string linkerList = "";

  for (auto &s : linker)
    linkerList += s + " ";

  if (pmode == 't') {
    ofile = "./bin/a.test ";
  };

  std::string gcc =
      compilerutils::buildLinkCmd(ofile, linkerList, config.debug);
  [[maybe_unused]] int rc = system(gcc.c_str());
  linker.erase(linker.begin(), linker.begin() + libs.size());

  if (!config.asm_) {
    for (auto &s : linker) {
      if (gUseCache && s.rfind("./.cache/", 0) == 0)
        continue;
      std::filesystem::remove(s);
    }
  }
  gProgress = nullptr;
  std::cout << std::endl;
  return true;
}
