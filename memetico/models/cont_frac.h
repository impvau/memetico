
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief ContinuedFraction is MemeticModel where each fraction term is a Regression
 */

#ifndef MEMETICO_MODELS_CONT_FRAC_H_
#define MEMETICO_MODELS_CONT_FRAC_H_

// Local
#include <memetico/helpers/safe_ops.h>
#include <memetico/helpers/hash.h>
#include <memetico/globals.h>
#include <memetico/model_base/model_meme.h>
#include <memetico/helpers/print.h>
#include <memetico/models/regression.h>

// Std
#include <stdexcept>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <math.h>
#include <unordered_set>
#include <unordered_map>

/**
 * @brief The ContinuedFraction class extends the Regression class for a new Model representation
 * 
 * A continued fraction is of the form \f$f(x)=g_0(x)+\frac{h_0(x)}{g_1(x)+\frac{h_1(x)}{g_2(x)+\cdots}}\f$ \n \n
 * Where
 * - \f$x\f$ is the set of independent variables \f$x = \{x_1, x_2,...,x_n\} \f$ \n
 * - \f$x\f$ is the set of coefficients for each independent varaible \f$c = \{c_1, c_2,...,c_n\} \f$ \n
 * - Each term \f$g_i(x)\f$, \f$h_i(x)\f$ is of a linear form \f$cx+c_0\f$ \n
 * 
 * The independent variables are utilised in each term, resulting in increasing coefficients and constants to optimise as fraction depth increases.
 * 
 */
