
#ifndef MEMETICO_CUDA_H
#define MEMETICO_CUDA_H

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>
#include <vector>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <stack>

#include <memetico/globals.h>
#include <memetico/gpu/gpu_dataset.h>
#include <memetico/gpu/tree_node.h>

#define THREAD_PER_BLOCK 512
#define MAX_PREFIX_LEN 2048
#define CUSR_DEPTH 18
#define DELTA 0.01f

namespace cusr {

    using namespace std;

    /** Types of objective function implemented on the GPU*/
    typedef enum Metric {
        mean_absolute_error,     
        root_mean_square_error,
        mean_square_error
    } metric_t;

    /** A program contains a model and its evaluated   */
    struct Program {
        prefix_t prefix;    // Nodes from the tree representing the equation, changed into vector<Node> form
        int depth{};        
        int length{};       // The number of evaluations in the tree
        double fitness{};    // Evaluated fitness from the GPU
    };

    /**
     * copy dataset from host side to device side
     * host side dataset:  x0, x1, .., xn
     *                     x0, x1, .., xn
     *                     .., .., .., ..
     *                     x0, x1, .., xn
     *
     * predicted value:    y0, y1, .., ym
     * the length of predicted value equals to the length of dataset
     * dataset will be in column-major storage in the device side to perform coalesced memory access
     * @param dsStruct
     * @param dataset
     * @param label
     */
    void copyDatasetAndLabel(GPUDataset *dsStruct, vector<vector<float>> &dataset, vector<float> &label, vector<float> &weights);

    /**
     * free data structure on the device side.
     * @param dataset_struct
     */
    void freeDataSetAndLabel(GPUDataset *dataset_struct);

    /**
     * evaluate fitness for a population
     * @param dataset
     * @param blockNum
     * @param population
     * @param metric
     */
    float calculateFitness(GPUDataset &dataset, int blockNum, vector<Program> &population, metric_t metric);
  
    /** Return string equation in human-readable form */
    string prefix_to_infix(prefix_t &prefix);

    void copySubset(GPUDataset *dataset_struct, vector<size_t> &idxs);

    void freeSubset(GPUDataset *dataset_struct);
}

#endif //MEMETICO_CUDA_H
