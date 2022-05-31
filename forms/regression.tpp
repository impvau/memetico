
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Implementation of the simple linear regression class
 * 
 */

/**
 * @brief Output operator for Regression<T>
 * We output a Regression as 
 * 'coeff_val1*(var_name1)+coeff_val2*(var_name2)+...+coeff_valN*(var_nameN)+c'
 * @return os
 */
template <class T>
ostream& operator<<(ostream& os, Regression<T>& r) {

    bool has_written = false;

    // For all parameters
    for(size_t i = 0; i < r.get_count(); i++) {

        // If active
        if( r.get_active(i) ) {

            // If not the first parameter, we have existing text, and the next number is not a negative, append a "+"
            if( i != (size_t)0 && has_written && r.get_param(i) > 0)
                os << "+";

            // Indicate we have written
            has_written = true;
            
            // Print the parameter, silence appearance of '1' or -1 unless constant
            if( abs(r.get_param(i)) != 1 || i == r.get_count()-1)
                os << r.get_param(i);

            // If coeff is not 1/-1 and not the constant, add *
            if( abs(r.get_param(i)) != 1 && i != r.get_count()-1)
                os << "*";
            
            // If not a constant, multiply by the IV
            if(i != r.get_count()-1) {

                if( memetico::FORMAT == PrintLatex || memetico::FORMAT == PrintNumpy)
                    os << Data::IVS[i];
                
                if( memetico::FORMAT == PrintExcel ) 
                    os << memetico::excel_name(i+2);

            }
            
        }
    }
    return os;
}

/**
 * Clone Regression<T> and return its pointer
 * - Copy penalty, error, fitness, count, params, active 
 * @return Regression<T>*
 */
template <class T>
Regression<T>* Regression<T>::clone() {

    // Create new structure
    Regression<T>* o = new Regression<T>(0);

    // Copy attributes
    o->set_penalty(this->penalty);
    o->set_error(this->error);
    o->set_fitness(this->fitness);

    delete o->term;
    o->term = this->term->clone();
    
    return o;

}

/**
 * @brief Mutate process for Regression class
 *  - Selects and toggles the active flag for a paramter uniform at random
 * @param m model for consideration in the mutation process
 */
template <class T>
void Regression<T>::mutate(Model<T>* m) {

    // Select any parameter at random (not size()+1 as RANDINT is inclusive)
    size_t param_pos = memetico::RANDINT(0, get_count()-1);
    set_active(param_pos, !get_active(param_pos));

}

/**
 * @brief Recombination for Regression
 *  
 * @param m1 first parent
 * @param m2 second parent
 * 
 * @return void
 *           
 */
template <class T>
void Regression<T>::recombine(Model<T>* m1, Model<T>* m2, int method_override) {

    // Select recombine type; intersection, union and xor at random
    int method;
    if( method_override > -1)   method = method_override;
    else                        method = memetico::RANDINT(0,2);

    // For each parameter
    for(size_t i = 0; i < get_count(); i++) {

        // Set active flag based on rebomination
        switch(method) {
            case 0:     set_active(i, m1->get_active(i) | m2->get_active(i)); break;       
            case 1:     set_active(i, m1->get_active(i) & m2->get_active(i)); break;       
            case 2:     set_active(i, m1->get_active(i) ^ m2->get_active(i)); break;       
            default:    cout << "Could not determine recombine method" << endl;
        }

        // If the parameter is active, detemine the coefficient based on the parents
        if(get_active(i)) {

            if(m1->get_active(i) && m2->get_active(i))
                //int rand = memetico::RANDINT(-1,4);
                //cout << "rnd:" << rand << " " << m1->params[i] << "+" << rand << "*(" << m2->params[i] << "-" << m1->params[i] << ")/3" << endl;
                set_param(i, m1->get_param(i) + memetico::RANDINT(-1,4) * (m2->get_param(i) - m1->get_param(i)) / 3);
            else if (m1->get_active(i))
                set_param(i, m1->get_param(i));
            else if (m2->get_active(i))
                set_param(i, m2->get_param(i));

        }
    }   
}

/** 
 * @brief Evaluate a Regressor given sample values
 * We wish to compute the regressor value given our coefficients in params (\f$x\f$)  and the samples (\f$c\f$)
 * which is computed by calculation \f$ c_1*x_1+c_2*x_2+...c_N*x_N+c_0\} \f$
 * @param values an array of sample values to evaluate at (\f$c\f$)
 * @return double prediction of 
 */
