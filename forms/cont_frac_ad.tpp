
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief Implementation of the Continued Fraction Regression class
*/

/**
 * @brief Construct ContinuedFraction
 * 
 * - Set depth to frac_depth
 * - Set terms to 2*depth+1
 * - Set params_per_term to Data::IVS.size()+1
 * - Set count to terms*params_per_term
 * - Allocate memeory as per Model and additionally allocate global_active array
 * - Randomise all coefficients and varaibles
 * 
 * @param frac_depth depth of the fraction
 * @return void
 * @bug Check if the Model is constructed first with memeory of frac_depth. Memory leak here?
 */
template <class T>
ContinuedFraction<T>::ContinuedFraction(size_t frac_depth) : Model<T>() {
    
    // Default values
    depth = frac_depth;
    terms = 2*depth+1;
    params_per_term = Data::IVS.size()+1;

    objs = new T*[terms];
    for(size_t i = 0; i < terms; i++) {
        
        T* temp = new T(params_per_term);
        objs[i] = temp;

    }

    // Additional Allocation
    global_active = new bool [params_per_term];
    
    randomise();
}

/**
 * @brief Deconstruct ContinuedFraction
 * 
 * - Deallocate Model variables
 * - Deallocate global_active
 */
template <class T>
ContinuedFraction<T>::~ContinuedFraction() {

    // All others handled by ~Model()
    delete global_active;
    for(size_t i = 0; i < terms; i++) {
        delete objs[i];
    }
    delete objs;

}

/**
 * Clone ContinuedFraction and return a point to a new object
 * 
 * @return ContinuedFraction<T>*
 */
template <class T>
ContinuedFraction<T>* ContinuedFraction<T>::clone() {

    ContinuedFraction<T>* o = new ContinuedFraction<T>();
    
    o->terms = terms;
    o->depth = depth;
    o->params_per_term = params_per_term;

    for(size_t i = 0; i < terms; i++) {
        delete o->objs[i];
        o->objs[i] = objs[i]->clone();
        o->global_active[i] = global_active[i];
    }

    o->penalty = this->penalty;
    o->error = this->error;
    o->fitness = this->fitness;
    o->count = this->count;

    return o;

}

/**
 * @brief Randomise paramter for frac_term at frac_param position to a uniformally random value inclusively between min and max
 * 
 * - Determine the true parameter position in the 1d params array given a term and independent variable
 * - If the parameter is globally inactive, set the coefficient to 0 and active mask off
 * - With a chance of 50 percent
 * -- Set the parameter 0 and active false OR
 * -- Set the active true and assign a coeffienct with uniform random chance between (min, max) or (-max, -min)
 * 
 * @param frac_term Term to randomise
 * @param frac_param Independent variable in term to randomise
 * @param min Inclusive minimum value for randomisation (negative or positive)
 * @param max Inclusive maximum value for randomisation (negative or positive)
 * @return void
 */
/*
template <class T>
void ContinuedFraction<T>::randomise_at(size_t term, size_t parm, int min, int max) {

    
    size_t param_pos = frac_term*params_per_term+frac_param;

    // If globally off, remove and return
    if( !global_active[frac_param] ) {
        this->params[param_pos] = 0;
        this->active[param_pos] = false;
        return;
    }

    if ( this->is_constant(param_pos) ) {

        if( memetico::RANDREAL() < 0.5 )        this->params[param_pos] = memetico::RANDINT(min, max);
        else                                    this->params[param_pos] = memetico::RANDINT(max*-1, min*-1);

        this->active[param_pos] = true;

    // Else 50% chance of randomisation
    } else if( memetico::RANDREAL() < 0.5 ) {

        // 50% chance of postive/negative
        if( memetico::RANDREAL() < 0.5 )        this->params[param_pos] = memetico::RANDINT(min, max);
        else                                    this->params[param_pos] = memetico::RANDINT(max*-1, min*-1);
    
        this->active[param_pos] = true;

    } else  {

        this->params[param_pos] = 0;
        this->active[param_pos] = false;

    }    
}
*/

/**
 * @brief Randomise all paramters to a uniformally random value inclusively between min and max
 * 
 * - Randomly turn on global active flag with 50% chance
 * - For all varaiables randomise as per randomise_at
 * 
 * @param min           Minimum value inclusive
 * @param max           Maximum value inclusive
 * @return              void
 */
/*
template <class T>
void ContinuedFraction<T>::randomise_all(int min, int max) {
   
    // Randomise
    for(size_t i = 0; i < Data::IVS.size()+1; i++) {

        global_active[i] = true;

    //    if( memetico::RANDREAL() < 0.5 )        global_active[i] = false;
    //    else                                    global_active[i] = true;
    }

    // Randomise all paramters
    for(size_t i = 0; i < terms; i++) {
        for(size_t j = 0; j < Data::IVS.size()+1; j++) {
            randomise_at(i,j,min,max);
        }
    }    
}
*/