template <typename Traits>
class ContinuedFraction : public MemeticModel<typename Traits::UType>, 
                          public Traits::template MPType<typename Traits::UType, 
                          ContinuedFraction<Traits>> {

    public:

        /**
         * @brief Construct ContinuedFraction
         * - Set depth to frac_depth
         * - Set frac_terms to 2*depth+1
         * - Set params_per_term to Data::IVS.size()+1
         * - Set count to frac_terms*params_per_term
         * - Create a T object for each term in the fraction
         * - Create the global_active array of size count
         * - Randomise the independent varaibles and constant values
         * - With 50% chance, globally inactive the independent varaibles 
         *  (i.e. first N-1 paramters, leaving the constant at parameter N active)
         * 
         * @param frac_depth depth of the fraction
         */
        ContinuedFraction(size_t frac_depth = 4);
        
        /** Constrct fraction as copy of o */
        ContinuedFraction(const ContinuedFraction<Traits> &o);

        //// Overriding functions 
        
        /** @brief Return value given a sequential index pos in the fraction */
        typename Traits::UType  get_value(size_t pos) override {
            return terms[term_from_pos(pos)].get_value(param_from_pos(pos));
        };

        /** @brief Return active flag given a sqeuential index pos in the fraction */
        bool    get_active(size_t pos) override {
            return terms[term_from_pos(pos)].get_active(param_from_pos(pos));
        };

        /** @brief Set value of paramter at fraction pos to val */
        void    set_value(size_t pos, typename Traits::UType val) override {
            terms[term_from_pos(pos)].set_value(param_from_pos(pos), val);
        };

        /** @brief Set local active flag of varaible at pos to val  */
        void    set_active(size_t pos, bool val) override {
            terms[term_from_pos(pos)].set_active(param_from_pos(pos), val);
        };

        /** @brief Return total number of active paramters across the entire fraction */
        size_t  get_count_active() override { return get_active_positions().size(); };

        /** @brief Return vector of indices of the active varaibles positions*/
        vector<size_t>  get_active_positions() override;

        /** @brief Return TreeNode for GPU processing */
        void    get_node(TreeNode * n) override;

        /** @brief Comparison operator for ContinuedFraction<T> */
        bool    operator== (ContinuedFraction<Traits>& o);

        /**
         * @brief Output operator for ContinuedFraction<T>
         * We ouput the fraction in the form 
         *      g0(x)+
         *          h0(x)/(g1(x)+
         *              h1(x)/g2(x)+..
         * 
         * Where each gi(x), hi(x) is a Regression in the form
         * 'coeff_val1*(var_name1)+coeff_val2*(var_name2)+...+coeff_valN*(var_nameN)+c'
         * 
         * @return os
         *           
         */        
        template <typename Traits2>
        friend ostream& operator<<(std::ostream& os, ContinuedFraction<Traits2>& c);

        /**  
         * @brief mutate \a this MemeticModel with consideration of the associated \a pocket solution
         * The hard mutation changes a global active flag meaning there is significant impact and occurs when the current (this)
         * is either better or almost as fit as the pocket OR it is greatly less fit than the pocket. This is defined as 
         * less than 1.2*pocket->fitness or >2*pocket->fitness
         * 
         * - Determine soft or hard mutation based on the above
         * 
         * - For hard mutation
         *  - Select an independent variable at random
         *  - Toggle the global active value
         *  - If the variable is now active, re-randomise the value
         *
         * - For soft mutation
         *  - Select a parameter (i.e. including constants) uniform at random from those that are not globally inactive 
         *  - If there are no such parameters exit
         *  - Toggle the active flag of the parameter
         * 
         * @param pocket the pocket solution associated with the current solution i.e. \a this
         */
        void    mutate(MemeticModel<typename Traits::UType>& pocket) override {

            // Call mutation policy
            Traits::template MPType<typename Traits::UType, ContinuedFraction<Traits>>::mutate(pocket);

        }

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
        void recombine(MemeticModel<typename Traits::UType>* model1, MemeticModel<typename Traits::UType>* model2, int method_override = -1) override;

        /** 
         * @brief Evaluate a ContinuedFraction given sample values
         * - Substitue \a values for the independent varaibles in the terms of the continued fraction
         *
         * @param values an array of sample values to evaluate at (\f$c\f$) which is params_per_term in length
         * @return value of the continued fraction evaluated at \a values 
         */
        double  evaluate(vector<double>& values) override;

        /** @brief Print the solution to stdout */
        void print() override { 
            cout << *this << endl; 
        };

        /** @brief Get the string representation of the solution */
        string str() override {
            stringstream ss;
            ss.precision(18);
            ss << *this;
            return ss.str();
        }

        //// CF specific functions

        /** @brief Return fraction depth */
        size_t  get_depth() const                   { return depth; };

        /** @brief Return number of terms 2*d+1 */
        size_t  get_frac_terms() const              { return depth*2+1;  };

        /** @brief Return maximum number of varaibles/constants per term (some may be masked) */
        size_t  get_params_per_term() const         { return params_per_term; };

        /** @brief Return total maximum number of paramters over all fraction terms (some may be masked) */
        size_t  get_param_count() const             { return params_per_term*get_frac_terms(); };

        /** @brief Return term at specific position in the fraction t0, t1, t2.. etc. */
        typename Traits::TType& get_terms(size_t term)                   { return terms[term]; };

        /** @brief Set term at pos to obj */
        void    set_terms(size_t pos, typename Traits::TType& obj)    { terms[pos] = obj; };

        /** @brief add term at pos to obj */
        void    add_terms(typename Traits::TType& obj)    { terms.push_back(obj); };

        void    pop_terms()    { terms.pop_back(); };

        void    set_params_per_term(size_t p)    { params_per_term = p; };

        /** @brief Set depth of the fraction and remove/add randomised terms */
        void    set_depth(size_t new_depth);
        
        void    set_depth_val(size_t new_depth) {depth = new_depth;};

        /**
         * @brief 
         * @param 
         */
        void    sanitise();
        
        vector<typename Traits::TType> terms;

    private:

        /** @brief Given a sequential index pos, return the term it appears on */
        size_t term_from_pos(size_t pos) const { return floor(pos/params_per_term); };
        
        /** @brief Given a sequential index pos, return the parameter possition it is (i.e. position in params_per_term) */
        size_t param_from_pos(size_t pos) const { return pos % params_per_term; };

        /** Zero-based depth of the fraction */
        size_t  depth;

        /** Number of parameters in each term. As this is a linear model we have Data::IVS.size()+1 where the +1 is the constant*/
        size_t  params_per_term;

};

#include <memetico/models/cont_frac.tpp>

#endif
