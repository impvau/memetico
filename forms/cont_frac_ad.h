
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for ContinuedFractionAdaptiveDepth class
*/

#ifndef FORMS_CONT_FRAC_AD_H
#define FORMS_CONT_FRAC_AD_H

// Local
#include <forms/cont_frac.h>

// Std
#include <stdexcept>
#include <typeinfo>

/**
 * @brief The ContinuedFractionAdaptiveDepth class extends the ContinuedFraction class with adaptive depth logic
 */
template<class T>
class ContinuedFractionAdaptiveDepth : public ContinuedFraction<T> {

    public:

        size_t rand_depth() {

            int rand = memetico::RANDINT(memetico::POCKET_DEPTH-1, memetico::POCKET_DEPTH+1);
            
            // Dont allow negative depth
            if(rand < 0 )   rand = 0;
            
            return size_t(rand);
        }

        // See implementation for details on the following functions
        ContinuedFractionAdaptiveDepth() : ContinuedFraction<T>( rand_depth() ) {};

        ContinuedFractionAdaptiveDepth<T>* clone() { 
            return static_cast<ContinuedFractionAdaptiveDepth<T>*>( ContinuedFraction<T>::clone()); 
        };

};

#endif
