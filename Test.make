
ROOT_DIR:=$(shell pwd)

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -pedantic -g -fopenmp -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"
LDFLAGS =  -lgomp

LIST = tests/main tests/model.test
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
