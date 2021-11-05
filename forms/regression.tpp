
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Implementation of the simple linear regression class
 * 
 */

/** 
 * @brief Evaluate a Regressor given sample values
 * 
 * We wish to compute the regressor value given our coefficients in params (\f$x\f$)  and the samples (\f$c\f$)
 * which is computed by calculation \f$ c_1*x_1+c_2*x_2+...c_N*x_N+c_0\} \f$      
 * 
 * @param values an array of sample values to evaluate at (\f$c\f$)
 * @param from_param starting point for partial evaluation of the model
 *                   an example usefullness is evaluating a single Regression equation
 *                   in a ContinuedFraction which contains mutliple sequential equations
 *                   for each term in the fraction
 * 
 * @return double value of model prediction
 */
template <class T>
double Regression<T>::evaluate(double* values, size_t from_param) {

    double ret = 0;
    size_t value_position = 0;      // Keep track of the position in values that is off base if from_param != 0 

    // Loop from_params for the number of variables that exist
    for(size_t i = from_param; i < from_param+Data::IVS.size(); i++ ) {
        
        if( this->active[i] ) {
            // Safely add the prior value and multiplication of the coefficient and sample value for the idendepent variable
            ret = memetico::add(
                        ret, 
                        memetico::multiply(
                            this->params[i],
                            values[value_position] 
                        ));
            
            if( memetico::do_debug)
                cout << "\t adding iv at " << i << " coeff: " << this->params[i] << " value:" << values[value_position] << endl;
                
        }
        value_position++;
    }

    // Safely add constant
    if( this->active[from_param+Data::IVS.size()] )
        ret = memetico::add(ret, this->params[from_param+Data::IVS.size()]);
    
    if(memetico::do_debug) {
        cout << "\t adding constant at " << from_param+Data::IVS.size() << ": " << this->params[from_param+Data::IVS.size()] << endl;
        cout << "\t term value: " << ret << " expr:"; Regression<T>::show_min(cout, memetico::PREC, from_param); cout << endl;
    }
    
    return ret;
}

/**
 * @brief Determine if the parameter at param_no is the constant without a coefficient
 * 
 * - Will consider Data::IVS.size()+1 as the coefficient length and constant
 * - Wraps aroud multiple regressions within the parameters using modulo
 * 
 * @param param_no index of parameter to check
 * @return bool
 */
template <class T>
bool Regression<T>::is_constant(size_t param_no) {
    return (param_no+1) % (Data::IVS.size()+1) == 0;
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
template <class T>
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

        if(is_constant(i))      param_name = "const";
        else                    param_name = Data::IVS[i];

        out << setw(max_name) << param_name << setw(max_param) << this->params[i] << boolalpha << setw(10) << this->active[i] << endl;
    }
    out << endl;

    // Return precision to the prior
    out.precision(temp_prec);

}

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
template <class T>
void Regression<T>::show_min(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type, bool do_summary) {

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
