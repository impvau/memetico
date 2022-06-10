
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Regression is a MemeticModel that contains a single linear function 
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_REGRESSION_H_
#define MEMETICO_MODELS_REGRESSION_H_

using namespace std;
using namespace memetico;

// Local
#include <memetico/globals.h>
#include <memetico/data.h>
#include <memetico/model_base/term.h>
#include <memetico/model_base/model_meme.h>

// Std Lib
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <iomanip>
#include <iostream>

/**
 * @brief A simple linear Model in the form \f$cx+c_0\f$
 * Where
 * - \f$x\f$ is the set of independent variables \f$x = \{x_1, x_2,...,x_n\} \f$
 * - \f$x\f$ is the set of coefficients for each independent varaible \f$c = \{c_1, c_2,...,c_n\} \f$ 
 * - \f$c_0\f$ is the constant 
 */
template<class T>
class Regression : public MemeticModel<T> {

    public:

        /** @brief Construct Regression with a Term of param_count size */
        Regression(size_t param_count = 0) : MemeticModel<T>() { term = new Term<T>(param_count); };

        /** 
         * @brief setter for active flag at \a pos with value \a val
         * @param pos position of paramter
         * @param val value of active flag
         */
        void        set_active(size_t pos, bool val)    { term->get_elem(pos)->set_active(val); };
       
        /** 
         * @brief setter for value at \a pos with value \a val
         * @param pos position of parameter
         * @param val value of active flag
         */
        void        set_param(size_t pos, T val)        { term->get_elem(pos)->set_value(val); };
        
        /**  
         * @brief getter for active flag at \a pos
         * @param pos position of parameter
         * @returns active flag at \a pos 
         */
        bool        get_active(size_t pos)              { return term->get_elem(pos)->get_active(); };
        
        /**  
         * @brief getter for parameter value at \a pos
         * @param pos position of parameter
         * @returns value at pos
         * @bug here we define the return as double. What if the parameter is an integer model
         * or we are using something like a coordinate representation for a parameter? We may 
         * need to extend here to a second template U but holding off for now.
         */
        double      get_param(size_t pos)               { return term->get_elem(pos)->get_value(); };

        /**  
         * @brief getter for parameter count
         * @returns number of parameters usable in the model
         */
        size_t      get_count()                         { return term->get_count(); };
        
        /**  
         * @brief getter for Term object
         * @returns Term containing the Regression Elements
         */
        Term<T>*    get_term()                          { return term; };

        /** @brief output operator for a Regression */
        template<class F>
        friend ostream& operator<<(ostream& os, const Regression<F>& r);

        /** 
         * @brief clone a Regression
         * @bug need to make this a copy constructor
         */
        Regression<T>* clone();

        /**  
         * @brief mutate \a this MemeticModel and optionally consider another model m.
         * - A random parameter is selected uniform at random and has its active flag toggled
         * 
         * @param m another model to consider in the mutation
         */
        void    mutate(MemeticModel<T>* m = nullptr) override;     

        /**  
         * @brief recombine \a this MemeticModel considering two other MemeticModels
         * - Determine recombination method
         *  - Where not \a method_override, determine a recombination method uniformly at random of 
         *      - Union (&)
         *      - Intersection (|) 
         *      - Symmetric difference (^)
         *  - Where \a method_override is provided select the method from above that corresponds to the enum value i.e.
         *      - -1 = Random
         *      -  0 = Union
         *      -  1 = Intesection
         *      -  2 = Symmetric Difference
         * - For each parameter in \a this, set the active flag based on the recombination of m1 and m2
         * - For each parameter, set the parameter value in \a this to:
         *  - If \a m1 and \a m2 parameters are active, m1->val + rand(-1,4) * (m2->val - m1->val) / 3
         *  - Else if \a m1 or \a m1 parameter is active, set the value to the active parents value
         *  - Else do not set the value for the current parameter
         * 
         * @param m1 first MemeticModel
         * @param m2 second MemeticModel
         * @param method_override force the use of a specific recombination method
         */
        void    recombine(MemeticModel<T>* model1, MemeticModel<T>* model2, int method_override = -1) override;

        /** 
         * @brief Evaluate a Regressor given sample \a values
         * - In the form \f$ c_1*x_1+c_2*x_2+...c_N*x_N+c_0\} \f$, substitue each variable \f$ x_1, x_2, ..., x_N \f$
         * with the value in \a values and return the result
         *
         * @param values an array of sample values that are \f$N\f$ in length
         * @return result of the Regressors evaluation at \a values
         */
        double  evaluate(double* values) override {
            
            // Add each parameter
            double ret = 0;
            for(size_t param = 0; param < get_count()-1; param++ ) {
                if( get_active(param) )
                    ret = add(ret, multiply( get_param(param), values[param]));
            }
            
            // Add constant
            if( get_active(get_count()-1) ) ret = add(ret, get_param(get_count()-1));
            
            return ret;
        }     

        /** 
         * @brief Randomise all parameter values between +[min, max] or -[max, min] or a specific parameter when \a pos is positive
         * @param min minimium value
         * @param max maximum value 
         * @param pos position of parameter
         * @bug should update the RANDINT/RANDREAL to be RAND and overload the function. That way we can use T min and T max as params
         * @bug the min max range removes the chance of being zero, but is really not suitable for the generic version. There should just be min and max
         * @bug does this really belong here?
         */
        void    randomise(int min, int max, int pos = -1);

        /** @brief get the number of active parameters */
        size_t  get_count_active() { return term->get_count_active(); };

    private:
    
        /** @brief the Regression term */
        Term<T>* term;

};

#include <memetico/models/regression.tpp>

#endif

