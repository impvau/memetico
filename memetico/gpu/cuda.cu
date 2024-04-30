
#include <memetico/gpu/cuda.cuh>

using namespace std;
using namespace cusr;

namespace cusr {
    
    void copySubset(GPUDataset *dataset_struct, vector<size_t> &idxs) {

        // copy label set
        size_t *device_idx_arr;
        cudaMalloc((void **) &device_idx_arr, sizeof(size_t) * idxs.size());
        cudaMemcpy(device_idx_arr, thrust::raw_pointer_cast(idxs.data()), sizeof(size_t) * idxs.size(), cudaMemcpyHostToDevice);
        dataset_struct->subset = device_idx_arr;
        dataset_struct->subset_size = idxs.size();

    }

    void freeSubset(GPUDataset *dataset_struct) {
        cudaFree(dataset_struct->subset);
        dataset_struct->subset_size = 0;
    }

    void copyDatasetAndLabel(GPUDataset *dataset_struct, vector<vector<float>> &dataset, vector<float> &label, vector<float> &weight) {
        dataset_struct->dataset_size = dataset.size();
        dataset_struct->is_weighted = weight.size() > 0;

        // format dataset into column-major
        int data_size = dataset.size();
        int variable_num = dataset[0].size();

        vector<float> device_dataset;

        for (int i = 0; i < variable_num; i++) {
            for (int j = 0; j < data_size; j++) {
                device_dataset.emplace_back(dataset[j][i]);
            }
        }

        // copy dataset
        float *device_dataset_arr;
        size_t dataset_pitch;
        auto errr0 = cudaMallocPitch((void **) &device_dataset_arr, &dataset_pitch, sizeof(float) * data_size, variable_num);
        auto errr1 = cudaMemcpy2D(device_dataset_arr, dataset_pitch, thrust::raw_pointer_cast(device_dataset.data()),
                        sizeof(float) * data_size, sizeof(float) * data_size, variable_num, cudaMemcpyHostToDevice);

        // Check dataset
        //float *device_return_arr;
        //auto err1 = cudaMemcpy2D(device_return_arr, sizeof(float) * data_size, device_dataset_arr, dataset_pitch, sizeof(float) * data_size, variable_num, cudaMemcpyDeviceToHost);
        //for (int i = 0; i < data_size*variable_num; i++) {
        //    cout << device_return_arr[i] << endl;
        //}

        dataset_struct->dataset_pitch = dataset_pitch;
        dataset_struct->dataset = device_dataset_arr;

        // copy label set
        float *device_label_arr;
        auto err0 = cudaMalloc((void **) &device_label_arr, sizeof(float) * data_size);
        auto err1 = cudaMemcpy(device_label_arr, thrust::raw_pointer_cast(label.data()), sizeof(float) * data_size, cudaMemcpyHostToDevice);

        // copy weight set
        float *device_weight_arr;
        auto err2 = cudaMalloc((void **) &device_weight_arr, sizeof(float) * data_size);
        auto err3 = cudaMemcpy(device_weight_arr, thrust::raw_pointer_cast(weight.data()), sizeof(float) * data_size, cudaMemcpyHostToDevice);

        // Check dataset
        //float *device_return_label_arr;
        //auto err2 = cudaMemcpy(device_return_label_arr, device_label_arr, sizeof(float)*data_size, cudaMemcpyDeviceToHost);
        //for (int i = 0; i < data_size; i++) {
        //    cout << device_return_label_arr[i] << endl;
        //}

        dataset_struct->label = device_label_arr;
        dataset_struct->weight = device_weight_arr;

    }

    void freeDataSetAndLabel(GPUDataset *dataset_struct) {
        cudaFree(dataset_struct->dataset);
        cudaFree(dataset_struct->label);
    }

    __constant__ float d_nodeValue[MAX_PREFIX_LEN];
    __constant__ float d_nodeType[MAX_PREFIX_LEN];

#define S_OFF THREAD_PER_BLOCK * (CUSR_DEPTH + 1) * blockIdx.x + top * THREAD_PER_BLOCK + threadIdx.x

