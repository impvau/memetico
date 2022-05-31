
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief A Term that exists within a Model
 * 
 */

#ifndef FORMS_TERM_H
#define FORMS_TERM_H

using namespace std;

// Local
#include <forms/element.h>

// Std Lib
#include <cstddef>

/**
 * @brief 
 * 
 */
template<class T>
class Term {

    protected:

        size_t          count;
        Element<T>**    elems;
        
    public:

        // Construction 
        Term(size_t element_count) { 
            count = element_count; 
            elems = new Element<T>*[count];
            for(size_t i = 0; i < count; i++)
                elems[i] = new Element<T>();
        }
        ~Term() { 

            for(size_t i = 0; i < count; i++)
                delete elems[i];

            delete elems;

        };

        Element<T>*     get_elem(size_t pos)       {if(pos < count) return elems[pos]; else return nullptr;}
        size_t          get_count()                {return count;}

        void set_elem(size_t pos, Element<T>* elem) { 
            
            if(pos < count) {
                delete elems[pos];
                elems[pos] = elem; 
            }
            
        };

        bool operator== (Term& o) {
            
            if( count != o.count )  return false;

            for( size_t i = 0; i < count; i++) {
                if(elems[i] != o.get_elem(i))
                    return false;
            }
            return true;
        }

        Term<T>* clone() {

            // Create new structure
            Term<T>* o = new Term<T>(this->count);
            
            for(size_t i = 0; i < this->count; i++)
                o->set_elem(i, this->get_elem(i)->clone() );
        
            return o;

        }

};

#endif
