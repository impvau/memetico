
ROOT_DIR:=$(shell pwd)

CXX = g++
CXXFLAGS = -std=c++17 -O0 -Wall -pedantic -g -fopenmp -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"
LDFLAGS =  -lgomp

LIST =  memetico/globals memetico/data memetico/data_set tests/main tests/element.test tests/term.test tests/model.test tests/regression.test tests/cont_frac.test
SRC = $(addsuffix .cpp, $(LIST))
OBJ = $(addprefix bin/, $(addsuffix .o, $(LIST)))

all: main

bin/%.o : %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

main: $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o bin/main

clean:
	rm -f bin/tests/* bin/*

.PHONY : all clean
