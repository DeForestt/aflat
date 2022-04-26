CXX       := g++
CXX_FLAGS := -std=c++17 -ggdb -g -fpermissive

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   :=
EXECUTABLE  := main

GDB := gdb


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(SRC)/CodeGenerator/*.cpp $(SRC)/Parser/*.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES) -pthread;
	bash rebuild-libs.sh

clean:
	-rm $(BIN)/*

debug: $(GDB) ./$(BIN)/$(EXECUTABLE)