    __global__ void
    calFitnessGPU(int len, float *ds, int dsPitch, float *label, float *weights, size_t *idxs, float *stack, float *result, float *result_weights,
                        int dataset_size, bool is_subset, bool is_weighted, bool do_print, metric_t metric) {

        extern __shared__ float sharedMem[];

        // Partition sharedMem into two halves
        float* shared = sharedMem;
        float* shared_weights = sharedMem + blockDim.x; // Offset by number of threads in a block

        shared[threadIdx.x] = 0;
        shared_weights[threadIdx.x] = 0;
        
        // each thread is responsible for one datapoint
        int dataset_no = blockIdx.x * THREAD_PER_BLOCK + threadIdx.x;

        if (dataset_no < dataset_size) {

            int top = 0;

            // do stack operation according to the type of each node
            for (int i = len - 1; i >= 0; i--) {

                int node_type = d_nodeType[i];
                float node_value = d_nodeValue[i];

                if (node_type == NodeType::CONST) {
                    stack[S_OFF] = node_value;
                    top++;
                } else if (node_type == NodeType::VAR) {
                    int var_num = node_value;

                    if( is_subset) {
                        stack[S_OFF] = ((float *) ((char *) ds + var_num * dsPitch))[idxs[dataset_no]];
                    } else {
                        stack[S_OFF] = ((float *) ((char *) ds + var_num * dsPitch))[dataset_no];
                    }
                    top++;

                } else if (node_type == NodeType::UFUNC) {
                    int function = node_value;
                    top--;
                    float var1 = stack[S_OFF];
                    if (function == Function::SIN) {
                        stack[S_OFF] = std::sin(var1);
                        top++;
                    } else if (function == Function::COS) {
                        stack[S_OFF] = std::cos(var1);
                        top++;
                    } else if (function == Function::TAN) {
                        stack[S_OFF] = std::tan(var1);
                        top++;
                    } else if (function == Function::LOG) {
                        if (var1 <= 0) {
                            stack[S_OFF] = -1.0f;
                            top++;
                        } else {
                            stack[S_OFF] = std::log(var1);
                            top++;
                        }
                    } else if (function == Function::INV) {
                        if (var1 == 0) {
                            var1 = DELTA;
                        }
                        stack[S_OFF] = 1.0f / var1;
                        top++;
                    }
                } else // if (node_type == NodeType::BFUNC)
                {
                    int function = node_value;
                    top--;
                    float var1 = stack[S_OFF];
                    top--;
                    float var2 = stack[S_OFF];

                    if (function == Function::ADD) {
                        stack[S_OFF] = var1 + var2;
                        top++;
                    } else if (function == Function::SUB) {
                        stack[S_OFF] = var1 - var2;
                        top++;
                    } else if (function == Function::MUL) {
                        stack[S_OFF] = var1 * var2;
                        top++;
                    } else if (function == Function::DIV) {
                        if (var2 == 0) {
                            var2 = DELTA;
                        }
                        stack[S_OFF] = var1 / var2;
                        top++;
                    } else if (function == Function::MAX) {
                        stack[S_OFF] = var1 >= var2 ? var1 : var2;
                        top++;
                    } else if (function == Function::MIN) {
                        stack[S_OFF] = var1 <= var2 ? var1 : var2;
                        top++;
                    }
                }

                if( dataset_no == 1 && do_print)
                    printf("Final ds=1 value %f \n", stack[S_OFF]);
            }

            top--;
            float prefix_value = stack[S_OFF];
            float label_value;
            float weight;
            if(is_subset) {
                label_value = label[idxs[dataset_no]];
                weight = weights[idxs[dataset_no]];
            } else {
                label_value = label[dataset_no];
                weight = weights[dataset_no];
            }
    
            float loss;
            float fitness;
            if(metric == metric_t::mean_square_error || metric == metric_t::root_mean_square_error) {
                loss = prefix_value - label_value;
                fitness = loss * loss;
            }
            if(metric == metric_t::mean_absolute_error) {
                loss = abs(prefix_value - label_value);
                fitness = loss;
            }
            if( is_weighted )
               fitness = fitness*weight;

            shared[threadIdx.x] = fitness;
            shared_weights[threadIdx.x] = weight; // Store the weight

            //if( is_weighted)
            //    printf("Block %d, Thread %d, Fitness: %f, Weight: %f  \n", blockIdx.x, threadIdx.x, shared[threadIdx.x], shared_weights[threadIdx.x] );

        }

        __syncthreads();

        // do parallel reduction
#if THREAD_PER_BLOCK >= 1024
        if (threadIdx.x < 512) {
            shared[threadIdx.x] += shared[threadIdx.x + 512];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 512];
        }
        __syncthreads();
#endif
#if THREAD_PER_BLOCK >= 512
        if (threadIdx.x < 256) {
            shared[threadIdx.x] += shared[threadIdx.x + 256];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 256];
        }
        __syncthreads();
