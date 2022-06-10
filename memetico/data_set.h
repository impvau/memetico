
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for the DataSet class
*/

#ifndef MEMETICO_DATA_SET_H
#define MEMETICO_DATA_SET_H
//Local
#include <memetico/globals.h>
#include <memetico/rng.h>
// Std
#include <cstdlib>
#include <iomanip>

/**
 * Class representing a set of data samples
 */
class DataSet {

    public:

        /** Count of samples the same as Data::COUNT, Data::COUNT_TEST or Data::COUNT_TRAIN but replicated here privately */
        size_t      count;

        /** Target values for samples */
        double*     y;

        /** Row number of the sample in the original file */
        size_t*     number;

        /** Independent variable values for samples */
        double**    variables;
      
        /** Uncertainty values for samples */
        double*     dy;

        /** Weight values for samples */
        double*     weight;

        // See Implementation for details
        
        DataSet(size_t count_samples, size_t count_variables, bool has_dy, bool has_weights, vector<string> variable_names);

        ~DataSet();

        vector<size_t> subset(double pct);
    
        void show(ostream& out = cout, size_t precision = memetico::PREC);

        void csv(ostream& out = cout, size_t precision = memetico::PREC);
    
    private: 

        /** Count of independent variables the same as Data::IVS.size() but replicated here privately */
        size_t      ivs;

        /** List of independent variable names the same as Data::IVS but replicated here privately */
        vector<string> names;     
};

#endif