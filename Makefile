
ROOT_DIR:=$(shell pwd)

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -pedantic -g -fopenmp -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"
LDFLAGS =  -lgomp


LIST = main memetico/globals memetico/data memetico/data_set
SRC = $(addsuffix .cpp, $(LIST))
OBJ = $(addprefix bin/, $(addsuffix .o, $(LIST)))

all: main

bin/%.o : %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

main: $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o bin/main

clean:
	rm -f bin/memetico/* bin/forms/* bin/tests/* bin/*

.PHONY : all clean
