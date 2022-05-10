
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for the Model class
 * 
 */

#ifndef FORMS_MODEL_H
#define FORMS_MODEL_H

using namespace std;

// Local
#include <memetico/globals.h>

// Std Lib
#include <stdlib.h>
#include <typeinfo>
#include <iomanip>
#include <limits>

/**
 * @brief The Model class is the lowest-level representation of a solution used by the Memetic Algorithm Agents
 * 
 * The class implements basic attributes that are common to all model solutions such as fitness, error and penalty
 * These apply irrespective of using the MA to solve a TSP, graph-based, or regression problem
 * Extension of the class is performed to provide the problem-specific attributes required, such as the separation
 * of params into coefficient and contants within the Regression class.
 * 
 * As the Agent class works with Models but these are pointers to derived classes, no changes are required in the 
 * core MA as new representations are used for models.
 * 
 * This is a template class designed specifically for int and double model types, but it could be leveraged for 
 * a paramter type T which could also enable sin(x), e^x, etc.
 * 
 */
template<class T>
class Model {
       
    public:

        /** Penalty for the number of paramters used */
        double  penalty;                   

        /** Error of the fraction given the objective function Model<T>::OBJECTIVE */
        double  error;

        /** Fraction fitness as a combination of error and penalty */
        double  fitness;    

        /** @brief Number of optimisable parameters in the model. 
         * 
         * The base clases use this structure differently such as the Regression class has Data:IVS.size() 
         * coefficients and a constant within the paramters. For the ContinuedFraction class the Regression
         * paramters are repeated for a certain number of terms and there are associated 
         */
        size_t  count;

        /** @brief Value of the optimisable paramters in the model.
         * 
         *  params is of the template type T, specified when constructing the model
         */
        T*      params;                    

        /** @brief Mask/active flag for each parameter */
        bool*   active;

        Model(size_t model_params = 0);     // See implementation
        
        virtual ~Model();                   // See implementation
        
        virtual Model<T>* clone();          // See implementation

        void    allocate();                 // See implementation

        T       random(T min, T max);       // See implementation

        /** 
         * @brief Basic evaluation of the model as implemented by derived classes
         * 
         * Requires implementation from derrived classes        
         *  
         * @return double value of model prediction
         */
        virtual double evaluate() { return numeric_limits<double>::max(); }

        /** 
         * @brief Evaluate a Model given sample values
         *  
         * Requires implementation from derrived classes        
         * 
         * @param values an array of sample values to evaluate at
         * @param from_param starting point for partial evaluation of the model
         *                   an example usefullness is evaluating a single Regression equation
         *                   in a ContinuedFraction which contains mutliple sequential equations
         *                   for each term in the fraction
         *                   Defaults to 0
         * 
         * @return double value of model prediction
         */
        virtual double evaluate(double* values, size_t from_param = 0) { return numeric_limits<double>::max(); }

        virtual void mutate(Model<T>* pocket = nullptr);                                  

        virtual void recombine(Model<T>* model1, Model<T>* model2, int min, int max); 

        virtual size_t params_used();

        /** 
         * @brief Output the Model to out stream
         * 
         * Requires implementation from derrived classes
         * 
         * @param out           Write stream 
         *                      Defaults to std::cout
         * 
         * @param precision     Decimal precision
         *                      Defaults to memetico::PREC
         * 
         * @param from_param    Parameter to being from. See Model<T>::evaluate() for further detail
         *                      Defaults to 0
         * 
         * @param print_type    Output format from memetico::PrintType
         *                      Defaults to memetico::PrintExcel
         * 
         * @return void
         */
        virtual void show(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel) {}

        virtual void show_min(ostream& out = cout, size_t precision = memetico::PREC, size_t from_param = 0, memetico::PrintType print_type = memetico::PrintExcel, bool do_summary = true);

};

#include <forms/model.tpp>

#endif

