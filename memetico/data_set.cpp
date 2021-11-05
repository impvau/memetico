
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Implementation of the DataSet class
*/

#include <memetico/data_set.h>

/**
 * Construct a DataSet of samples
 * @param count_samples number of samples or rows
 * @param count_variables number of independent variables
 * @param has_dy flag for utilisation of dy
 * @param has_weight flag for utilisation of weights
 * @return DataSet
 */
DataSet::DataSet(size_t count_samples, size_t count_variables, bool has_dy, bool has_weights, vector<string> variable_names) {

    count = count_samples;
    ivs = count_variables;
    names = variable_names;

    y = new double[count];
    number = new size_t[count];
    
    variables = (double **)malloc(count*sizeof(double));
    for (size_t i = 0; i < count; i++)  
        variables[i] = new double[ivs];

    if( has_dy )            dy = new double[count];
    else                    dy = nullptr;

    if( has_weights )       weight = new double[count];
    else                    weight = nullptr;

}

/**
 * Deconstruct DataSet 
 */
DataSet::~DataSet() {

    cout << "~DataSet" << endl;
    
    if(y != nullptr)        free(y);
    if(number != nullptr)   free(y);
    if(dy != nullptr)       free(dy);
    if(weight != nullptr)   free(weight);   

    for (size_t i = 0; i < count; i++) 
        free(variables[i]);  
    
    free(variables);

}

/**
 * Get indexes for a percentage of the DataSet uniformly and at random
 * 
 * @param pct value between 0 and 1 representing the percentage of data to select
 * 
 * @return number of elements in arr
 * 
 */
vector<size_t> DataSet::subset(double pct) {

    size_t ret_count = (long) (pct * count);
    return memetico::RANDINT.unique_set(ret_count, 0, count);

}

/**
 * Output DataSet state
 * 
 * @param   out         Output stream to write to
 *                      Defaults to cout
 * 
 * @param   precision   resolution of real numerical output
 *                      Defaults to memetico::PREC
 * 
 * @return          void
 */
void DataSet::show(ostream& out, size_t precision) {

    size_t temp_precision = out.precision();
    out.precision(precision);

    size_t pad = precision+5;
    
    for(size_t i = 0; i < count; i++) {

        // Header
        if( i == 0) {
                
            out << setw(8) << "#" << setw(8) << "Sample" << setw(pad) << "y";
            for(size_t j = 0; j < ivs; j++)
                out << setw(pad) << names[j];
            
            if(dy != nullptr)
                out << setw(pad) << "dy";

            if(weight != nullptr)
                out << setw(pad) << "weight";

            out << endl;
            out << "----------------------------" << endl;
        }

        out << setw(8) << i+1;
        out << setw(8) << number[i];
        out << setw(pad) << y[i];

        for(size_t j = 0; j < ivs; j++)                    
            out << setw(pad) << variables[i][j];

        if(dy != nullptr)
            out << setw(pad) << dy[i];

        if(weight != nullptr)
            out << setw(pad) << weight[i];
            
        out << endl;
    }

    cout << endl;
    out.precision(temp_precision);

}

/**
 */
void DataSet::csv(ostream& out, size_t precision) {

    
    size_t temp_precision = out.precision();
    out.precision(precision);
    
    for(size_t i = 0; i < count; i++) {

        // Header
        if( i == 0) {
                
            out << "y";
            for(size_t j = 0; j < ivs; j++)
                out << "," << names[j];
            
            if(dy != nullptr)
                out << "," << "dy";

            if(weight != nullptr)
                out << "," << "weight";

            out << endl;
        }

        out << y[i];

        for(size_t j = 0; j < ivs; j++)                    
            out << ","  << variables[i][j];

        if(dy != nullptr)
            out << ","  << dy[i];

        if(weight != nullptr)
            out << ","  << weight[i];
            
        out << endl;
    }

    out.precision(temp_precision);
}
