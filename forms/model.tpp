
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Implementation of the Model class
 * 
 */

/**
 * @brief Constrct Model 
 * 
 * - Default penalty to 1
 * - Default error to double max
 * - Default fitness to double max
 * - Allocate memory if model_params is not zero
 * 
 * @param       model_params    Number of parameters in the model
 * 
 * @return      void
 */
template <class T>
Model<T>::Model(size_t model_params) {

    // Default values
    penalty = 1;
    error = numeric_limits<double>::max();
    fitness = numeric_limits<double>::max();
    count = model_params;

    // If using the standard linear function
    if( count != 0) 
        allocate();

}

/**
 * @brief Deconstruct Model
 * 
 * - Deallocate memeory created by Model<T>::allocate
 * 
 * @return          void
 */
template <class T>
Model<T>::~Model() {

    // Deallocate
    if( count != 0 ) {
        free(params);
        free(active);
    }
}

/**
 * Clone Model and return a point to a new object
 * 
 * @return Model<T>*
 */
template <class T>
Model<T>* Model<T>::clone() {

    // Create new structure
    Model<T>* new_object = new Model<T>(count);

    // Copy params and active flags
    for(size_t i = 0; i < count; i++) {
        new_object->params[i] = params[i];
        new_object->active[i] = params[i];
    }

    // Copy fitness
    new_object->penalty = penalty;
    new_object->error = error;
    new_object->fitness = fitness;
    new_object->count = count;

    return new_object;

}

/**
 * @brief Allocate memory given a number of paramaters
 * 
 * - Allocate params array that is of type T
 * - Allocate active array that is boolean type
 * 
 * Exceptions
 * - Throw runtime_error if attempting to allocate zero-parameter Model
 * 
 * @return          void
 */
template <class T>
void Model<T>::allocate() {

    if(count == 0)
        throw runtime_error("Attempting to allocate model memory when there are no parameters");

    // Allocate memory given the number of parameters
    params = (T *)malloc(count*sizeof(T));
    active = (bool *)malloc(count*sizeof(bool));

}

/**
 * @brief Mutate process for this Model
 * 
 * - Toggles the active flag for a random varaible
 * - Expected override in derived class
 *
 * It is expected that this Model is Agent<U>::CURRENT and pocket is the associated Agent<U>::POCKET
 * The mutation process may change based on the pocket solution 
 * 
 * @param pocket the pocket to consider during mutation
 * 
 * @return void
 *           
 */
template <class T>
void Model<T>::mutate(Model<T>* model) {

    // Select a random parameter
    size_t param_pos = memetico::RANDINT(0, this->count);

    // Toggle the parameter on/off
    this->active[param_pos] = !this->active[param_pos];

    return;

}

/**
 * @brief Recombine this Model based on model1 and model2
 * 
 * For each parameter
 * - Selects intersection, union and symmetric difference at random for the recombination type i.e.
 * -- Intersection we define as OR of both booleans
 * -- Union we define as AND of both booleans
 * -- Symmetric difference we define as XOR of both booleans
 * - Set the active flag based on the selected method
 * - For parmeters that are changing from inactive to active
 * -- If only model1 or model2 was active, assign the parameter value from the active model to this
 * -- If both model1 and model2 were active, multiply 1/3 of the difference between the models by a random integer between min and max, then add model1 parameter value
 * 
 * @param model1 first Model for use in recomination
 * @param model1 second Model for use in recomination
 * @param min inclusive starting range for recomination random values
 * @param max inclusive ending range for recomination random values
 * 
 * @return void
 *           
 */
template <class T>
void Model<T>::recombine(Model<T>* model1, Model<T>* model2, int min, int max) {

    int method = memetico::RANDINT(0,2);
    
    /*
    switch(method) {
        case 0:     log << "Type: Intersection";  break;       
        case 1:     log << "Type: Union"; break;
        case 2:     log << "Type: Symmetric Difference"; break;
        default:    log << "Could not determine recombine method" << endl;
    }
    log << endl;
    log << "\t Original: "; show_min(log, memetico::PREC,0,memetico::PrintExcel, false); log << endl;
    log << "\t    First: "; model1->show_min(log, memetico::PREC,0,memetico::PrintExcel, false); log << endl;
    log << "\t   Second: "; model2->show_min(log, memetico::PREC,0,memetico::PrintExcel, false); log << endl;
    */

    for(size_t i = 0; i < count; i++) {

        bool was_off = active[i];
        switch(method) {
            case 0:     active[i] = model1->active[i] | model2->active[i]; break;       
            case 1:     active[i] = model1->active[i] & model2->active[i]; break;
            case 2:     active[i] = model1->active[i] ^ model2->active[i]; break;
            default:    cout << "Could not determine recombine method" << endl;
        }

        // If was off and now is on
        if(was_off && active[i]) {

            if(model1->active[i] and model1->active[i]) 
                params[i] =  model1->params[i] + memetico::RANDINT(min,max) * (model2->params[i] - model1->params[i]) / 3;
            else if (model1->active[i])
                params[i] = model1->params[i];
            else if (model2->active[i])
                params[i] = model2->params[i];

        }
    }
    
    //log << "\t      New: "; show_min(log, memetico::PREC,0,memetico::PrintExcel, false); log << endl;
}

/**
 * @brief Determine the number of parameters utilised by the Model
 * 
 * @return size_t
 *           
 */
template <class T>
size_t Model<T>::params_used() {

    size_t ret = 0;
    for(size_t i = 0; i < count; i++) {

        if( active[i] and params[i] != 0 )
            ret++;

    }
    return ret;
}

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

