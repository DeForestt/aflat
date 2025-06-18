Agents.MD
Welcome to the AFlat codebase. This document outlines the essential practices for contributing safely and effectively. Please read carefully before making any changes.

🚧 Expectations
Format all C++ code using:

bash
Copy
Edit
find . -path ./Lib -prune -o -path ./test/catch.hpp -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \+ -exec git add {} \+
Build with CMake. It's the supported and canonical way to configure the C++ build.

Test everything you change:

For AFlat code: edit src/main.af and run:

bash
Copy
Edit
./bin/aflat run
For C++ code: changes must include unit tests. Use the Catch2 framework located in test/.

Quick rebuilds:

Use make to rebuild the C++ binary and regenerate standard libraries:

bash
Copy
Edit
make
Standard library updates:

After editing anything in libraries/std/src, run:

bash
Copy
Edit
./rebuild-libs.sh
No output means success.

🗂️ Repo Structure
Root
README.md — explains AFlat’s goals and basic usage.

Tooling scripts: makefile, install.sh, coverage.sh.

CMakeLists.txt — builds the project; sources from src/, tests from test/.

Source Code: src/
Main Entry Point

main.cpp: Orchestrates preprocessing, parsing, lowering, and code generation.

Scanner (Lexing)

Scanner.cpp, Scanner.hpp: Tokenization logic.

Preprocessor

PreProcessor.cpp, PreProcessor.hpp: Handles .needs, .def, and includes.

Parser & AST

Parser.cpp, Parser.hpp: Builds the AST.

Parser/AST/: AST node definitions.

AST Lowering

Parser/Lower.cpp: Transforms AST for code generation.

Code Generator

CodeGenerator/CodeGenerator.cpp: Generates instructions.

include/CodeGenerator/CodeGenerator.hpp: Interface.

Also includes ScopeManager, Utils, etc.

Assembly Utilities

ASM.cpp, ASM.hpp: Defines instructions and wraps them into an asmc::File.

Headers: include/
Mirrors src/ — each subsystem (e.g., Parser, PreProcessor, CodeGenerator) has a public interface here.

Standard Library: libraries/std/
.af source files and .gs headers for built-in modules.
remember that when adding a new Standard Library module, you must also update the standard library build script. (rebuild-libs.sh)
as well as add it to the vector in src/main.cpp.

Rebuild using:

bash
Copy
Edit
./rebuild-libs.sh
Tests: test/
Contains Catch2-based unit tests.

Covers components like the code generator, preprocessor, parser, and ASM.

🧭 Where to Modify Logic
Concern	Location
Scanning / Lexing	src/Scanner.cpp, include/Scanner.hpp
Preprocessing	src/PreProcessor.cpp, include/PreProcessor.hpp
Parsing / AST	src/Parser/, include/Parser/
AST Lowering	src/Parser/Lower.cpp
Code Generation	src/CodeGenerator/, include/CodeGenerator/
ASM Formatting	src/ASM.cpp, include/ASM.hpp

Any changes to the above affect compiler behavior. These are compiled into bin/aflat via make or CMake. Standard libraries are rebuilt separately.

Let me know if you want this duplicated as a GitHub Wiki page or split into separate dev-focused and contributor-focused versions.
