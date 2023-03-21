
/** @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Andrew Ciezak <andy@impv.au>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief ContinuedFraction is MemeticModel where each fraction term is a Regression
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_CONT_FRAC_H_
#define MEMETICO_MODELS_CONT_FRAC_H_

// Local
#include <memetico/helpers/safe_ops.h>
#include <memetico/model_base/model_meme.h>
#include <memetico/helpers/print.h>
#include <memetico/models/regression.h>

// Std
#include <stdexcept>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <math.h>

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
template<class T>
class ContinuedFraction : public MemeticModel<T> {

    public:

        /** Default lower bound on parameter randomisations */
        static int      RAND_LOWER;

        /** Default upper bound on parameter randomisations */ 
        static int      RAND_UPPER;

        /** Depth of the fraction indicating the number of terms \f$ t = 2*DEPTH+1 \f$ */ 
        static size_t   DEPTH;

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
        ContinuedFraction(size_t frac_depth = ContinuedFraction<T>::DEPTH);
        
        /** Constrct fraction as copy of o */
        ContinuedFraction(const ContinuedFraction<T> &o);
        
        /** 
         * @brief Evaluate a ContinuedFraction given sample values
         * - Substitue \a values for the independent varaibles in the terms of the continued fraction
         *
         * @param values an array of sample values to evaluate at (\f$c\f$) which is params_per_term in length
         * @return value of the continued fraction evaluated at \a values 
         */
        virtual double  evaluate(vector<double>& values);

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
        void    mutate(MemeticModel<T>& pocket) override;

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
        void    recombine(MemeticModel<T>* model1, MemeticModel<T>* model2, int method_override = -1) override;

        /** @brief Return fraction depth */
        size_t  get_depth() const                   { return depth;  };

        /** @brief Return number of terms 2*d+1 */
        size_t  get_frac_terms() const              { return frac_terms;  };

        /** @brief Return maximum number of varaibles/constants per term (some may be masked) */
        size_t  get_params_per_term() const         { return params_per_term; };

        /** @brief Return total maximum number of paramters over all fraction terms (some may be masked) */
        size_t  get_param_count() const             { return params_per_term*frac_terms; };

        /** @brief Return number of pararmters globally turned on */
        bool    get_global_active(size_t iv) const  { return global_active[iv]; };

        /** @brief Return term at specific position in the fraction t0, t1, t2.. etc. */
        Regression<T>&      get_terms(size_t term)              { return terms[term]; };

        virtual void get_node(TreeNode * n);

        /** @brief Return vector of active varaible indices 
         * @bug We should use the specific size of each object T rather than that stored in the Cf.
         * Later, we may wish to have a dynamic number of parameters per fraction term.
         */
        virtual vector<size_t>  get_active_positions();

        /** @brief Return value at index pos in the fraction*/
        double  get_value(size_t pos) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;

            // Only globally or locally on
            if( get_global_active(param) || terms[term].get_active(param) )  
                return terms[term].get_value(param);
            else                            
                return 0;
        };

        /** @brief Return active flat at index pos in the fraction */
        bool    get_active(size_t pos) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            
             // Only globally or locally on
            if( get_global_active(param) || terms[term].get_active(param) )  
                return terms[term].get_active(param);
            else                            
                return false;
        };

        /** @brief Return number of active paramters across the entire fraction */
        size_t  get_count_active() { return get_active_positions().size(); };

        /** @brief Set value of paramter at fraction pos to val */
        virtual void    set_value(size_t pos, T val) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            terms[term].set_value(param, val);
        };

        /** @brief Set term at pos to obj */
        void    set_terms(size_t pos, Regression<T>& obj)    { terms[pos] = obj; };

        /** @brief Set local active flag of varaible at pos to val  */
        void    set_active(size_t pos, bool val) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            terms[term].set_active(param, val);
        };

        /** @brief Set global active flag for a specific data variable iv to val  */
        void    set_global_active(size_t iv, bool val, bool align_fraction = true);

        /** @brief Set depth of the fraction and remove/add randomised terms */
        void    set_depth(size_t new_depth);
        
        virtual void print() { cout << *this << endl; };
        virtual string str  () {
            stringstream ss;
            ss.precision(18);
            ss << *this;
            return ss.str();
        }

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
        template<class F>
        friend ostream& operator<<(ostream& os, ContinuedFraction<F>& c);

        /** @brief Comparison operator for ContinuedFraction<T> */
        bool operator== (ContinuedFraction<T>& o);

        /**
         * @brief Randomise all paramters to a uniformally random value inclusively between min and max
         * - Randomly activate globally the independent varaibles with 50% chance (i.e. first N-1 parameters)
         * - Set the constant on (i.e. Nth parameter)
         * - Randomise the varaibles between \a min and \a max as per T::randomise() function
         * 
         * @param min           Minimum value inclusive
         * @param max           Maximum value inclusive
         */
        void    randomise(int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

        /**
         * @brief 
         * @param 
         */
        void    sanitise();
        
    private:

        /** Global active flag that applies for the independent varaible down all depths and overrides any setting in the active array */
        vector<bool>   global_active;

        /** Zero-based depth of the fraction */
        size_t  depth;

        /** Number of terms in the fraction defined as \f$2*depth+1\f$ where the first depth has only one term and each subsequent depth has an additional 2 terms */
        size_t  frac_terms;

        /** Number of parameters in each term. As this is a linear model we have Data::IVS.size()+1 where the +1 is the constant*/
        size_t  params_per_term;

        vector<Regression<T>> terms;


};

template<class T>
int ContinuedFraction<T>::RAND_LOWER = 1;

template<class T>
int ContinuedFraction<T>::RAND_UPPER = 30;

template<class T>
size_t ContinuedFraction<T>::DEPTH = 4;

#include <memetico/models/cont_frac.tpp>

#endif
