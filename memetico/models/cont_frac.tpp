
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief Implementation of the Continued Fraction Regression class
*/

template <class T>
ContinuedFraction<T>::ContinuedFraction(size_t frac_depth) : MemeticModel<T>() {
    
    // Default values
    depth = frac_depth;
    frac_terms = 2*depth+1;
    params_per_term = Data::IVS.size()+1;

    objs = new T*[frac_terms];
    for(size_t i = 0; i < frac_terms; i++)
        objs[i] = new T(params_per_term);

    // Additional Allocation
    global_active = new bool [params_per_term];
    
    randomise();
    sanitise();

}

template <class T>
ContinuedFraction<T>* ContinuedFraction<T>::clone() {

    ContinuedFraction<T>* o = new ContinuedFraction<T>();
    
    o->frac_terms = frac_terms;
    o->depth = depth;
    o->params_per_term = params_per_term;

    for(size_t i = 0; i < get_frac_terms(); i++) {
        delete o->objs[i];
        o->objs[i] = objs[i]->clone();
        o->global_active[i] = get_global_active(i);
    }

    o->set_penalty(this->get_penalty());
    o->set_error(this->get_error());
    o->set_fitness(this->get_fitness());

    return o;

}

template <class T>
void ContinuedFraction<T>::randomise(int min, int max) {

    // For all parameters
    for( size_t param = 0; param < params_per_term; param++ ) {
        
        // Randomly turn the IV on/off, always set the constant on
        if( memetico::RANDREAL() < (2.0/3.0) || params_per_term-1 == param )
            set_global_active(param, true);
        else                               
            set_global_active(param, false);

    }

    // Randomise all parameters in all frac_terms, this will respect the active status
    for(size_t term = 0; term < get_frac_terms(); term++)
        objs[term]->randomise(min, max);
    
}

template <class T>
void ContinuedFraction<T>::sanitise() {

    // Check all terms
    for(size_t term = 0; term < get_frac_terms(); term++) {

        // if no terms are active on the fraction, at least set the constant on
        if( objs[term]->get_count_active() == 0 ) {
            
            //cout << *objs[term] << " Cnt: " << objs[term]->get_count_active() << " Val: " << objs[term]->get_param(params_per_term-1) << endl;
            objs[term]->set_active(params_per_term-1, true);

            if( objs[term]->get_param(params_per_term-1) == 0 )
                objs[term]->set_param(params_per_term-1,1);

            //cout << *objs[term] << " Cnt: " << objs[term]->get_count_active() << " Val: " << objs[term]->get_param(params_per_term-1) << endl;
        }
    }
}

template <class T>
double ContinuedFraction<T>::evaluate(double* values) {

    double ret = 0;

    // Catch all under/overflows that occur
    try {

        // For all terms, starting from the closing term
        for(int term = get_frac_terms()-1; term > -1; term -= 2 )

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

template <class T>
void ContinuedFraction<T>::mutate(MemeticModel<T>* model) {

    size_t param_pos;

    // Hard mutate when the fitness difference to the pocket is extreme (small or large)
    if( this->get_fitness() < 1.2 * model->get_fitness() || this->get_fitness() > 2* model->get_fitness() ) {

        // Select uniformly at random an independent variable (not a constant)
        param_pos = memetico::RANDINT(0, Data::IVS.size()-1);
        
        // Toggle it globally
        set_global_active( param_pos, !get_global_active(param_pos) );

        // If the variable is globally active
        if(get_global_active(param_pos)) {

            // With 50% chance randomise down the fraction with a non-zero value
            for(size_t i = 0; i < get_frac_terms(); i++ )
                objs[i]->randomise(ContinuedFraction<T>::RAND_LOWER, ContinuedFraction<T>::RAND_UPPER);

        }
    }
    // Soft mutate when the difference is small
    else {

        // Identify parameters that we can switch on i.e. not globally inactive
        vector<size_t> potential_params = get_active_positions();
        
        // Ensure there are params to change
        if(potential_params.size() == 0) {
            sanitise();
            return;
        }

        // Select a random parameter
        size_t param_pos = memetico::RANDINT(0, potential_params.size()-1);

        // Toggle value 
        set_active(param_pos, !get_active(param_pos));
        
    }

    // Make sure we have a fraction
    size_t global_on = 0;
    for(size_t i = 0; i < params_per_term; i++) {
        if(get_global_active(i))
            global_on++;
    }
    if( global_on == 0 )
        randomise(ContinuedFraction<T>::RAND_LOWER, ContinuedFraction<T>::RAND_UPPER);
   
    sanitise();
    return;
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
    for( size_t i = 0; i < c.get_frac_terms(); i++) {

        // For odd terms place the diver to make it the next denominator
        if( i % 2 == 0 && i != 0) {
            os << "/(";
            close++;
        }

        // Print term in braces
        os << "(" << *c.get_objs(i) << ")";

        // Add plus for even terms exluding the closing term
        if( (i % 2 == 0 || i == 0) && i != c.get_frac_terms()-1 )
            os << "+";

    }
    
    // Close the open braces
    for(size_t i = 0; i < close; i++)
        os << ")";

    return os;

}

template <class T>
void ContinuedFraction<T>::recombine(MemeticModel<T>* model1, MemeticModel<T>* model2) {

    int method = memetico::RANDINT(0,2);

    ContinuedFraction<T>* m1 = static_cast<ContinuedFraction<T>*>(model1);
    ContinuedFraction<T>* m2 = static_cast<ContinuedFraction<T>*>(model2);

    // Minimum from m1/m2
    size_t min_terms = min(m1->get_frac_terms(), m2->get_frac_terms());

    // Minimum of the above with the current object
    min_terms = min(min_terms, get_frac_terms());

    for(size_t term = 0; term < min_terms; term++)
        objs[term]->recombine( m1->get_objs(term) , m2->get_objs(term), method);

    sanitise();

}