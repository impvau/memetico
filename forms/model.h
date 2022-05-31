
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
#include <memetico/data.h>

// Std Lib
#include <stdlib.h>
#include <typeinfo>
#include <iomanip>
#include <limits>

/**
 * @brief The Model class is the lowest-level representation of a solution used within an Agent
 * 
 * The class implements basic attributes that are common to all models, and these apply irrespective of a TSP solution,
 * a regression solution, or a continued fractions solution. As the Agent class works with Models but these are pointers 
 * to derived classes, no changes are required in the core MA as new representations are used for models. The Population 
 * and Agent classes can be exteded where non-generic behaviour needs to be overriden outside of the key functions customisable
 * in Model-derived classes.
 * 
 * We expect the template T to be a basis type such as int or double
 * 
 */
template<class T>
class Model {
    
    protected:

        /** @brief Penalty associated to the Model */
        double  penalty;                   

        /** @brief Error associated to the Model after running the loss function */
        double  error;

        /** @brief Fitnress of the model that multiplies penalty with error */
        double  fitness;

        /** @brief Number of optimisable parameters in the model. */
        size_t  count;        

        /** @brief Array of parameters of type T */
        T*      params;                    

        /** @brief Active state for any parameter  */
        bool*   active;

    public:

        // Construction
        Model(size_t param_count = 0);
        virtual ~Model();                   
        
        // Setters
        virtual void    set_active(size_t param, bool val)  { if( param < count) active[param] = val; };
        virtual void    set_param(size_t param, T val)      { if( param < count) params[param] = val;}
        void            set_penalty(double val)             { penalty = val; };
        void            set_error(double val)               { error = val; };
        void            set_fitness(double val)             { fitness = val; };
        
        // Getters
        virtual bool    get_active(size_t param)    {return false;};
        virtual double  get_param(size_t param)     {return 0;};
        double          get_penalty()               { return penalty; };
        double          get_error()                 { return error; };
        double          get_fitness()               { return fitness; };
        size_t          get_count()                 { return count;};
        

        // Basic operators
        template<class F>
        friend ostream& operator<<(ostream& os, const Model<F>& m);

        // Basic Functions
        virtual Model<T>* clone();       

        // Class Functions
        virtual void    mutate(Model<T>* m = nullptr) {};
        virtual void    recombine(Model<T>* m1, Model<T>* m2, int method_override = -1) {};
        virtual size_t  params_used() { return 0; };
        virtual double  evaluate(double* values) { return numeric_limits<double>::max(); }

        // To delete these later
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

