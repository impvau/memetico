
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Header for the Data class
*/

#ifndef MEMETICO_DATA_H
#define MEMETICO_DATA_H
//Local
#include <memetico/globals.h>
#include <memetico/data_set.h>
// Std
#include <stdexcept>
#include <string>
#include <regex>
#include <fstream>
#include <iomanip>


/**
 * Class encpasuatling train and test data
 */
class Data {
    
    private:

        bool valid_suffix(string file, string suffix);

        void read_meta_data(string file, bool is_test);

        void read_samples(string file, bool is_test);

        size_t dy_col;

        size_t weight_col;

    public:

        Data(string train_file, string test_file);

        Data(string file, double pct = Data::SPLIT);

        /** Flag for use of uncertainty in train/test DataSet */
        bool has_dy;

        /** Flag for use of weight in train/test DataSet */
        bool has_weight;

        /** Flag indicating training data is not the same as test data */
        bool has_test;

        /** Training dataset */
        DataSet*    train;

        /** Testing dataset */
        DataSet*    test;

        /** Total number of training samples */ 
        static size_t   TRAIN_COUNT;

        /** Total number of testing samples */ 
        static size_t   TEST_COUNT;

        /** Total number of samples */ 
        static size_t   COUNT;

        /** Percentage of dataset that is training between 0 -> 1. If 1 it means Training = Testing and only train will exist */ 
        static double   SPLIT;

        /** Vector of independent varaible names from the data source */
        static vector<string>   IVS;

        void        show(ostream& out = cout, size_t precision = memetico::PREC);

};

#endif