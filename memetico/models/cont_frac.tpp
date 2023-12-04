
/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Implementation of the Continued Fraction Regression class
*/

template <typename Traits>
ContinuedFraction<Traits>::ContinuedFraction(size_t frac_depth) : MemeticModel<typename Traits::UType>() {
    
    // Default values
    depth = frac_depth;
    params_per_term = MemeticModel<typename Traits::UType>::IVS.size()+1;

    for(size_t i = 0; i < get_frac_terms(); i++)
        terms.push_back(typename Traits::TType(params_per_term));
    
    Traits::template MPType<typename Traits::UType, ContinuedFraction<Traits>>::initialise();
    
    sanitise();

}

template <typename Traits>
ContinuedFraction<Traits>::ContinuedFraction(const ContinuedFraction<Traits> &o) : MemeticModel<typename Traits::UType>(o) {

    depth = o.get_depth();
    params_per_term = o.get_params_per_term();

    for(size_t i = 0; i < o.get_frac_terms(); i++) {
        typename Traits::TType temp = typename Traits::TType(o.terms[i]);
        terms.push_back(temp);
    }

}

template <typename Traits>
void ContinuedFraction<Traits>::sanitise() {

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

template <typename Traits>
double ContinuedFraction<Traits>::evaluate(vector<double>& values) {

    double ret = 0;
    double An2 = 1.0;
    double An1 = terms[0].evaluate(values);
    double An = An1;
    double Bn2 = 0.0;
    double Bn1 = 1.0;
    double Bn = Bn1;
    for(int i=1; i<=get_depth(); i++){
        An = add(multiply(terms[2*i].evaluate(values),An1), multiply(terms[2*i-1].evaluate(values),An2));
        Bn = add(multiply(terms[2*i].evaluate(values),Bn1), multiply(terms[2*i-1].evaluate(values),Bn2));
        An2 = An1;
        An1 = An;
        Bn2 = Bn1;
        Bn1 = Bn;
    }
    try{
        ret = divide(An, Bn);
    }
    catch (exception& e) {
        cout << "exception in evaluate()" << endl;
        return numeric_limits<double>::max();
    }

  return ret;

}

template <typename Traits>
void ContinuedFraction<Traits>::get_node(TreeNode * parent) {
   
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

template <typename Traits>
void ContinuedFraction<Traits>::recombine(MemeticModel<typename Traits::UType>* model1, MemeticModel<typename Traits::UType>* model2, int method_override) {

    int method = RandInt::RANDINT->rand(0,2);

    ContinuedFraction<Traits>* m1 = static_cast<ContinuedFraction<Traits>*>(model1);
    ContinuedFraction<Traits>* m2 = static_cast<ContinuedFraction<Traits>*>(model2);

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

template <typename Traits>
vector<size_t>  ContinuedFraction<Traits>::get_active_positions() {

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

template <typename Traits>
bool ContinuedFraction<Traits>::operator== (ContinuedFraction<Traits>& o) {

    if( !MemeticModel<typename Traits::UType>::operator==(o) )
        return false;

    // Check depth
    if( depth != o.depth )
        return false;

    // Check frac terms
    if( get_frac_terms() != o.get_frac_terms()) 
        return false;

    // Params per term checks
    if( params_per_term != o.params_per_term)
        return false;

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

template <typename Traits>
void ContinuedFraction<Traits>::set_depth(size_t new_depth) {

    size_t  new_frac_terms = 2*new_depth+1;

    // If we are reducing terms, simply remove the number of terms from the vector
    if( new_frac_terms < get_frac_terms()) {

        size_t excess_terms = get_frac_terms()-new_frac_terms;
        for(size_t i = 0; i < excess_terms; i++)
            terms.pop_back();

    // If we are adding terms, we need to increase terms
    } else {

        size_t new_terms = new_frac_terms-get_frac_terms();
        for(size_t i = 0; i < new_terms; i++) {
            typename Traits::TType new_term  = typename Traits::TType(params_per_term);
            new_term.randomise();
            terms.push_back(new_term);
        }

    }

    depth = new_depth;

}

template <typename Traits2>
ostream& operator<<(std::ostream& os, ContinuedFraction<Traits2>& c) {

    if( Model::EXPRESSION == Recurrence ){ // case Model::EXPRESSION == Recurrence
    string An2 = "(1.0)";
    string An1 = "("+c.get_terms(0).str()+")";
    string An;
    string Bn2 = "(0.0)";
    string Bn1 = "(1.0)";
    string Bn;
    for(int i=1; i<=c.get_depth(); i++){
      An = "(("+c.get_terms(2*i).str()+")*"+An1+"+("+c.get_terms(2*i-1).str()+")*"+An2+")";
      Bn = "(("+c.get_terms(2*i).str()+")*"+Bn1+"+("+c.get_terms(2*i-1).str()+")*"+Bn2+")";
      An2 = An1;
      An1 = An;
      Bn2 = Bn1;
      Bn1 = Bn;
    }
    os << "("+An+")/("+Bn+")";
  } // end case Model::EXPRESSION == Recurrence  
  else{ // case Model::EXPRESSION == Naive
    size_t close = 0;
    for( size_t i = 0; i < c.get_frac_terms(); i++) {
        // For odd terms place the diver to make it the next denominator
        if( i % 2 == 0 && i != 0) {
            if( Model::FORMAT == PrintLatex )
                os << "}{(";
            else if( Model::FORMAT == PrintExcel)
                os << "/(";
            close++;
        }
        // Print term in braces
        os << "(" << c.get_terms(i) << ")";
        //if( Model::FORMAT == PrintLatex && i != c.get_frac_terms() )
        //  os << "}";
        // Add plus for even terms exluding the closing term
        if( (i % 2 == 0 || i == 0) && i != c.get_frac_terms()-1 ) {
            os << "+";
            if( Model::FORMAT == PrintLatex && i+1 < c.get_frac_terms() )
                os << "\\frac{";
        }
    }
    // Close the open braces
    for(size_t i = 0; i < close; i++) {
        if( Model::FORMAT == PrintLatex )
            os << ")}";
        else if( Model::FORMAT == PrintExcel)
            os << ")";
    }
  } // case Model::EXPRESSION == Naive
  
  return os;

}
