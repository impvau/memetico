
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
class ContinuedFraction : public Model<T> {

    private:

        /** Global active flag that applies for the independent varaible down all depths and overrides any setting in the active array */
        bool*   global_active;

        /** Zero-based depth of the fraction */
        size_t  depth;

        /** Number of terms in the fraction defined as \f$2*depth+1\f$ where the first depth has only one term and each subsequent depth has an additional 2 terms */
        size_t  terms;

        /** Number of parameters in each term. As this is a linear model we have Data::IVS.size()+1 where the +1 is the constant*/
        size_t  params_per_term;

        T** objs;

        // See implementation for details on the following functions

        //void    randomise_at(size_t frac_term, size_t frac_param, int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

        //void    randomise_all(int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

        void    randomise(int min = ContinuedFraction<T>::RAND_LOWER, int max = ContinuedFraction<T>::RAND_UPPER);

    public:

        /** Default lower bound on randomisations */
        static int      RAND_LOWER;

        /** Default upper bound on randomisations */ 
        static int      RAND_UPPER;

        /** Depth of the fraction */ 
        static size_t   DEPTH;


        // See implementation for details on the following functions
        //ContinuedFraction();
        ContinuedFraction(size_t frac_depth = ContinuedFraction<T>::DEPTH);

        ~ContinuedFraction();

        ContinuedFraction<T>* clone() override;

        double  evaluate(double* values);

        void    mutate(Model<T>* pocket = nullptr) override;

        void    recombine(Model<T>* model1, Model<T>* model2);

        size_t  params_used() override;

        void    show(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel) override;

        void    show_min(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel, bool do_summary = true) override;

        size_t  get_depth()                         { return depth;  };
        size_t  get_terms()                         { return terms;  };
        size_t  get_params_per_term()               { return params_per_term; };
        size_t  get_param_count()                   { return params_per_term*terms; };
        bool    get_global_active(size_t iv)        { return global_active[iv]; };
        T*      get_objs(size_t term)               { return objs[term]; };

        vector<size_t>  get_active_positions();
        double          get_param(size_t pos);
        bool            get_active(size_t pos);
        void            set_param(size_t active_param, double val);
        void            set_objs(size_t pos, T* obj)    { objs[pos] = obj; };

        void    set_global_active(size_t iv, bool val);

        template<class F>
        friend ostream& operator<<(ostream& os, const ContinuedFraction<F>& c);


};

template<class T>
int ContinuedFraction<T>::RAND_LOWER = 1;

template<class T>
int ContinuedFraction<T>::RAND_UPPER = 3;

template<class T>
size_t ContinuedFraction<T>::DEPTH = 4;


#include <forms/cont_frac.tpp>

#endif
