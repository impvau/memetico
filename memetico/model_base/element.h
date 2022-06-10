
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief An Element that occurs within a Term
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_ELEMENT_H_
#define MEMETICO_MODELS_ELEMENT_H_

using namespace std;

#include <memetico/globals.h>

/**
 * @brief A specific Element within within a Term of elements.
 * 
 * An Element consists of a value of type T and an active/inactive flag.
 * 
 * Example usage is within the Regression class that extends the Model class.
 * The Regression class has a Term with many Elements from its form \f$ f(x)=c_1x_1+c_2x_2..+c_Nx_N \f$
 * Where each \f$c_ix_i\f$ is an Element that may be on or off
 * 
 * The template class is provided such that each Element could be some other form such as \f$ f(x)={x_1}^{c_1}+... \f$
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
            set_active(element_active);
            set_value(element_value);
        };

        /** @brief get Element value */
        T           get_value()     {return value;};

        /** @brief get Element active flag */
        bool        get_active()    {return active;};

        /** @brief set Element value */
        void        set_value(T element_value)   {value = element_value;};
        
        /** @brief set Element active_flag */
        void        set_active(bool element_active)     {active = element_active;};

        /** @brief comparison operator for Element */
        bool operator== (const T& o) {
            return (value == o.value && active == o.active);
        }

        /** @brief not equal comparison operator for Element */
        bool operator!= (const T& o) {
            return !(this == o);
        }

        /** 
         * @brief clone function for Element
         * @bug To change this to copy constructor
         */
        Element<T>* clone() {   return new Element<T>(this->get_active(), this->get_value()); };

    private:

        /** Value of the Element */
        T           value;

        /** Active/inactive state of the Element */
        bool        active;

};

#endif
