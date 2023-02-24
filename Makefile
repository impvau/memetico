
ROOT_DIR:=$(shell pwd)

# Old flags for g++ compiling
#CC = g++
#CCFLAGS = -std=c++17 -O3 -g -fopenmp -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"

# Flags for compiling with NVCC
CU = nvcc
CUFLAGS = -std=c++17 -O0 -g -ccbin g++-10 -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"

LDFLAGS =  -lgomp

# List the .cpp files required for compiling to .o objects, note we exlcude the .tpp template files
LIST = main memetico/globals memetico/data memetico/data_set

# List the .cu cuda files. We can technically compile these files with NVCC and all others with g++ 
# However there are no impacts in debugging or optimisation that work differently with NVCC and
# when comparing -O3 optimisation it was 3 seconds faster on a 50second run
CULIST = memetico/cuda

# Append suffix to files above
SRC = $(addsuffix .cpp, $(LIST)) $(addsuffix .cu, $(CULIST)) 

# Generate object files based on the -o filenames.
# This assumes that the directories that appear in LIST or CULIST have corresponding directories in ./bin/
OBJ = $(addprefix bin/, $(addsuffix .o, $(LIST)))  $(addprefix bin/, $(addsuffix .o, $(CULIST)))

all: main

# Compile .cpp files
bin/%.o : %.cpp
	$(CU) -c $< $(CUFLAGS) -o $@

# Compile .cu files (same as .cpp)
bin/%.o : %.cu
	$(CU) -c $< $(CUFLAGS) -o $@

# Compiile main executable.
main: $(OBJ)
	$(CU) $^ $(LDFLAGS) -o bin/main 

# Clean bin directories to ensure recompilation
clean:
	rm -f bin/memetico/* bin/models/* bin/tests/* bin/*
