
/** @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief A Term of many Elements
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_TERM_H
#define MEMETICO_MODELS_TERM_H

using namespace std;

// Local
#include <memetico/model_base/element.h>

// Std Lib
#include <vector>
#include <iostream>
#include <cstddef>

/**
 * @brief A Term that contains many Elements
 * 
 * An Term consists of an array of T Elements and an associated count
 * 
 * Example usage is within the Regression class that contains a single term \f$ c_1x_1+c_2x_2..+c_Nx_N \f$
 * Where each coefficient and variable pair represent an Element
 * 
 * The template class prepares the Regression object to take more than simple types such as int and double.
 * We are able to utilise a Term another form e.g. \f$ sin(c_1x_1+...)e^{c_1x_1+...}+cos(c_1x_1+...)e^{c_1x_1+...} \f$
 */
template<class T>
class AdditiveTerm {
        
    public:

        /** @brief Construct an Additive with element_count default Elements */
        AdditiveTerm(size_t element_count = 0) { 
            
            // Create count number of empty elements
            for(size_t i = 0; i < element_count; i++) {
                elems.push_back(Element<T>());
            }
        }

        /** @brief Construct a copy of o in this */
        AdditiveTerm(const AdditiveTerm<T> &o) {

            // Copy element between Additive Terms
            for(size_t i = 0; i < o.elems.size(); i++)
                elems.push_back( o.elems[i] );
            
        }

        /** @brief Return reference to the pos \a th Element of the AdditiveTerm @param pos */
        Element<T>&     get_elem(size_t pos)                    {return elems[pos]; }

        /** @brief Copy o to the pos \a th position in the AdditiveTerm @param o */
        void            set_elem(size_t pos, Element<T>& o)     {elems[pos] = o; }

        /** @brief Return number of elements */
        size_t          get_count()                             {return elems.size();}

        /** @brief Return the number of active elements */
        size_t          get_count_active() { 
            size_t ret = 0;
            for(size_t i = 0; i < get_count(); i++) {
                if( get_elem(i).get_active() ) {
                    ret += 1;
                }
            }
            return ret;
        };

        /** @brief Return list of element positions containing active variables */
        vector<size_t>  get_active_positions() {
            
            vector<size_t> ret;
            for(size_t i = 0; i < get_count(); i++) {
                if( get_elem(i).get_active() ) {
                    ret.push_back(i);
                }
            }
            return ret;

        }

        // Functions on Element objects

        /** @brief Set the Element at \a pos to \a elem  */
        void            set_value(size_t pos, T value)          { elems[pos].set_value(value); }

        /** @brief Set the active flag for Element at \a pos to \a active */
        void            set_active(size_t pos, bool active)     { elems[pos].set_active(active); }

        /** @brief Return the value within Element at \a pos */
        T               get_value(size_t pos)                   { return elems[pos].get_value(); }

        /** @brief Return the active flag within Element at \a pos */
        bool            get_active(size_t pos)                  { return elems[pos].get_active(); }

        /** @brief comparison operator for AdditiveTerm */
        bool operator== (AdditiveTerm& o) {
            
            if( get_count() != o.get_count() )
                return false;

            // Check each Element and exist if there is a mismatch
            for( size_t i = 0; i < o.get_count(); i++) {
                if(elems[i] != o.get_elem(i))
                    return false;
            }

            // Else all is matching
            return true;
        }

        /** @brief Output AdditiveTerm */
        friend ostream& operator<<(ostream& os, AdditiveTerm<T>& o) {

            vector<size_t> list = o.get_active_positions();
            
            for( size_t i = 0; i < list.size(); i++ ) {

                // Only if non-zero
                if(o.get_value(list[i]) != 0) {

                    // Where not the first to display, we need an addition sign
                    if( i != 0 ) {

                        // Only display plus when next element is positive, negative auto displayed
                        if( o.get_value(list[i]) > 0 )
                            os << "+";
                    }

                    // print coeff
                    os << o.elems[list[i]];

                    // if not constant, times by x<index>
                    if( list[i] != o.get_count()-1 )
                        os << "*x" << list[i];

                }

            }

            return os;
        }

    private:

        /** Array of Elements **/
        vector<Element<T>>      elems;

};


#endif