/**
 * @brief Randomise all paramters to a uniformally random value inclusively between min and max
 * 
 * - Randomly turn on global active flag with 50% chance
 * - For all varaiables randomise as per randomise_at
 * 
 * @param min           Minimum value inclusive
 * @param max           Maximum value inclusive
 * @return              void
 */
template <class T>
void ContinuedFraction<T>::randomise(int min, int max) {

    // For all parameters
    for( size_t param = 0; param < params_per_term; param++ ) {
        
        // Randomly turn the IV on/off, always set the constant on
        if( memetico::RANDREAL() < 0.5 || params_per_term-1 == param )
            set_global_active(param, true);
        else                               
            set_global_active(param, false);

    }

    // Randomise all parameters in all terms, this will respect the active status
    for(size_t term = 0; term < terms; term++)
        objs[term]->randomise(min, max);   
    
}

/** 
 * @brief Evaluate a ContinuedFraction given sample values
 * 
 * We wish to compute the ContinuedFraction where each term has a Regressor of the form \f$ c_1*x_1+c_2*x_2+...c_N*x_N+c_0\ \f$ 
 * for each \f$ g_i(x) \f$ and \f$ h_i(x) \f$ term in the fraction \f$f(x)=g_0(x)+\frac{h_0(x)}{g_1(x)+\frac{h_1(x)}{g_2(x)+\cdots}}\f$
 * 
 * We recursively call Regression::evaluate on each term and then compute the fraction from the laster term to the first term
 * 
 * @param values an array of sample values to evaluate at (\f$c\f$) which is params_per_term in length
 * @param from_param starting point for partial evaluation of the model
 *                   an example usefullness is evaluating a single Regression equation
 *                   in a ContinuedFraction which contains mutliple sequential equations
 *                   for each term in the fraction
 * @return double value of model prediction
 * 
 * @bug need to add in the try catch for this function
 */
template <class T>
double ContinuedFraction<T>::evaluate(double* values) {

    
    double ret = 0;

    //if( memetico::do_debug )
    //    show();

    // Catch all under/overflows that occur
    try {

        // For all terms, starting from the closing term
        for(int term = get_terms()-1; term > -1; term -= 2 )

            if( term != 0) {
                // We sum the previously calculated portion with the next denominator up the fraction
                // We then divide this sum by the next numerator up the fraction
                // Consider a+b/(c+d/(e+f/g))
                // - ret1 = f/(g+ret) = f/(g+0)
                // - ret2 = d/(e+ret1)
                // - ret3 = b/(c+ret2) 
                // - Then we add the final term at 0
                //
                ret = memetico::divide(
                    objs[term-1]->evaluate(values),
                    memetico::add(
                        objs[term]->evaluate(values),
                        ret
                    )
                );
            } else
                ret = memetico::add(ret, objs[term]->evaluate(values));

    } catch (exception& e) { 
        return numeric_limits<double>::max();
    }

    return ret;
    
}

/**
 * @brief Mutate process for this ContinuedFraction
 *  
 * It is expected that this Model is Agent<U>::CURRENT and model parameter is the associated Agent<U>::POCKET
 * 
 * The hard mutation changes a global active flag meaning there is significant impact and occurs when the current (this)
 * is either better or almost as fit as the pocket OR it is greatly less fit than the pocket. This is defined as 
 * less than 1.2 * the pocket or > 2* the pocket
 * 
 * - Select a indpendent varaible uniformly at random
 * - Toggle global active flag
 * - If the active flag is now on
 * -- For each term randomise the parameter
 * 
 * The soft mutation changes a single variable meaning there is minor impact and occurs when the current (this) is within
 * 1.2 and 1.8 times the error of the pocket
 * 
 * - Select a indpendent varaible uniformly at random from those that are not globally inactive (i.e. where toggle will have effect)
 * - Toggle the active flag for that specific parameter
 * - Note we do not randomise the coefficient, rather if it is becoming active it retains the coefficient value set prior
 * 
 * @param model the model to consider during mutation, expected to be the assoicated pocket of the 
 * 
 * @return void
 *           
 */
