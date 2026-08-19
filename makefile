CXX       := g++
CXX_FLAGS := -std=c++17 -O3 -march=native -fpermissive -g

BIN     := bin
SRC     := src
INCLUDE := src

LIBRARIES   :=
EXECUTABLE  := aflat

GDB := gdb


all: $(BIN)/$(EXECUTABLE)

run: all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(shell find $(SRC) -name '*.cpp')
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) $^ -o $@ $(LIBRARIES) -pthread;
	bash ./rebuild-libs.sh
	

clean:
	-rm $(BIN)/*

libs:
	bash ./rebuild-libs.sh

debug: $(GDB) ./$(BIN)/$(EXECUTABLE)
