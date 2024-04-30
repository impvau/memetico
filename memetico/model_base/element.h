
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief A class representing an Element in a Regression model, which may be active or inactive and has an associated value of type T.
 */

#ifndef MEMETICO_MODELS_ELEMENT_H_
#define MEMETICO_MODELS_ELEMENT_H_

using namespace std;

#include <stdlib.h>

/**
 * @brief A class representing an Element in a Regression model, which may be active or inactive and has an associated value of type T.
 * The Regression class provides an example usage of the Element class, where it contains a vector of Element<T> objects.
 * In the regression form of a model, each term is represented as an Element, where each Element is either active or inactive and has a value of type T.
 * When we consider the form \f$ f(x)=c_1x_1+c_2x_2..+c_Nx_N \f$, has each term is considered as \f$c_ix_i\f$ and is modelled as an Element.
 * The Element class provides methods to set and get the active status of the element, as well as its value. It also provides overloaded operators for comparison and arithmetic operations on Element objects.
 */
template<class T>
class Element {

    public:

        /**
         * @brief Construct Element with value and active flag
         * @param element_active
         * @param element_value
         */
        Element(bool element_active = false, T element_value = 0) {
            active = element_active;
            value = element_value;
        };

        /** @brief Copy constructor */
        Element(const Element<T> &o) {
            value = T(o.value);
            active = o.active;
        };

        /** @brief Get Element value */
        T       get_value()     {return value;};

        /** @brief Get Element active flag */
        bool    get_active()    {return active;};

        /** @brief Set Element value */
        void    set_value(T element_value)      {value = element_value;};
        
        /** @brief Set Element active_flag */
        void    set_active(bool element_active) {active = element_active;};

        /** @brief Check value and active flag matches between this and o */
        bool operator== (const Element<T>& o)   {return (value == o.value && active == o.active);};

        /** @brief Check !(this == o) */
        bool operator!= (const Element<T>& o)   {return !(*this == o);};

        /** @brief Output value if active is true */
        friend ostream& operator<<(ostream& os, Element<T>& o) {  
            if( o.get_active() )    os << o.get_value();
            return os;
        }

    private:

        /** Value of the Element */
        T           value;

        /** Active/inactive state of the Element */
        bool        active;

};

#endif
