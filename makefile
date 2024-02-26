CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb -g -fpermissive

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   :=
EXECUTABLE  := aflat

GDB := gdb


all: $(BIN)/$(EXECUTABLE)

run: all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(SRC)/CodeGenerator/*.cpp $(SRC)/Parser/*.cpp $(SRC)/Parser/AST/Statements/*.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES) -pthread;
	bash ./rebuild-libs.sh
	

clean:
	-rm $(BIN)/*

libs:
	bash ./rebuild-libs.sh

debug: $(GDB) ./$(BIN)/$(EXECUTABLE)