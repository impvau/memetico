
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief Implementation of the Continued Fraction Regression class
*/

template <class T>
ContinuedFraction<T>::ContinuedFraction(size_t frac_depth) : MemeticModel<T>() {
    
    // Default values
    depth = frac_depth;
    frac_terms = 2*depth+1;
    params_per_term = ContinuedFraction<T>::IVS.size()+1;

    for(size_t i = 0; i < params_per_term; i++)
        global_active.push_back(false);

    for(size_t i = 0; i < frac_terms; i++)
        terms.push_back(Regression<T>(params_per_term));
    
    randomise();
    sanitise();

}

template <class T>
ContinuedFraction<T>::ContinuedFraction(const ContinuedFraction<T> &o) : MemeticModel<T>(o) {

    depth = o.get_depth();
    frac_terms = o.get_frac_terms();
    params_per_term = o.get_params_per_term();

    for(size_t i = 0; i < params_per_term; i++)
        global_active.push_back( o.get_global_active(i) );

    for(size_t i = 0; i < o.frac_terms; i++) {
        Regression<T> temp = Regression<T>(o.terms[i]);
        terms.push_back(temp);
    }

}

template <class T>
void ContinuedFraction<T>::randomise(int min, int max) {

    // For all parameters
    for( size_t param = 0; param < params_per_term; param++ ) {

        // Randomly turn the IV on/off, always set the constant on
        if( RandReal::RANDREAL->rand() < (2.0/3.0) || params_per_term-1 == param )
            set_global_active(param, true);
        else                               
            set_global_active(param, false);

    }

    // Randomise all parameters in all frac_terms, this will respect the active status
    for(size_t term = 0; term < get_frac_terms(); term++)
        terms[term].randomise(min, max);
    
}

template <class T>
void ContinuedFraction<T>::sanitise() {

    // Check all terms
    for(size_t term = 0; term < get_frac_terms(); term++) {

        // if no terms are active on the fraction, at least set the constant on
        // This could occur by soft mutation where the constant is turned off
        if( terms[term].get_count_active() == 0 ) {
            
            terms[term].set_active(params_per_term-1, true);

            // If the constant is 0, make it 1 to cause as little impact as possible
            // e.g. if its a denominator term is it /1 rather than .../0
            // if it is numnerator it is 1/... instead of 0/...
            double safe_value = 1;
            terms[term].set_value(params_per_term-1, safe_value);

        }
    }
}

template <class T>
double ContinuedFraction<T>::evaluate(vector<double>& values) {

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
                ret = divide(
                    terms[term-1].evaluate(values),
                    add(
                        terms[term].evaluate(values),
                        ret
                    )
                );
            } else
                ret = add(ret, terms[term].evaluate(values));

    } catch (exception& e) { 
        return numeric_limits<double>::max();
    }

    return ret;
    
}

template <class T>
void ContinuedFraction<T>::mutate(MemeticModel<T>& model) {

    size_t param_pos;

    // Hard mutate when the fitness difference to the pocket is extreme (small or large)
    if( this->get_fitness() < 1.2 * model.get_fitness() || this->get_fitness() > 2* model.get_fitness() ) {

        // Select uniformly at random an independent variable (not a constant)
        param_pos = RandInt::RANDINT->rand(0, ContinuedFraction<T>::IVS.size()-1);
        
        // Toggle it globally
        set_global_active( param_pos, !get_global_active(param_pos) );

        // If the variable is globally active
        if(get_global_active(param_pos)) {

            // With 50% chance randomise down the fraction with a non-zero value
            for(size_t i = 0; i < get_frac_terms(); i++ )
                terms[i].randomise(ContinuedFraction<T>::RAND_LOWER, ContinuedFraction<T>::RAND_UPPER);

        }
    }
    // Soft mutate when the difference is small
    else {

        // Identify parameters that we can switch on i.e. not globally inactive
        vector<size_t> potential_params = get_active_positions();
        
        // Ensure there are params to change
        if(potential_params.size() == 0) {
            //sanitise();
            return;
        }

        // Select a random parameter
        size_t param_pos = RandInt::RANDINT->rand(0, potential_params.size()-1);

        // Toggle value 
        set_active(potential_params[param_pos], !get_active(potential_params[param_pos]));
        
    }

    this->set_fitness(numeric_limits<double>::max());
    this->set_error(numeric_limits<double>::max());

    sanitise();
    return;
}

