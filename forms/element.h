
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief An element that occurs within a term
 * 
 */

#ifndef FORMS_ELEMENT_H
#define FORMS_ELEMENT_H

using namespace std;

#include <memetico/globals.h>

/**
 * @brief A representation of a single element within a term
 * 
 */
template<class T>
class Element {

    protected:

        T           value;
        bool        active;

    public:

        Element(bool element_active = false, T element_value = 0) {
            set_active(element_active);
            set_value(element_value);
        };
        ~Element() {};

        T           get_value()     {return value;};
        bool        get_active()    {return active;};

        void        set_value(T element_value)   {value = element_value;};
        void        set_active(bool element_active)     {active = element_active;};

        bool operator== (const T& o) {
            return (value == o.value &&
                    active == o.active);
        }

        Element<T>* clone() {   return new Element<T>(this->get_active(), this->get_value()); };

};

#endif
