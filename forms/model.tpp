
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Implementation of the Model class
 * 
 */

/**
 * @brief Constrct Model 
 * - Default penalty to 1
 * - Default error to double max
 * - Default fitness to double max
 * - Allocate memory for parameters
 * @param param_count Number of parameters in the model
 * @return void
 */
template <class T>
Model<T>::Model(size_t param_count) {

    // Default values
    penalty = 1;
    error = numeric_limits<double>::max();
    fitness = numeric_limits<double>::max();
    count = param_count;

    // If using the standard linear function
    if( count != 0) {

        // Allocate memory given the number of parameters
        params = new T [count];
        active = new bool [count];

        // Set default values
        for(size_t i = 0; i < count; i++) {
            params[i] = 0;
            active[i] = false;
        }

    }  

}

/**
 * @brief Deallocate memory for Model
 */
template <class T>
Model<T>::~Model() {

    // Deallocate
    if( count != 0 ) {
        delete params;
        delete active;
    }
}

/**
 * @brief Output operator for Model<T>
 * - Print active model parameters in the form
 *   'coeff_val (var_name) '
 * @return os
 */
template <class T>
ostream& operator<<(ostream& os, const Model<T>& m)
{
    
    // For all parameters in the model
    for(size_t i = 0; i < m.count; i++) {

        // if active
        if( m.active[i] ) {
            
            // print parameter value and varaible name
            os << m.params[i];
            os  << " (" << Data::IVS[i] << ") ";
        }       
    }
    return os;
}

/**
 * Clone Model and return its pointer
 * - Copy penalty, error, fitness, count, params, active 
 * @return Model<T>*
 */
template <class T>
Model<T>* Model<T>::clone() {

    // Create new structure
    Model<T>* new_object = new Model<T>(count);

    // Copy fitness
    new_object->penalty = penalty;
    new_object->error = error;
    new_object->fitness = fitness;
    new_object->count = count;

    // Copy all values 
    for( size_t param = 0; param < count; param++) {
        new_object->active[param] = active[param];
        new_object->params[param] = params[param];
    }

    return new_object;

}

// To remove later
/** 
 * @brief  Output minimal information of the Model state
 * 
 * Requires implementation from derrived classes
 * 
 * @param out           Write stream 
 * 
 * @param precision     Decimal precision
 * 
 * @param from_param    Parameter to being from. See Model<T>::evaluate() for further detail
 * 
 * @param print_type    Output format from memetico::PrintType
 * @bug we should be able to remove this function in favour of the << operator
 * 
 * @return void
 */
template <class T>
void Model<T>::show_min(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type, bool do_summary) {

    size_t temp_precision = out.precision();
    out.precision(precision);

    size_t word_pad = 10;
    size_t num_pad = precision+5;
    out << setw(word_pad) << "fitness: " << setw(num_pad) << fitness << setw(word_pad) << " error: " << setw(num_pad) << error << setw(word_pad) << " penalty: " << penalty << " params_used: " << params_used() << endl;

    out.precision(temp_precision);

}
