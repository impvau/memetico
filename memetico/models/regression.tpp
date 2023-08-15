
/**
 * @file
 * @brief See regression.h
 */

template <class T>
void Regression<T>::coeff_node(TreeNode * tree_node, float constant, int var_num) {

    tree_node->node.node_type = NodeType::BFUNC;
    tree_node->node.function = Function::MUL;
                
    tree_node->left = new TreeNode();
    tree_node->left->node.node_type = NodeType::CONST;
    tree_node->left->node.constant = constant;

    tree_node->right = new TreeNode();
    tree_node->right->node.node_type = NodeType::VAR;
    tree_node->right->node.variable = var_num;

}

template <class T>
void Regression<T>::get_node(TreeNode * n) {
    
    vector<size_t> term_active_variables = get_active_positions();

    TreeNode * temp_node = n;
    if(term_active_variables.size() > 1) {

        // For each pair of  
        for(size_t j = 0; j < term_active_variables.size()-1; j++) {

            if( j != 0 ) {
                temp_node->right = new TreeNode();
                temp_node = temp_node->right;
            }

            temp_node->node.node_type = NodeType::BFUNC;
            temp_node->node.function = Function::ADD;
            temp_node->left = new TreeNode();
            coeff_node( temp_node->left, get_value(term_active_variables[j]), term_active_variables[j] );
            
        }

        // Get last index
        size_t j = term_active_variables.back();

        // if constant
        if( term_active_variables.back() == get_count()-1 ) {
            temp_node->right = new TreeNode();
            temp_node->right->node.node_type = NodeType::CONST;
            temp_node->right->node.constant = get_value(j);
        // Else if coeff
        } else {
            temp_node->right = new TreeNode();
            coeff_node(temp_node->right, get_value(j), j );
        }

    } else if( term_active_variables.size() == 1) {

        // Get last index
        size_t j = term_active_variables.back();

        // if constant
        if( term_active_variables.back() == get_count()-1 ) {
            n->node.node_type = NodeType::CONST;
            n->node.constant = get_value(j);
        // Else if coeff
        } else {
            coeff_node(n, get_value(j), j );
        }
    } else {
            n->node.node_type = NodeType::CONST;
            n->node.constant = 1;
    }

}

template <class T>
void Regression<T>::mutate(MemeticModel<T> & m) {

    // Select any parameter at random (not size()+1 as RANDINT is inclusive)
    size_t param_pos = RandInt::RANDINT->rand(0, get_count()-1);
    set_active(param_pos, !get_active(param_pos));

}

template <class T>
void Regression<T>::recombine(MemeticModel<T> * m1, MemeticModel<T> * m2, int method_override) {

    // Select recombine type; intersection, union and xor at random
    int method;
    if( method_override > -1)   method = method_override;
    else                        method = RandInt::RANDINT->rand(0,2);

    // For each parameter
    for(size_t i = 0; i < get_count(); i++) {

        bool temp_active;

        // Set active flag based on rebomination
        switch(method) {
            case 0:
                temp_active = m1->get_active(i) | m2->get_active(i);
                set_active(i, temp_active); 
                break;       
            case 1:
                temp_active = m1->get_active(i) & m2->get_active(i);
                set_active(i, temp_active); 
                break;       
            case 2:
                temp_active = m1->get_active(i) ^ m2->get_active(i);
                set_active(i, temp_active); 
                break;      
            default:    cout << "Could not determine recombine method" << endl;
        }

        // If the parameter is active, detemine the coefficient based on the parents
        if(get_active(i)) {

            if(m1->get_active(i) && m2->get_active(i)) {
                int rand = RandInt::RANDINT->rand(-1,4);                
                T temp = m1->get_value(i) + rand * (m2->get_value(i) - m1->get_value(i)) / 3;
                set_value(i, temp);
            } else if (m1->get_active(i)) {
                T temp = m1->get_value(i);
                set_value(i, temp);
            } else if (m2->get_active(i)) {
                T temp = m2->get_value(i);
                set_value(i, temp);
            }
        }
    }   
}

template <class T>
void Regression<T>::randomise(int min, int max, int pos) {

    if( pos > -1 && get_active(pos)) {

        //  Turn on with even chance of positive/negative value
        if( RandReal::RANDREAL->rand() < 0.5 ) {
            double rand = RandInt::RANDINT->rand(min, max);
            set_value(pos, rand);
        } else {                            
            double rand = RandInt::RANDINT->rand(max*-1, min*-1);
            set_value(pos, rand);
        }

    } else {

        // For all paramters in the Regression
        for(size_t param = 0; param < get_count(); param++) {
            
            // if the parameter is active
            if(get_active(param)) {

                //  Turn on with even chance of positive/negative value
                if( RandReal::RANDREAL->rand() < 0.5 ) {
                    double rand = RandInt::RANDINT->rand(min, max);
                    set_value(param, rand);
                } else {                              
                    double rand = RandInt::RANDINT->rand(max*-1, min*-1);
                    set_value(param, rand);
                }
            
            }
        }
    }
}

template <class T>
double Regression<T>::evaluate(vector<double> & values) {
    
    // Add each parameter
    double ret = 0;
    for(size_t param = 0; param < get_count()-1; param++ ) {
        if( get_active(param) )
            ret = add(ret, multiply( get_value(param), values[param]));
    }
    
    // Add constant
    if( get_active(get_count()-1) ) ret = add(ret, get_value(get_count()-1));
    
    return ret;
}

template <class F>
ostream& operator<<(ostream& os, Regression<F>& r)
{
            
    bool has_written = false;

    // For all parameters
    for(size_t i = 0; i < r.get_count(); i++) {

        // If active and non-zero
        if( r.get_active(i) && r.get_value(i) != 0) {

            // Determine if to print the sign
            // - Negative is always printed with the value, but we dont print the value for -1, so we must print the sign
            // - Positive is never printed with the value (we must), but we dont want to print the + when no terms exist on the left
            // - Constant is alway shown

            // Write plus when a variable has been written and larger than non-zero
            if( has_written && r.get_value(i) > 0)
                os << "+";

            // Write negative when not a constant and the value is -1
            // Otherwise print sign is managed by the parameter printout
            if( i != r.get_count()-1 && r.get_value(i) == -1)
                os << "-";

            // Write the coefficient when it is a non-zero constant, or non-one |coefficient|
            if( i == r.get_count()-1 || abs(r.get_value(i)) != 1 ) {

                os << r.get_value(i);
                // If not a constant, add the multiplier
                if( i != r.get_count()-1 ) {
                    if( Regression<F>::FORMAT != PrintLatex ) 
                        os << "*";
                }
                    
            }
            
            // Write the variable name
            if(i != r.get_count()-1) {

                if( MemeticModel<F>::FORMAT == PrintLatex ) {

                    string s = MemeticModel<F>::IVS[i];
                    regex pattern("^x(\\d{1,4})$");
                    smatch match;
                    if( regex_match(s, match, pattern) ) {
                        int value = std::stoi(match[1].str());
                        s = "x_{" + std::to_string(value) + "}";
                    }
                    os << s;
                }

                if( MemeticModel<F>::FORMAT == PrintNumpy)
                    os << MemeticModel<F>::IVS[i];
                
                if( MemeticModel<F>::FORMAT == PrintExcel ) {
                    os << excel_name(i+2);
                }

            }

            // Indicate we have written a varaible
            has_written = true;
            
        }
    }
    return os;
}
