
/** @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief ContinuedFraction is MemeticModel where each fraction term is a Regression
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_CONT_FRAC_H_
#define MEMETICO_MODELS_CONT_FRAC_H_

// Local
#include <memetico/globals.h>
#include <memetico/model_base/model_meme.h>

// Std
#include <stdexcept>
#include <typeinfo>

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
        
        /** @brief Deconstruct ContinuedFraction by deleting global_active array and all term objects */
        ~ContinuedFraction() {
            delete global_active;
            for(size_t i = 0; i < frac_terms; i++)
                delete objs[i];
            delete objs;
        }

        /** 
         * @brief clone a Regression
         * @bug need to make this a copy constructor
         */
        ContinuedFraction<T>* clone() override;

        /** 
         * @brief Evaluate a ContinuedFraction given sample values
         * - Substitue \a values for the independent varaibles in the terms of the continued fraction
         *
         * @param values an array of sample values to evaluate at (\f$c\f$) which is params_per_term in length
         * @return value of the continued fraction evaluated at \a values 
         */
        double  evaluate(double* values);

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
        void    mutate(MemeticModel<T>* pocket = nullptr) override;

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
        void    recombine(MemeticModel<T>* model1, MemeticModel<T>* model2);

        /** @brief getter for fraction depth */
        size_t  get_depth()                         { return depth;  };

        /** @brief getter for fraction terms */
        size_t  get_frac_terms()                    { return frac_terms;  };

        /** @brief getter for paramters within each term of the fraction */
        size_t  get_params_per_term()               { return params_per_term; };

        /** @brief getter for total number of parameters */
        size_t  get_param_count()                   { return params_per_term*frac_terms; };

        /** @brief getter for global active flags */
        bool    get_global_active(size_t iv)        { return global_active[iv]; };

        /** @brief getter for each term T active flags */
        T*      get_objs(size_t term)               { return objs[term]; };

        /** 
         * @brief get parameter positions for the active parameters over the entire fraction. 
         * Here we map all parameters in T to a 1-D array 
         * @bug We should use the specific size of each object T rather than that stored in the Cf.
         * Later, we may wish to have a dynamic number of parameters per fraction term.
         */
        vector<size_t>  get_active_positions() {
            // Accumulate the positions across all T objects
            vector<size_t> ret;
            for( size_t term = 0; term < get_frac_terms(); term++ ) {
                for( size_t param = 0; param < params_per_term; param++ ) {
                    if( objs[term]->get_active(param) )
                        ret.push_back( term*get_params_per_term()+param );
                }
            }
            return ret;
        };

        /** @brief getter parameter based on 1-D position */
        double  get_param(size_t pos) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;

            //return objs[term]->get_param(param);

            // Only if globally on
            if( get_global_active(param) )  return objs[term]->get_param(param);
            else                            return 0;
        };

        /** @brief getter for active flag based on 1-D position */
        bool    get_active(size_t pos) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            
            //return objs[term]->get_active(param);

            // Only if globally on
            if( get_global_active(param) )  return objs[term]->get_active(param);
            else                            return false;
        };

        /** @brief get the number of active parameters */
        size_t  get_count_active() { return get_active_positions().size(); };

        /** @brief setter for value based on 1-D position */
        void    set_param(size_t pos, double val) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            objs[term]->set_param(param, val);
        };

        /** @brief setter for fraction term given fraction term number \a pos */
        void    set_objs(size_t pos, T* obj)    { objs[pos] = obj; };

        /** @brief setter for the active value based on 1-D position */
        void    set_active(size_t pos, bool val) {
            size_t term = floor(pos/params_per_term);
            size_t param = pos % params_per_term;
            objs[term]->set_active(param, val);
        };

        /** @brief setter for global active flag for a given variable */
        void    set_global_active(size_t iv, bool val) {

            // Make sure iv is within the range of paramters
            if(iv < params_per_term) {

                // Update global flag
                global_active[iv] = val;

                // Update all term flags
                for(size_t term = 0; term < get_frac_terms(); term++)
                    objs[term]->set_active(iv, val);

            }  
        };

        /** @brief setter for fraction depth */
        void    set_depth(size_t new_depth) { 
              
            //cout << "set_depth new_depth:" << new_depth << " old_depth:" << depth << endl;
            //cout << "old_frac: " << *this << endl;

            // Setup the new terms and term objects
            size_t  new_frac_terms = 2*new_depth+1;
            T**     new_objs = new T*[new_frac_terms];
            for(size_t i = 0; i < min(frac_terms, new_frac_terms); i++)
                new_objs[i] = objs[i]->clone();

            // If we have increased size, i.e. frac_terms < new_frac_terms, then randomise the values of the new terms
            for(size_t i = min(frac_terms, new_frac_terms); i < new_frac_terms; i++)
                new_objs[i] = new T(params_per_term);
            
            // Delete old object array and assign the new objects, depth and number of terms
            delete objs;
            objs = new_objs;
            depth = new_depth;
            frac_terms = new_frac_terms;

            //cout << "new_frac: " << *this << endl;

        };
        
        /** @brief output operator for a ContinuedFraction */
        template<class F>
        friend ostream& operator<<(ostream& os, const ContinuedFraction<F>& c);

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
        bool*   global_active;

        /** Zero-based depth of the fraction */
        size_t  depth;

        /** Number of terms in the fraction defined as \f$2*depth+1\f$ where the first depth has only one term and each subsequent depth has an additional 2 terms */
        size_t  frac_terms;

        /** Number of parameters in each term. As this is a linear model we have Data::IVS.size()+1 where the +1 is the constant*/
        size_t  params_per_term;

        T** objs;


};

template<class T>
int ContinuedFraction<T>::RAND_LOWER = 1;

template<class T>
int ContinuedFraction<T>::RAND_UPPER = 30;

template<class T>
size_t ContinuedFraction<T>::DEPTH = 4;

#include <memetico/models/cont_frac.tpp>

#endif
