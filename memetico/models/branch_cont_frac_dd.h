
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief ContinuedFraction is MemeticModel where each fraction term is a Regression
 */

#ifndef MEMETICO_MODELS_BRANCH_CONT_FRAC_H_
#define MEMETICO_MODELS_BRANCH_CONT_FRAC_H_

// Local
#include <memetico/helpers/safe_ops.h>
#include <memetico/model_base/model_meme.h>
#include <memetico/helpers/print.h>
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac_dd.h>

// Std
#include <stdexcept>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <math.h>

/**
 * @brief 
 */
template <typename Traits>
class BranchedContinuedFraction : public ContinuedFractionDynamicDepth<Traits> {

    public:

        BranchedContinuedFraction(size_t not_used = 0) : ContinuedFractionDynamicDepth<Traits>(this->determine_depth()) {
        //BranchedContinuedFraction(size_t not_used = 0) : ContinuedFractionDynamicDepth<T,U>(1) {

            this->sub_depth = meme::DEPTH;

            for(size_t i = 0; i < this->get_frac_terms(); i++) {
                typename Traits::TType temp = typename Traits::TType(this->sub_depth);
                this->set_terms(i, temp);
                if(i == 0 )
                    this->set_params_per_term(this->get_terms(0).get_param_count());
            }

        };

        BranchedContinuedFraction(const BranchedContinuedFraction<Traits> &o) : ContinuedFractionDynamicDepth<Traits>(o) {
            
            this->sub_depth = o.sub_depth;

            for(size_t i = 0; i < this->get_frac_terms(); i++) {
                typename Traits::TType temp = typename Traits::TType(this->terms[i]);
                this->set_terms(i, temp);
                if(i == 0 )
                    this->set_params_per_term(this->get_terms(0).get_param_count());
            }
            
        };

        size_t  get_param_count() { 
            size_t params = 0;
            for(size_t i = 0; i < this->get_frac_terms(); i++) {
                params += this->get_terms(i).get_param_count();
            }
            return params;
        };

        /** @brief Overwrite as 0, does not make sense as each term of the fraction has different global active */
        bool    get_global_active(size_t iv) const  { return 0; };

        size_t  get_count_active() { 
            size_t count = 0;
            for(size_t i = 0; i < this->get_frac_terms(); i++) {
                count += this->get_terms(i).get_count_active();
            }
            return count;
        };

        void    mutate(MemeticModel<typename Traits::UType>& pocket) override {

            return;

            // We shift depth, but maintain the same depth across all terms
            size_t new_depth = this->determine_depth();

            // Mutate each term
            for(size_t i = 0; i < this->get_frac_terms(); i++) {
                this->get_terms(i).mutate(pocket);
                this->get_terms(i).set_depth(new_depth);
            }

        };

        /**  
         * @brief recombine \a this MemeticModel considering two other MemeticModels
         * - Determine the recombination method to apply to all T fraction terms
         * - For all terms in the fraction
         *  - Call the underlying recombination method in T
         *  - Force the use of the global recombination method
         * 
         * @param m1 first MemeticModel
         * @param m2 second MemeticModel
         */
        void    recombine(MemeticModel<typename Traits::UType>* model1, MemeticModel<typename Traits::UType>* model2, int method_override = -1) override {

            BranchedContinuedFraction<Traits>* m1 = static_cast<BranchedContinuedFraction<Traits>*>(model1);
            BranchedContinuedFraction<Traits>* m2 = static_cast<BranchedContinuedFraction<Traits>*>(model2);

            int method = RandInt::RANDINT->rand(0,2);

            //cout << "m1: " << *m1 << endl << endl;
            //cout << "m2: " << *m2 << endl << endl;

            // Minimum from m1/m2
            size_t min_terms = min(m1->get_frac_terms(), m2->get_frac_terms());

            // Minimum of the above with the current object
            min_terms = min(min_terms, this->get_frac_terms());

            for(size_t i = 0; i < min_terms; i++) {

                //cout << "m1 t" << i << ": " << m1->get_terms(i) << endl << endl;
                //cout << "m2 t" << i << ": " << m1->get_terms(i) << endl << endl;
                //cout << m1->get_terms(i) << endl;
                //cout << m2->get_terms(i) << endl;

                this->get_terms(i).recombine( &(m1->get_terms(i)), &(m2->get_terms(i)), method);
            }

        }

        vector<size_t>  get_active_positions() {
            
            vector<size_t> active_pos;
            for(size_t i = 0; i < this->get_frac_terms(); i++) {

                vector<size_t> temp_pos = this->get_terms(i).get_active_positions();
                for (auto& pos : temp_pos) {
                    pos += this->get_terms(i).get_param_count()*i;
                }

                active_pos.insert(active_pos.end(), temp_pos.begin(), temp_pos.end());

            }

            return active_pos;
        }

        bool operator== (ContinuedFraction<Traits>& o) {

            if( this->get_depth() != o.get_depth())
                return false;

            if( this->get_frac_terms() != o.get_frac_terms() )
                return false;

            for(size_t i = 0 ; i < this->get_frac_terms(); i++) {
                if( !(this->get_terms(i) == o.get_terms(i)) )
                    return false;
            }

            // Else we match!
            return true;
        }

        void set_depth(size_t new_depth) {

            size_t  new_frac_terms = 2*new_depth+1;

            // If we are reducing terms, simply remove the number of terms from the vector
            if( new_frac_terms < this->get_frac_terms()) {

                size_t excess_terms = this->get_frac_terms()-new_frac_terms;
                for(size_t i = 0; i < excess_terms; i++)
                    this->pop_terms();

            // If we are adding terms, we need to increase terms
            } else {

                size_t new_terms = new_frac_terms-this->get_frac_terms();
                for(size_t i = 0; i < new_terms; i++) {
                    typename Traits::TType new_term  = typename Traits::TType(1);
                    new_term.randomise();
                    this->add_terms(new_term);
                }

            }

            this->set_depth_val(new_depth);

        }

        size_t get_sub_depth() {return this->sub_depth;};

    private:

        size_t sub_depth;

};

#endif
