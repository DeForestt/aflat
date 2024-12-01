CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb -g -fpermissive

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES := -pthread

# Define executables
EXECUTABLE     := aflat
EXECUTABLE_LSP := aflat_lsp

# Source files (excluding the main files)
SOURCES := $(filter-out $(SRC)/main.cpp $(SRC)/main.lsp.cpp, $(wildcard $(SRC)/*.cpp)) \
           $(wildcard $(SRC)/CodeGenerator/*.cpp) \
           $(wildcard $(SRC)/Parser/*.cpp) \
           $(wildcard $(SRC)/Parser/AST/Statements/*.cpp)

MAIN_SRC       := $(SRC)/main.cpp
MAIN_LSP_SRC   := $(SRC)/main.lsp.cpp

# GDB debugger
GDB := gdb

.PHONY: all clean libs debug

# Default target builds both executables
all: $(BIN)/$(EXECUTABLE) $(BIN)/$(EXECUTABLE_LSP)

# Run the main executable
run: $(BIN)/$(EXECUTABLE)
	clear
	./$(BIN)/$(EXECUTABLE)

# Build aflat
$(BIN)/$(EXECUTABLE): $(MAIN_SRC) $(SOURCES)
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES)
	bash ./rebuild-libs.sh

# Build aflat_lsp
$(BIN)/$(EXECUTABLE_LSP): $(MAIN_LSP_SRC) $(SOURCES)
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES)
	bash ./rebuild-libs.sh

# Clean the build directory
clean:
	-rm -f $(BIN)/*

# Rebuild libraries
libs:
	bash ./rebuild-libs.sh

# Debug the main executable
debug: $(GDB) ./$(BIN)/$(EXECUTABLE)

