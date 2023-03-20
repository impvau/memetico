
/** @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Regression is a MemeticModel that contains a single linear function 
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_REGRESSION_H_
#define MEMETICO_MODELS_REGRESSION_H_

using namespace std;

// Local
#include <memetico/helpers/print.h>
#include <memetico/helpers/excel.h>
#include <memetico/helpers/rng.h>
#include <memetico/helpers/safe_ops.h>
#include <memetico/model_base/additive_term.h>
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
        Regression(size_t param_count = 0) : MemeticModel<T>() { term = AdditiveTerm<T>(param_count); };

        /** @brief Construct Regression with a Term of param_count size */
        Regression(const Regression<T> &o) : MemeticModel<T>(o) { term = AdditiveTerm<T>(o.term); };        

        /** @brief Set the active flag for the \a pos th element of the regression term to \a val */
        void        set_active(size_t pos, bool val)    { term.set_active(pos, val); };
       
        /** @brief Set value for the \a pos th element with value \a val */
        void        set_value(size_t pos, T val)       { term.set_value(pos, val); };
        
        /** @brief Set term */
        void        set_term(AdditiveTerm<T>& new_term) { term = new_term; };

        /** @brief Return active flag at \a pos */
        bool        get_active(size_t pos)              { return term.get_active(pos); };
        
        /** @brief Return value at \a pos th element */
        double      get_value(size_t pos)               { return term.get_value(pos); };

        /** @brief Return count of elements */
        size_t      get_count()                         { return term.get_count(); };
        
        /** @brief Return TreeNode for GPU processing */
        virtual void get_node(TreeNode * n);

        /** @brief Return regression sole AdditiveTerm */
        AdditiveTerm<T>&    get_term()                  { return term; };

        /** @brief get the number of active parameters */
        size_t              get_count_active()          { return term.get_count_active(); };

        /** @brief get the number of active parameters */
        vector<size_t>      get_active_positions()      { return term.get_active_positions(); };

        void   coeff_node(TreeNode * n, float constant, int var_num);

        /** @brief Comparison operator for Regression<T> */
        bool operator== (Regression<T>& o) {

            if( !(MemeticModel<T>::operator==(o)) )
                return false;
                
            return term == o.term;
        }

        /**
         * @brief Output operator for Regression<T>
         * We output a Regression as 
         * 'coeff_val1*(var_name1)+coeff_val2*(var_name2)+...+coeff_valN*(var_nameN)+c'
         * @return os
         */
        template <class F>
        friend ostream& operator<<(ostream& os, Regression<F>& r);

        /**  
         * @brief mutate \a this MemeticModel and optionally consider another model m.
         * - A random parameter is selected uniform at random and has its active flag toggled
         * 
         * @param m another model to consider in the mutation
         */
        void    mutate(MemeticModel<T> & m) override;     

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
        void    recombine(MemeticModel<T> * model1, MemeticModel<T> * model2, int method_override = -1) override;

        /** 
         * @brief Evaluate a Regressor given sample \a values
         * - In the form \f$ c_1*x_1+c_2*x_2+...c_N*x_N+c_0\} \f$, substitue each variable \f$ x_1, x_2, ..., x_N \f$
         * with the value in \a values and return the result
         *
         * @param values an array of sample values that are \f$N\f$ in length
         * @return result of the Regressors evaluation at \a values
         */
        virtual double  evaluate(vector<double> & values);  

        virtual void print() {cout << "regres" << endl; };

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

    private:
    
        /** @brief the Regression term */
        AdditiveTerm<T> term;

};

#include <memetico/models/regression.tpp>

#endif

