
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @version 1.0
 * @brief Header for ContinuedFraction Model representation
*/

#ifndef FORMS_CONT_FRAC_H
#define FORMS_CONT_FRAC_H

// Local
#include <memetico/globals.h>
#include <forms/regression.h>

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
class ContinuedFraction : public Regression<T> {

    private:

        /** Global active flag that applies for the independent varaible down all depths and overrides any setting in the active array */
        bool*   global_active;

        /** Zero-based depth of the fraction */
        size_t  depth;

        /** Number of terms in the fraction defined as \f$2*depth+1\f$ where the first depth has only one term and each subsequent depth has an additional 2 terms */
        size_t  terms;

        /** Number of parameters in each term. As this is a linear model we have Data::IVS.size()+1 where the +1 is the constant*/
        size_t  params_per_term;

        // See implementation for details on the following functions

        void    randomise_at(size_t frac_term, size_t frac_param, int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

        void    randomise_all(int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

    public:

        /** Default lower bound on randomisations */
        static int      RAND_LOWER;

        /** Default upper bound on randomisations */ 
        static int      RAND_UPPER;

        /** Depth of the fraction */ 
        static size_t   DEPTH;


        // See implementation for details on the following functions

        ContinuedFraction(size_t frac_depth = ContinuedFraction<T>::DEPTH, bool do_log = false);

        ~ContinuedFraction();

        ContinuedFraction<T>* clone() override;

        double  evaluate(double* values, size_t from_param = 0) override;

        void    mutate(Model<T>* pocket) override;

        //void    recombine(Model<T>* model1, Model<T>* model2, int min, int max) override;

        size_t params_used() override;

        void    show(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel) override;

        void    show_min(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel, bool do_summary = true) override;

};

template<class T>
int ContinuedFraction<T>::RAND_LOWER = 1;

template<class T>
int ContinuedFraction<T>::RAND_UPPER = 3;

template<class T>
size_t ContinuedFraction<T>::DEPTH = 4;


#include <forms/cont_frac.tpp>

#endif
