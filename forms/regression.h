
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for Regression Model representation
 * 
 */

#ifndef FORMS_REGRESSION_H
#define FORMS_REGRESSION_H

using namespace std;
using namespace memetico;

// Local
#include <memetico/globals.h>
#include <memetico/data.h>
#include <forms/term.h>
#include <forms/model.h>

// Std Lib
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <iomanip>
#include <iostream>

/**
 * @brief A Model for Regression problems
 * A simple linear Model in the form \f$cx+c_0\f$
 * Where
 * - \f$x\f$ is the set of independent variables \f$x = \{x_1, x_2,...,x_n\} \f$
 * - \f$x\f$ is the set of coefficients for each independent varaible \f$c = \{c_1, c_2,...,c_n\} \f$ 
 * - \f$c_0\f$ is the constant 
 */
template<class T>
class Regression : public Model<T> {
    
    protected:

        

    public:

        Term<T>* term;

        // Construction 
        Regression(size_t size = 0) : Model<T>() { term = new Term<T>(size); };

        // Setters
        void    set_active(size_t pos, bool val)    { term->get_elem(pos)->set_active(val); };
        void    set_param(size_t pos, T val)        { term->get_elem(pos)->set_value(val); };
        
        // Getters
        bool    get_active(size_t pos)  { return term->get_elem(pos)->get_active(); };
        double  get_param(size_t pos)   { return term->get_elem(pos)->get_value(); };
        size_t  get_count()             { return term->get_count(); };

        // Basic Operators
        template<class F>
        friend ostream& operator<<(ostream& os, const Regression<F>& r);

        // Basic Functions
        Regression<T>* clone();

        // Class Functions
        void    mutate(Model<T>* m = nullptr) override;     
        void    recombine(Model<T>* model1, Model<T>* model2, int method_override = -1) override;
        double  evaluate(double* values) override;      
        void    randomise(int min, int max);

        size_t  used();

        // To remove 
        //void    show(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel) override;    
        //void    show_min(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel, bool do_summary = true) override;       

};


#include <forms/regression.tpp>

#endif

