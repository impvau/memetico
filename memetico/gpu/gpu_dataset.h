
/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Header for the DataSet class
 * 
 * Adapted from work by @author Rui Zhang <zhang_ray@stu.scu.edu.cn> from https://link.springer.com/chapter/10.1007/978-3-031-20862-1_38
*/

#ifndef CUSR_DATA_SET_H
#define CUSR_DATA_SET_H
// Std

namespace cusr {

    /** Represents a dataset on the GPU*/
    struct GPUDataset {
        float *dataset;         // Pointer to GPU 1D array containing the flattened data
        size_t dataset_pitch;   // Pitch is some sort of translation factor used when transferring 2D memeory arrays between GPU and host
        float *label;           // Pointer to GPU 1D array containing the target variable
        float *weight;           // Pointer to GPU 1D array containing the weight variable
        bool is_weighted;
        int dataset_size;       // Number of samples in GPU data
        size_t *subset;         // Pointer to GPU 1D array containing subset_size number of indices within dataset to evaluate
        int subset_size;        // Number of samples to evaluate in GPU data
    };

}

#endif