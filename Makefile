
# Makefile for application
# - GPU compilation results in different slightly different MSE scores (more precise)
# 	https://docs.nvidia.com/cuda/floating-point/index.html (slightly dated)
# 	-fmad=false achieves parity in some circumstances, but particularly not with high depth fractions

ROOT_DIR:=$(shell pwd)

# Flags for compiling with NVCC
CU = nvcc
# CUFLAGS = -std=c++17 -O3 -g -ccbin g++-10 -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"
CUFLAGS = -std=c++17 -O3 -g -ccbin mpic++ -I "/usr/include/eigen3" -I "/usr/include/nlohmann/" -I "$(ROOT_DIR)/"


LDFLAGS =  -lgomp

# List the .cpp files required for compiling to .o objects, note we exlcude the .tpp template files
LIST_HELPERS_CODE = 	memetico/helpers/rng
LIST_MODEL_BASE_CODE = 	memetico/model_base/model
LIST_MODELS_CODE = 		
LIST_POP_CODE =			
LIST_DATA_CODE =		memetico/data/data_set
LIST_GPU_CODE =			memetico/gpu/cuda

# List the .cu cuda files. We can technically compile these files with NVCC and all others with g++ 
# However there are no impacts in debugging or optimisation that work differently with NVCC and
# when comparing -O3 optimisation it was 3 seconds faster on a 50second run
CULIST =

LIST_CODE = $(LIST_HELPERS_CODE) $(LIST_MODEL_BASE_CODE) $(LIST_MODELS_CODE) $(LIST_POP_CODE) $(LIST_DATA_CODE) $(LIST_GPU_CODE)
LIST = main $(LIST_CODE)

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
	$(CU) -ccbin mpic++ $^ $(LDFLAGS) -o bin/main 

# Clean bin directories to ensure recompilation
clean:
	rm -f bin/memetico/helpers/* bin/memetico/models/* bin/memetico/model_base/* bin/memetico/population/* bin/memetico/data/* bin/memetico/optimise/*  bin/main* bin/memetico/gpu/*
