
/** @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief ContinuedFractionDynamicDepth is a ContinuedFraction that can dynamically change depth
 */

#ifndef MEMETICO_MODELS_CONT_FRAC_DD_H_
#define MEMETICO_MODELS_CONT_FRAC_DD_H_

// Local
#include <memetico/models/cont_frac.h>

// Std
#include <stdexcept>
#include <typeinfo>

/**
 * @brief Types of Dynamic Depth approaches
 */
enum DynamicDepthType {
    DynamicNone,
    DynamicAdaptive,
    DynamicRandom,
    DynamicAdaptiveMutation
};

/**
 * @brief The ContinuedFractionAdaptiveDepth class extends the ContinuedFraction class with adaptive depth logic
 */
template<typename T, typename U>
class ContinuedFractionDynamicDepth : public ContinuedFraction<T,U> {

    public:

        /**
         * @brief Construct ContinuedFractionAdaptiveDepth as per parent ContinuedFraction<T,U>,
         * however using a depth in the range [best_model_depth-1, best_model_depth+1] while ensuring 
         * depth is not negative (reset to 0 when attempted)
         */
        ContinuedFractionDynamicDepth(size_t depth = ContinuedFractionDynamicDepth<T,U>::determine_depth()) : ContinuedFraction<T,U>( depth ) {};

        /** @brief Copy constructor */
        ContinuedFractionDynamicDepth(const ContinuedFractionDynamicDepth<T,U> &o) :
            ContinuedFraction<T,U>::ContinuedFraction<T,U>(o) {};

        /** @brief Mutate operator 
         * Mutate as standard ContinuedFraction but if DynamicAdaptiveMutation is set, then re-determine depth
        */
        void    mutate(MemeticModel<U> & model) override {
            
            // Call normal mutate
            ContinuedFraction<T,U>::mutate(model);

            // If we are using the adaptive mutate approach
            if( DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation ) {

                size_t d = determine_depth();
                this->set_depth(d);
            }
        };

        /** @brief return a depth uniformly at random between [meme::POCKET_DEPTH-1, meme::POCKET_DEPTH+1], setting to 0 when negative */ 
        static size_t determine_depth() {

            int rand = meme::DEPTH;

            // If adaptive approach or adaptive mutatution approach
            if(DYNAMIC_DEPTH_TYPE == DynamicAdaptive || DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation) {

                rand = meme::RANDINT(meme::POCKET_DEPTH-1, meme::POCKET_DEPTH+1);
                
                // Dont allow negative depth
                if(rand < 0 )   rand = 0;

            // If random depth
            } else if (DYNAMIC_DEPTH_TYPE == DynamicRandom)
                rand = meme::RANDINT(0, 5);
            
            //cout << "New fraction of depth: " << rand << endl;

            return size_t(rand);
        };

        static DynamicDepthType DYNAMIC_DEPTH_TYPE;

    private: 
        
};

template<typename T, typename U>
DynamicDepthType ContinuedFractionDynamicDepth<T,U>::DYNAMIC_DEPTH_TYPE = DynamicNone;
    
#endif