template <class T>
void ContinuedFraction<T>::mutate(Model<T>* model) {

    /*
    size_t param_pos;

    // If current is close to the model fitness or lower OR if it is more than double the model fitness, hard mutate
    if( this->fitness  < 1.2 * model->fitness || this->fitness > 2* model->fitness ) {

        // Select uniformly at random a variable
        param_pos = memetico::RANDINT(0, Data::IVS.size()-1);
        
        // Toggle it globally
        global_active[param_pos] = !global_active[param_pos];

        // If the variable is globally active
        if(global_active[param_pos]) {

            // With 50% chance randomise it to non-zero
            for(size_t i = 0; i < terms; i++ )
                randomise_at(i,param_pos);

        } 
        // If the variable is globally off
        else {
            
            // Turn off the local mask for each level
            // We keep the previous parameter value
            for(size_t i = 0; i < terms; i++ )
                this->active[param_pos+(i*params_per_term)] = false;
        }

        //log << "Hard Mutation. Current Fitness: " << this->fitness;
        //log << " Pocket Fitness: " << model->fitness;
        //log << " IV: " << Data::IVS[param_pos];
        //log << " Now Active? " << boolalpha << global_active[param_pos] << endl;

    }
    // Else soft mutate
    else {

        // Identify parameters that we can switch on i.e. not globally inactive
        vector<size_t> potential_params;
        for(size_t i = 0; i < this->count; i++) {

            // If globally off, remove
            if( !global_active[i % (Data::IVS.size()+1)] )
                continue;

            potential_params.push_back(i);

        }

        //cout << "potential params: " << potential_params.size() << endl;
        // We can only mutate if we have a valid variable we can turn on
        //if(potential_params.empty()) {
        //    log << "Soft Mutation. Potential Params to turn on is empty" << endl;
        //    return;
        //}
            
            
        // Select a random parameter
        size_t param_pos = memetico::RANDINT(0, potential_params.size()-1);

        //cout << "selected param " << param_pos << endl;
        // Update to the real index
        param_pos = potential_params[param_pos];

        //cout << "param pos " << param_pos << endl;
        // Toggle the parameter on/off
        this->active[param_pos] = !this->active[param_pos];

        
        //log << "Soft Mutation. IV: ";
        //bool is_const = this->is_constant(param_pos);
        //if( is_const )  log << "constant";
        //else            log << Data::IVS[param_pos % (Data::IVS.size()+1)];

        //log << " Term: " << floor(param_pos/(Data::IVS.size()+1)) << " Now Active? " << boolalpha << global_active[param_pos] << "  Value: " << this->params[param_pos] << endl;
        
    }

    //cout << "Ending Mutate" << endl;
    return;
    */
}

/**
 * @brief Determine the number of parameters utilised by the ContinuedFraction
 * 
 * Any variable that appears at least once on any term is counted as 1
 * 
 * @return size_t
 * @bug our penalty requires the params_used() function. We softly penalise with the current implementation
 *      as we add the memetico::PENALTY for each time a varaible occurs but we don't consider how many times
 *      it appears down the term. i.e. if it appears once in a 10 term fraction or if it appears 10 times.
 *      We should consider strategies for making this more intelligent
 */
template <class T>
size_t ContinuedFraction<T>::params_used() {

    size_t ret = 0;

    /*
    // Technically defaults all items to false
    vector<bool> param_appears(Data::IVS.size()+1);

    
    size_t param_pos = 0;
    for(size_t i = 0; i < terms; i++) {

        for(size_t j = 0; j < Data::IVS.size()+1; j++) {

            // We force the parameter as true if it exists on any term
            if( this->active[param_pos] && 
                this->params[param_pos] != 0 &&
                !this->is_constant(param_pos)
                ) {
                //cout << "Param used at term: " << i << " "
                param_appears[j] = true;
            }

            param_pos++;
        }
    }

    // Get the number of appearances
    for(size_t i = 0; i < param_appears.size(); i++) {
        if(param_appears[i])
            ret++;
    }   
    */ 
    return ret;
    
}

/**
 * @brief Output operator for ContinuedFraction<T>
 * We ouput the fraction in the form 
 *      g0(x)+
 *          h0(x)/(g1(x)+
 *              h1(x)/g2(x)+..
 * 
 * Where each gi(x), hi(x) is a Regression in the form
 * 'coeff_val1*(var_name1)+coeff_val2*(var_name2)+...+coeff_valN*(var_nameN)+c'
 * 
 * @return os
 *           
 */
template <class T>
ostream& operator<<(ostream& os, ContinuedFraction<T>& c) {

    size_t close = 0;
    for( size_t i = 0; i < c.get_terms(); i++) {

        // For odd terms place the diver to make it the next denominator
        if( i % 2 == 0 && i != 0) {
            os << "/(";
            close++;
        }

        // Print term in braces
        os << "(" << *c.get_objs(i) << ")";

        // Add plus for even terms exluding the closing term
        if( (i % 2 == 0 || i == 0) && i != c.get_terms()-1 )
            os << "+";

    }
    
    // Close the open braces
    for(size_t i = 0; i < close; i++)
        os << ")";

    return os;

}