#endif
        if (threadIdx.x < 128) { 
            shared[threadIdx.x] += shared[threadIdx.x + 128];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 128];
        }
        __syncthreads();

        if (threadIdx.x < 64) { 
            shared[threadIdx.x] += shared[threadIdx.x + 64];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 64];
        }
        __syncthreads();

        if (threadIdx.x < 32) { 
            shared[threadIdx.x] += shared[threadIdx.x + 32];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 32];
        }
        __syncthreads();

        if (threadIdx.x < 16) { 
            shared[threadIdx.x] += shared[threadIdx.x + 16];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 16];
        }
        __syncthreads();

        if (threadIdx.x < 8) { 
            shared[threadIdx.x] += shared[threadIdx.x + 8];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 8];
        }
        __syncthreads();

        if (threadIdx.x < 4) { 
            shared[threadIdx.x] += shared[threadIdx.x + 4];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 4];
        }
        __syncthreads();

        if (threadIdx.x < 2) { 
            shared[threadIdx.x] += shared[threadIdx.x + 2];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 2];
        }
        __syncthreads();
        
        if (threadIdx.x < 1) {
            
            shared[threadIdx.x] += shared[threadIdx.x + 1];
            shared_weights[threadIdx.x] += shared_weights[threadIdx.x + 1];

            //if( is_weighted) {
                // Print the reduced values in the first thread of each block
                //printf("Block %d, Reduced Shared: %f, Weight: %f\n", blockIdx.x, shared[0], shared_weights[0]);
            //}

            result[blockIdx.x] = shared[0];
            result_weights[blockIdx.x] = shared_weights[0]; // Sum of weights in this block

        }
    }

    float *mallocStack(int blockNum) {
        float *stack;

        // allocate stack space, the size of which = sizeof(float) * THREAD_PER_BLOCK * (maxDepth + 1)
        cudaMalloc((void **) &stack, sizeof(float) * THREAD_PER_BLOCK * (CUSR_DEPTH + 1) * blockNum);

        return stack;
    }

    void calSingleProgram(GPUDataset &dataset, int blockNum, Program &program,
                        float *stack, float *result, float *result_weights, float *h_res, float *h_res_weights, metric_t metric) {

        // --------- restrict the length of prefix ---------
        assert(program.length < MAX_PREFIX_LEN);
        // -------------------------------------------------

        // -------- copy to constant memory --------
        float h_nodeValue[MAX_PREFIX_LEN];
        float h_nodeType[MAX_PREFIX_LEN];

        for (int i = 0; i < program.length; i++) {
            int type = program.prefix[i].node_type;
            h_nodeType[i] = type;
            if (type == NodeType::CONST) {
                h_nodeValue[i] = program.prefix[i].constant;
            } else if (type == NodeType::VAR) {
                h_nodeValue[i] = program.prefix[i].variable;
            } else { // unary function or binary function
                h_nodeValue[i] = program.prefix[i].function;
            }
        }

        cudaMemcpyToSymbol(d_nodeValue, h_nodeValue, sizeof(float) * program.length);
        cudaMemcpyToSymbol(d_nodeType, h_nodeType, sizeof(float) * program.length);

        int size;
        if( dataset.subset_size > 0 )
            size = dataset.subset_size;
        else 
            size = dataset.dataset_size;

        // -------- calculation and synchronization --------

        //calFitnessGPU<<<blockNum, THREAD_PER_BLOCK, sizeof(float) * THREAD_PER_BLOCK>>>
                    //(program.length, dataset.dataset, dataset.dataset_pitch, dataset.label, dataset.weight, dataset.subset, stack, result, result_weights,
                        //size, dataset.subset_size > 0, dataset.is_weighted, true, metric);
        
        calFitnessGPU<<<blockNum, THREAD_PER_BLOCK, sizeof(float) * THREAD_PER_BLOCK * 2>>>
            (program.length, dataset.dataset, dataset.dataset_pitch, dataset.label, dataset.weight, dataset.subset, stack, result, result_weights,
            size, dataset.subset_size > 0, dataset.is_weighted, false, metric);

        

        cudaDeviceSynchronize();

        // -------- reduction on the result --------
        cudaMemcpy(h_res, result, sizeof(float) * blockNum, cudaMemcpyDeviceToHost);
        cudaMemcpy(h_res_weights, result_weights, sizeof(float) * blockNum, cudaMemcpyDeviceToHost); // Get weights

        float total_fitness = 0;
        float total_weights = 0;

        for (int i = 0; i < blockNum; i++) {
            total_fitness += h_res[i];
            total_weights += h_res_weights[i]; // Sum up weights
        }

        // Calculate final weighted fitness
        if (metric == metric_t::mean_absolute_error || metric == metric_t::mean_square_error) {
            program.fitness = dataset.is_weighted ? total_fitness / total_weights : total_fitness / (float) size;
        } else if (metric == metric_t::root_mean_square_error) {
            program.fitness = dataset.is_weighted > 0 ? sqrt(total_fitness / total_weights) : sqrt(total_fitness / (float) size);
        }
        
        total_fitness = 0;
        total_weights = 0;

    }

    float 
    calculateFitness(GPUDataset &dataset, int blockNum, vector<Program> &population, metric_t metric) {

        // allocate space for result
        float *result;
        float *result_weights;
        cudaMalloc((void **) &result, sizeof(float) * blockNum);
        cudaMalloc((void **) &result_weights, sizeof(float) * blockNum);

        // allocate stack space
        float *stack = mallocStack(blockNum);

        // save result and do CPU side reduction
        float *h_res = new float[blockNum];
        float *h_res_weights = new float[blockNum];

        // evaluate fitness for each program in the population
        calSingleProgram(dataset, blockNum, population[0], stack, result, result_weights, h_res, h_res_weights, metric);

        // free memory space
        cudaFree(result);
        cudaFree(result_weights);
        cudaFree(stack);
        delete[] h_res;
        delete[] h_res_weights;

        return population[0].fitness;
    }

}

