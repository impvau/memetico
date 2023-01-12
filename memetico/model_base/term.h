
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
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
class Term {
        
    public:

        /**
         * @brief Construct Term with a number of new T Elements
         * @param element_count
         */
        Term(size_t element_count) { 
            
            count = element_count; 

            // Create count number of empty elements
            elems = new Element<T>*[count];
            for(size_t i = 0; i < count; i++)
                elems[i] = new Element<T>();
        }

        /**
         * @brief Deconstruct Term and all created elements
         */
        ~Term() { 

            // Delete count number of elements
            for(size_t i = 0; i < count; i++)
                delete elems[i];

            // Delete array
            delete elems;

        };

        /**
         * @brief Get the \a pos th Element if it exists
         * @param pos the position of the Element
         * @return pointer to the element 
         */
        Element<T>*     get_elem(size_t pos)       {if(pos < count) return elems[pos]; else return nullptr;}

        /** @brief get the number of elements */
        size_t          get_count()                {return count;}

        /** @brief get the number of active elements */
        size_t          get_count_active() { 
            size_t ret = 0;
            for(size_t i = 0; i < get_count(); i++) {
                if( get_elem(i)->get_active() ) {
                    ret += 1;
                }
            }
            return ret;
        };

        /**
         * @brief set the Element at position \a pos to \a elem if there are at least \a pos number of elements
         * @param pos position of the Element
         * @param elem point to an Element 
         */
        void set_elem(size_t pos, Element<T>* elem) { 
            
            // Ensure there are pos elements
            if(pos < count) {

                // We have an Element already from the constructor, so remove
                delete elems[pos];

                // Then replace with new pointer
                elems[pos] = elem; 
            }
            
        };

        /** @brief comparison operator for Term */
        bool operator== (Term& o) {
            
            // Number of Elements must match
            if( count != o.count )  return false;

            // Check each Element and exist if there is a mismatch
            for( size_t i = 0; i < count; i++) {
                if(elems[i] != o.get_elem(i))
                    return false;
            }

            // Else all is matching
            return true;
        }

        /** 
         * @brief clone function for Term
         * @bug To change this to copy constructor
         */
        Term<T>* clone() {

            // Create new structure
            Term<T>* o = new Term<T>(this->count);
            
            for(size_t i = 0; i < this->count; i++)
                o->set_elem(i, this->get_elem(i)->clone() );
        
            return o;

        }

    private:

        /** Count of Elements in elems **/
        size_t          count;

        /** Array of Elements **/
        Element<T>**    elems;
        
};

#endif