/** 
 * @brief Ouput the ContinuedFraction parameter names and values
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
void ContinuedFraction<T>::show(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type) {

    /*
    size_t temp_precision = out.precision();
    out.precision(precision);

    size_t term = 0;        // Term we are on
    string term_name;       // Term char i.e. g or h
    string term_name_no;    // Term num i.e. the i in gi(x) or hi(x)
    size_t depth = 0;       // Depth we are on
    string param_name;      // Name of param
    
    // Determine max length of parameter names and add a little more padding for presentation
    size_t max_name = 0;
    for( size_t i = 0 ; i < this->count; i++ ) {
        if( max_name < Data::IVS.size() )
            max_name = Data::IVS.size();
    }
    max_name += 6;

    // Set the maximum parameter value length with some padding
    size_t max_param = precision + 9;

    // Print header
    out << "Fraction Parameters" << endl;
    show_min(out,precision,from_param);
    out << endl;
    out << setw(10) << "Depth" << setw(10) << "Term" << setw(10) << "Name" << setw(10) << "Index"  <<  setw(max_name) << "Param" << setw(max_param) << "Value" << setw(10) << "Active?" << endl;
    out << "-------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

    // For all parameters
    for(size_t i = 0; i < this->count; i++) {

        term = floor(i/params_per_term);
        depth = ceil(term/2);
   
        if( term == 0 )     term_name_no = "0";
        else                term_name_no = to_string( int(ceil(term/2)) );

        // Determine term name
        if( term % 2 == 0)  term_name = "g";
        else                term_name = "h";

        // Determine param name
        if(this->is_constant(i))    param_name = "const";
        else                        param_name = Data::IVS[ i % (Data::IVS.size()+1) ];

        out << setw(10) << depth << setw(10) << term << setw(10) << term_name+term_name_no+"(x)" << setw(10) << to_string(i) << setw(max_name) << param_name << setw(max_param) << this->params[i] << boolalpha << setw(10) << this->active[i] << endl;        

    }
    out << endl;

    out << "Global Active" << endl;
    for(size_t i = 0; i < Data::IVS.size(); i++ )
        out << setw(max_name) << Data::IVS[i] << boolalpha << setw(10) << this->global_active[i] << endl;

    out.precision(temp_precision);
    */
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
void ContinuedFraction<T>::show_min(ostream& out, size_t precision, size_t from_param, memetico::PrintType print_type, bool do_summary) {

    /*
    size_t temp_precision = out.precision();
    out.precision(precision);

    // Loop along the fraction, as we dont need to evaluate we go from the start
    // Print first term
    Regression<T>::show_min(out, precision,0,print_type);

    // Print a depth at a time
    for(size_t i = params_per_term; i < this->count; i+=(params_per_term*2) ) {
        out << "+";
        out << "("; Regression<T>::show_min(out, precision, i, print_type); out << ")/(";
        Regression<T>::show_min(out, precision, i+params_per_term, print_type); 
    }

    // Add closing braces
    for( size_t i = 0; i < depth; i++ )
        out << ")";
    
    if(do_summary) {
        out << endl;
        Model<T>::show_min(out, precision, 0, print_type);
    }

    out.precision(temp_precision);
    */
}

template <class T>
void ContinuedFraction<T>::set_global_active(size_t iv, bool val) {

    // Make sure iv is within the range of paramters
    if(iv < params_per_term) {

        // Update global flag
        global_active[iv] = val;

        // Update all term flags
        for(size_t term = 0; term < terms; term++)
            objs[term]->set_active(iv, val);

    }   

}

template <class T>
double ContinuedFraction<T>::get_param(size_t pos) {

    size_t term = floor(pos/params_per_term);
    size_t param = pos % params_per_term;
    return objs[term]->get_param(param);

}

template <class T>
bool ContinuedFraction<T>::get_active(size_t pos) {

    size_t term = floor(pos/params_per_term);
    size_t param = pos % params_per_term;
    return objs[term]->get_active(param);

}

template <class T>
void ContinuedFraction<T>::set_param(size_t pos, double val) {

    size_t term = floor(pos/params_per_term);
    size_t param = pos % params_per_term;
    objs[term]->set_param(param, val);

}

template <class T>
vector<size_t> ContinuedFraction<T>::get_active_positions() {

    vector<size_t> ret;
    for( size_t term = 0; term < terms; term++ ) {
        for( size_t param = 0; param < params_per_term; param++ ) {

            if( objs[term]->get_active(param) )
                ret.push_back( term*params_per_term+param );
        }
    }
    return ret;

}

template <class T>
void ContinuedFraction<T>::recombine(Model<T>* model1, Model<T>* model2) {

    int method = memetico::RANDINT(0,2);

    for(size_t term = 0; term < terms; term++) {

        ContinuedFraction<T>* m1 = static_cast<ContinuedFraction<T>*>(model1);
        ContinuedFraction<T>* m2 = static_cast<ContinuedFraction<T>*>(model2);
        objs[term]->recombine( m1->get_objs(term) , m2->get_objs(term), method);
    }
        
}