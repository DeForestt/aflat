CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb -g -fpermissive
INCLUDE := include
BIN     := bin
SRC     := src

LIBRARIES   :=
EXECUTABLE  := aflat
LSP_EXECUTABLE := aflat-lsp

GDB := gdb


all: $(BIN)/$(EXECUTABLE) $(BIN)/$(LSP_EXECUTABLE)

run: all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(SRC)/CodeGenerator/*.cpp $(SRC)/Parser/*.cpp $(SRC)/Parser/AST/Statements/*.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES) -pthread;
	bash ./rebuild-libs.sh

$(BIN)/$(LSP_EXECUTABLE): src/lsp/*.cpp src/Scanner.cpp src/Exceptions.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ -pthread
	

clean:
	-rm $(BIN)/*

libs:
	bash ./rebuild-libs.sh

debug:
	$(GDB) ./$(BIN)/$(EXECUTABLE)

