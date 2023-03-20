
/**
 * @file
 * @author Rui Zhang <zhang_ray@stu.scu.edu.cn>
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Header for the DataSet class
*/

#ifndef CUSR_DATA_SET_H
#define CUSR_DATA_SET_H
// Std

namespace cusr {

    /** Represents a dataset on the GPU*/
    struct GPUDataset {
        float *dataset;         // Pointer to GPU 1D array containing the flattened data
        size_t dataset_pitch;   // Pitch is some sort of translation factor used when transferring 2D memeory arrays between GPU and host
        float *label;           // Pointer to GPU 1D array containing the target varaible
        int dataset_size;       // Number of samples in GPU data
        size_t *subset;         // Pointer to GPU 1D array containing subset_size number of indices within dataset to evaluate
        int subset_size;        // Number of samples to evaluate in GPU data
    };

}

#endif