template <class T>
void ContinuedFraction<T>::get_node(TreeNode * parent) {
   
    TreeNode * temp_node;
    TreeNode * save_node = nullptr;

    // We work from the bottom of the tree up
    for(size_t i = get_frac_terms()-1; i > 1; i = i-2) {

        // If last loop, save into the parent
        if( !(i-2 > 1) )
            temp_node = parent;
        // Else save into a new temporary tree that will become right child of parent eventually
        else 
            temp_node = new TreeNode();

        // Set up the parent
        temp_node->node.node_type = NodeType::BFUNC;
        temp_node->node.function = Function::ADD;

        // Parent right is the divisor
        temp_node->right = new TreeNode();
        temp_node->right->node.node_type = NodeType::BFUNC;
        temp_node->right->node.function = Function::DIV;
        // Numerator
        temp_node->right->left = new TreeNode();
        terms[i-1].get_node(temp_node->right->left);

        // Denominator
        if(save_node == nullptr) {
            temp_node->right->right = new TreeNode();
            terms[i].get_node(temp_node->right->right);
        } else
            temp_node->right->right = save_node;
        

        // Addition term
        temp_node->left = new TreeNode();
        terms[i-2].get_node(temp_node->left);

        // Save pointer to progress
        save_node = temp_node;
    }
  
    if( get_frac_terms() == 1)
        terms[0].get_node(parent);

}

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
        os << "(" << c.get_terms(i) << ")";

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
void ContinuedFraction<T>::recombine(MemeticModel<T>* model1, MemeticModel<T>* model2, int method_override) {

    int method = RandInt::RANDINT->rand(0,2);

    ContinuedFraction<T>* m1 = static_cast<ContinuedFraction<T>*>(model1);
    ContinuedFraction<T>* m2 = static_cast<ContinuedFraction<T>*>(model2);

    // Minimum from m1/m2
    size_t min_terms = min(m1->get_frac_terms(), m2->get_frac_terms());

    // Minimum of the above with the current object
    min_terms = min(min_terms, get_frac_terms());

    for(size_t term = 0; term < min_terms; term++)
        terms[term].recombine( &(m1->get_terms(term)), &(m2->get_terms(term)), method);

    sanitise();

    this->set_fitness(numeric_limits<double>::max());
    this->set_error(numeric_limits<double>::max());

}

template <class T>
vector<size_t>  ContinuedFraction<T>::get_active_positions() {

    vector<size_t> ret;
    
    for( size_t i = 0; i < get_frac_terms(); i++ ) {

        // Get positions relative to the term
        vector<size_t> term_poss = terms[i].get_active_positions();

        // Offset to the point in the fraction, 
        // for term 0 = Offset 0, for term 1 = Offset params_per_term
        // for term 2 = Offtset 2*params_per_term, etc.
        transform(term_poss.begin(), term_poss.end(), term_poss.begin(), 
                    [&](auto& value){ return value+params_per_term*i; });

        // Append term positions to return vector
        ret.insert( ret.end(), term_poss.begin(), term_poss.end());

    }

    return ret;
}

template <class T>
bool ContinuedFraction<T>::operator== (ContinuedFraction<T>& o) {

    if( !MemeticModel<T>::operator==(o) )
        return false;

    // Check depth
    if( depth != o.depth )
        return false;

    // Check frac terms
    if( frac_terms != o.frac_terms) 
        return false;

    // Params per term checks
    if( params_per_term != o.params_per_term)
        return false;

    // Global active checks
    if( global_active.size() != o.global_active.size() )
        return false;

    for(size_t i = 0 ; i < global_active.size() ; i++) {
        if( global_active[i] != o.global_active[i] )
            return false;
    }

    // Term checks
    if( terms.size() != o.terms.size( ))
        return false;

    for(size_t i = 0; i < terms.size(); i++ ){
        if( !(terms[i] == o.terms[i]) )
            return false;
    }
    
    // Else we match!
    return true;
}

template <class T>
void ContinuedFraction<T>::set_depth(size_t new_depth) {

    size_t  new_frac_terms = 2*new_depth+1;

    // If we are reducing terms, simply remove the number of terms from the vector
    if( new_frac_terms < frac_terms) {

        size_t excess_terms = frac_terms-new_frac_terms;
        for(size_t i = 0; i < excess_terms; i++)
            terms.pop_back();

    // If we are adding terms, we need to increase terms
    } else {

        size_t new_terms = new_frac_terms-frac_terms;
        for(size_t i = 0; i < new_terms; i++) {
            Regression<T> new_term  = Regression<T>(params_per_term);
            new_term.randomise(ContinuedFraction<T>::RAND_LOWER, ContinuedFraction<T>::RAND_UPPER);
            terms.push_back(new_term);
        }

    }

    depth = new_depth;
    frac_terms = new_frac_terms;

}

template <class T>
void ContinuedFraction<T>::set_global_active(size_t iv, bool val, bool align_fraction) {

    // Make sure iv is within the range of paramters
    if(iv < params_per_term) {

        // Update global flag
        global_active[iv] = val;

        if( align_fraction ) {

            // Update all term flags
            for(size_t term = 0; term < get_frac_terms(); term++)
                terms[term].set_active(iv, val);
        }
    }  
};
