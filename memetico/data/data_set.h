
/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Header for the DataSet class
*/

#ifndef MEMETICO_DATA_SET_H
#define MEMETICO_DATA_SET_H
// Std
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <memetico/gpu/gpu_dataset.h>
#include <memetico/gpu/cuda.cuh>
#include <memetico/helpers/rng.h>
#include <memetico/helpers/text.h>

using namespace cusr;

/**
 * Class representing a set of data samples
 */
class DataSet {

    public:

        /** Construct DataSet with file */
        DataSet(string file_name, bool do_gpu = false) { 
            filename = file_name; 
            gpu = do_gpu;
            target_column = -1;
            weight_column = -1;
            uncertainty_column = -1;
        };

        /** @brief Free GPU data */
        ~DataSet() {
            if( gpu ) {
                if(device_data.subset_size > 0) {
                    freeSubset(&device_data);
                    device_data.subset_size = 0;
                }

                freeDataSetAndLabel(&device_data);
            }
        }

        /** Target values for samples */
        vector<double>          y;

        /** Independent variable values for samples */
        vector<vector<double>>  samples;

        /** Weight values for samples */
        vector<double>          weight;

        /** Uncertainty values for samples  */
        vector<double>          dy;

        /** Names of the independent varaibles */
        static vector<string>   IVS;

        /** @brief Return filename for DataSet */
        string get_file()       { return filename; };

        /** @brief Return number of samples */
        size_t get_count()      { return y.size(); };

        /** @brief Return if GPU is being used */
        bool get_gpu()          {return gpu;};

        /** @brief Return if all samples have weight */
        bool has_weight()       { return weight.size() == get_count();}

        /** @brief Return if all samples have uncertainty */
        bool has_uncertainty()  { return dy.size() == get_count();}

        /** @brief Load data from filename into the object */
        void load();

        /** @brief Output DataSet to CSV */
        void csv(string file_name);

        /** @brief Get indexes for a percentage of the DataSet uniformly and at random */
        vector<size_t> subset(float pct, bool to_GPU = true);

        // GPU specific 

        /** @brief GPU dataset for use in cuda.cuh */
        GPUDataset device_data;

        /** @brief GPU initialisation */
        void setup_gpu() {

            // GPU must be configured 
            if(!gpu)    return;

            vector<vector<float>> float_samples;
            for(size_t i = 0; i < samples.size(); i++) {
                vector<float> temp;
                for(size_t j = 0; j < IVS.size(); j++) {
                    temp.push_back(samples[i][j]);
                }
                float_samples.push_back(temp);
            }

            vector<float> float_y;
            for(size_t i = 0; i < y.size(); i++)
                float_y.push_back(y[i]);
            
            vector<float> float_w;
            if( has_weight() ) {
                for(size_t i = 0; i < y.size(); i++)
                    float_w.push_back(weight[i]);
            }

            copyDatasetAndLabel(&device_data, float_samples, float_y, float_w); 
            device_data.subset_size = 0;
        }
    
    private: 

        /** Load first row of file */
        void load_header(string s);

        /** Load data line in file */
        void load_data(string s);

        /** Location of file used to generate dataset */
        string          filename;

        /** Weight column */
        int             weight_column;

        /** Uncertainty column */
        int             uncertainty_column;

        /** Target column */
        int             target_column;

        /** Flag for GPU operation */
        bool            gpu;

};

#endif