template <class T>
double Regression<T>::evaluate(double* values) {

    double ret = 0;
    
    // For each parameter, excluding constant
    for(size_t param = 0; param < get_count()-1; param++ ) {
        
        // If active, ret += coeff*value
        if( get_active(param) ) {
            
            //double multi = multiply(this->params[param], values[param]);
            //double multi_add = add(ret, multiply(this->params[param], values[param]));
            ret = add(ret, multiply( get_param(param), values[param]));
            //cout << "param:" << param << " multi:" << multi << " ret:" << ret << " accum:" << multi_add << endl;

        }

    }
    
    // Add constant
    if( get_active(get_count()-1) )
        ret = add(ret, get_param(get_count()-1));
    
    return ret;
}

/** 
 * @brief Randomise all active coeffents between +[min, max] or -[max, min]
 * @param min minimium value
 * @param max maximum value 
 * @return void
 * @bug should update the RANDINT/RANDREAL to be RAND and overload the function. That way we can use T min and T max as params
 * @bug the min max range removes the chance of being zero, but is really not suitable for the generic. There should just be min and max
 */
template <class T>
void Regression<T>::randomise(int min, int max) {

    // For all paramters in the Regression
    for(size_t param = 0; param < get_count(); param++) {
        
        // if the parameter is active
        if(get_active(param)) {

            //  Turn on with even chance of positive/negative value
            if( memetico::RANDREAL() < 0.5 )
                set_param(param, memetico::RANDINT(min, max));
            else                               
                set_param(param, memetico::RANDINT(max*-1, min*-1));
           
        }
    }
}

/**
 * @brief Determine number of active parameters
 * @return size_t number of active parameters
 */
template <class T>
size_t Regression<T>::used() {

    size_t ret = 0;

    // For each parameter
    for( size_t param = 0; param < get_count(); param++)

        // if active, accumulate
        if( get_active(param) )
            ret++;

    // Return total active
    return ret;
}

/** 
 * @brief Ouput the Regressors parameter names and values
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
//template <class T>
/*
void Regression<T>::show(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type) {

    // Update out to print to the precision specified
    size_t temp_prec = out.precision();
    out.precision(precision);

    string param_name;

    // Determine max length of parameter names and add a little more padding for presentation
    size_t max_name = 0;
    for( size_t i = 0 ; i < this->count; i++ ) {
        if( max_name < Data::IVS.size() )
            max_name = Data::IVS.size();
    }
    max_name += 3;

    // Set the maximum parameter value length with some padding
    size_t max_param = precision + 3;

    // Print table
    out << "Model Paramters" << endl << endl;
    out << setw(max_name) << "Names" << setw(max_param) << "Value" << setw(10) << "Active?" << endl;
    out << "------------------------------------------------------------------------------------------" << endl;
    for( size_t i = 0 ; i < this->count; i++ ) {

        if(i == this->count-1)  param_name = "const";
        else                    param_name = Data::IVS[i];

        out << setw(max_name) << param_name << setw(max_param) << this->params[i] << boolalpha << setw(10) << this->active[i] << endl;
    }
    out << endl;

    // Return precision to the prior
    out.precision(temp_prec);
}*/

/** 
 * @brief  Output minimal information of the Regressor state
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
//template <class T>
/*void Regression<T>::show_min(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type, bool do_summary) {

    // Update out to print to the precision specified
    size_t temp_prec = out.precision();
    out.precision(precision);

    string variable_name;

    // Build coeffients and ivs
    bool has_item = false;
    for( size_t i = 0; i < Data::IVS.size(); i++ ) {
        
        if( print_type == memetico::PrintExcel )        variable_name = memetico::excel_name(i+2);
        else if( print_type == memetico::PrintNumpy)    variable_name = "x"+to_string(i+1);
        else                                            variable_name = Data::IVS[i];

        if( has_item && this->params[from_param+i] >= 0 && this->active[from_param+i] )      out << "+";

        if( this->params[from_param+i] != 0 && this->active[from_param+i] ) {
            out << this->params[from_param+i] << "*" << variable_name;
            has_item = true;
        }
    }
    // Tak on constant
    if( has_item && this->params[from_param+Data::IVS.size()] >= 0 && this->active[from_param+Data::IVS.size()] )  out << "+";

    if( this->params[from_param+Data::IVS.size()] != 0 && this->active[from_param+Data::IVS.size()] ) {
        out << this->params[from_param+Data::IVS.size()];
        has_item = true;
    }

    if( !has_item )
        out << "0";

    // Return precision to the prior
    out.precision(temp_prec);

}
*/