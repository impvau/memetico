
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

// Local
#include <memetico/globals.h>
#include <memetico/data.h>
#include <forms/model.h>

// Std Lib
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <iomanip>
#include <iostream>

/**
 * @brief The Regression class extends Model for symbolic regression representation
 * 
 * The class implements a simple linear regression model \f$cx+c_0\f$
 * Where
 * - \f$x\f$ is the set of independent variables \f$x = \{x_1, x_2,...,x_n\} \f$
 * - \f$x\f$ is the set of coefficients for each independent varaible \f$c = \{c_1, c_2,...,c_n\} \f$ 
 * - \f$c_0\f$ is the constant 
 * 
 * This class can be extended to representations such as 'Continued Fraction', 'Power Law', 'Engel Expansion' etc.
 * 
 */
template<class T>
class Regression : public Model<T> {
       
    public:

        // See Implementation for function details

        double  evaluate(double* values, size_t from_param = 0) override;      

        bool    is_constant(size_t param_no);          

        //void    mutate(Model<T>* pocket) override;     

        //void    recombine(Model<T>* model1, Model<T>* model2, int min, int max) override;    

        void    show(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel) override;    

        void    show_min(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel, bool do_summary = true) override;


};


#include <forms/regression.tpp>

#endif

