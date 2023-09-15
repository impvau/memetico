
/** @file
 * @author andy@impv.au
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
 * @brief The ContinuedFractionAdaptiveDepth class extends the ContinuedFraction class with adaptive depth logic
 */

template <typename Traits>
class ContinuedFractionDynamicDepth : public MemeticModel<typename Traits::UType>, 
                          public Traits::template MPType<typename Traits::UType, 
                          ContinuedFraction<Traits>> {

    public:

        /**
         * @brief Construct ContinuedFractionAdaptiveDepth as per parent ContinuedFraction<T,U>,
         * however using a depth in the range [best_model_depth-1, best_model_depth+1] while ensuring 
         * depth is not negative (reset to 0 when attempted)
         */
        //ContinuedFractionDynamicDepth(size_t depth = ContinuedFractionDynamicDepth<T,U,MP>::determine_depth()) : ContinuedFraction<T,U,MP>( depth ) {};
        ContinuedFractionDynamicDepth(size_t depth = determine_depth()) : ContinuedFraction<Traits>( depth ) {};

        /** @brief Copy constructor */
        ContinuedFractionDynamicDepth(const ContinuedFractionDynamicDepth<Traits> &o) :
            ContinuedFraction<Traits>::ContinuedFraction<Traits>(o) {};

        /** @brief Mutate operator 
         * Mutate as standard ContinuedFraction but if DynamicAdaptiveMutation is set, then re-determine depth
        */
        void    mutate(MemeticModel<typename Traits::UType> & model) override {
            
            // Call normal mutate
            Traits::template MPType<typename Traits::UType, ContinuedFractionDynamicDepth<Traits>>::mutate(model);

            // If we are using the adaptive mutate approach
            if( meme::DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation ) {

                size_t d = determine_depth();
                this->set_depth(d);
            }
        };

        /** @brief return a depth uniformly at random between [meme::POCKET_DEPTH-1, meme::POCKET_DEPTH+1], setting to 0 when negative */ 
        static size_t determine_depth() {

            int rand = meme::DEPTH;

            // If adaptive approach or adaptive mutatution approach
            if(meme::DYNAMIC_DEPTH_TYPE == DynamicAdaptive || meme::DYNAMIC_DEPTH_TYPE == DynamicAdaptiveMutation) {

                rand = meme::RANDINT(meme::POCKET_DEPTH-1, meme::POCKET_DEPTH+1);
                
                // Dont allow negative depth
                if(rand < 0 )   rand = 0;

            // If random depth
            } else if (meme::DYNAMIC_DEPTH_TYPE == DynamicRandom)
                rand = meme::RANDINT(0, 5);
            
            //cout << "New fraction of depth: " << rand << endl;

            return size_t(rand);
        };

    private: 
        
};

#endif
