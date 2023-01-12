
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief ContinuedFractionDynamicDepth is a ContinuedFraction that can dynamically change depth
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_CONT_FRAC_DD_H_
#define MEMETICO_MODELS_CONT_FRAC_DD_H_

// Local
#include <memetico/models/cont_frac.h>

// Std
#include <stdexcept>
#include <typeinfo>

/**
 * @brief The ContinuedFractionAdaptiveDepth class extends the ContinuedFraction class with adaptive depth logic
 */
template<class T>
class ContinuedFractionDynamicDepth : public ContinuedFraction<T> {

    public:

        /**
         * @brief Construct ContinuedFractionAdaptiveDepth as per parent ContinuedFraction<T>,
         * however using a depth in the range [best_model_depth-1, best_model_depth+1] while ensuring 
         * depth is not negative (reset to 0 when attempted)
         */
        ContinuedFractionDynamicDepth() : ContinuedFraction<T>( determine_depth() ) {};

        /** @brief output operator for a ContinuedFractionAdaptiveDepth */
        ContinuedFractionDynamicDepth<T>* clone() { 
            return static_cast<ContinuedFractionDynamicDepth<T>*>( ContinuedFraction<T>::clone()); 
        };

        void    mutate(MemeticModel<T>* model = nullptr) override {
            
            // Call normal mutate
            ContinuedFraction<T>::mutate(model);

            // If we are using the adaptive mutate approach
            if( memetico::DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation ) {

                size_t d = determine_depth();
                cout << "AdaptiveMutate depth: " << this->get_depth() << " new_depth: " << d << endl;
                this->set_depth(d);
            }
        };


    private:
    
        /** @brief return a depth uniformly at random between [memetico::POCKET_DEPTH-1, memetico::POCKET_DEPTH+1], setting to 0 when negative */ 
        size_t determine_depth() {

            int rand = (int)ContinuedFraction<T>::DEPTH;

            // If adaptive approach or adaptive mutatution approach
            if(memetico::DYNAMIC_DEPTH_TYPE == DynamicAdaptive || memetico::DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation) {

                rand = memetico::RANDINT(memetico::POCKET_DEPTH-1, memetico::POCKET_DEPTH+1);
                
                // Dont allow negative depth
                if(rand < 0 )   rand = 0;

            // If random depth
            } else if (memetico::DYNAMIC_DEPTH_TYPE == DynamicRandom)
                rand = memetico::RANDINT(0, ContinuedFraction<T>::DEPTH);
            
            cout << "Creating Fraction Depth: " << rand << " Global_Depth: " << memetico::POCKET_DEPTH << endl;

            return size_t(rand);
        };
};
    
#endif
