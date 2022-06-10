
/**
 * @file
 * @brief See regression.h
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

template <class T>
Regression<T>* Regression<T>::clone() {

    // Create new structure
    Regression<T>* o = new Regression<T>(0);

    // Copy attributes
    o->set_penalty(this->get_penalty());
    o->set_error(this->get_error());
    o->set_fitness(this->get_fitness());

    delete o->term;
    o->term = this->term->clone();
    
    return o;

}

template <class T>
void Regression<T>::mutate(MemeticModel<T>* m) {

    // Select any parameter at random (not size()+1 as RANDINT is inclusive)
    size_t param_pos = memetico::RANDINT(0, get_count()-1);
    set_active(param_pos, !get_active(param_pos));

}

template <class T>
void Regression<T>::recombine(MemeticModel<T>* m1, MemeticModel<T>* m2, int method_override) {

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

template <class T>
void Regression<T>::randomise(int min, int max, int pos) {

    if( pos > -1 ) {

        //  Turn on with even chance of positive/negative value
        if( memetico::RANDREAL() < 0.5 )
            set_param(pos, memetico::RANDINT(min, max));
        else                               
            set_param(pos, memetico::RANDINT(max*-1, min*-1));

    } else {

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
}
