
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief A class representing a Model that has fitness, error and penalty but may apply to TSP, SR or other problems
 */

#ifndef MEMETICO_MODELS_MODEL_H_
#define MEMETICO_MODELS_MODEL_H_

using namespace std;
#include <vector>
#include <limits>
#include <string>
#include <memetico/data/data_set.h>
#include <memetico/gpu/tree_node.h>
#include <memetico/helpers/print.h>

using namespace cusr;

/**
 * @brief A class representing a Model that has fitness, error and penalty but may apply to TSP, SR or other problems
 * 
 * The class implements basic attributes that are common to all models, applicable equally to the TSP to Regression problems.
 * We must extend this class to handle these problems, where the TSP may implement a distance matrix and a regression problem
 * may store its elements in a Term object. Key functions are specific which must be implemented for all problems, such as
 * evaluate() and setters/getters for common varaibles such as error, penalty and fitness.
 * 
 * The template is used for the benefit of the derived classes which may be of simple types such as double or more complex
 * types such as Regression<double> or even ContinuedFractions<Regression<double>>
 */
class Model {
    
    public:

        /** @brief Construct Model with max error, fitness and no penalty */
        Model() {
            penalty = 1;
            error = numeric_limits<double>::max();
            fitness = numeric_limits<double>::max();
        }

        /** @brief Copy Model o into this */
        Model(const Model &o) {

            // Copy fitness
            penalty = o.penalty;
            error = o.error;
            fitness = o.fitness;
    
        }
            
        ~Model() {
            if( node != nullptr ) {
                delete node;
                node = nullptr;
            }
        }

        /** @brief setter for penalty */
        void            set_penalty(double val)     { penalty = val; };

        /** @brief setter for error */
        void            set_error(double val)       { error = val; };
        
        /** @brief setter for fitness */
        void            set_fitness(double val)     { fitness = val; };
        
        /** @brief getter for penalty */
        double          get_penalty()               { return penalty; };

        /** @brief getter for error */
        double          get_error()                 { return error; };

        /** @brief getter for fitness */
        double          get_fitness()               { return fitness; };
        
        /** @brief Return TreeNode for GPU evaluation */
        virtual void    get_node(TreeNode * n)      {};

        /** 
         * @brief evaluate the fiteness of the model
         * @param values 
         * @return model error
         * @bug technically should be type T for values? What if we have a TSP problem that
         * takes a graph? Is double appropriate for an error score in all cases?
         */
        virtual double  evaluate(vector<double> & values) { return numeric_limits<double>::max(); };
        
        virtual void    print()                     {   cout << "model" << endl;};

        virtual string  str()                       {   return "model";};

        /** @brief output operator for a model */
        friend ostream& operator<<(ostream& os, Model& m);

        /** @brief Comparison operator for Model */
        bool operator== (Model& o) {

            if( penalty != o.penalty )
                return false;

            if( fitness != o.fitness )
                return false;

            if( error != o.error )
                return false;

            return true;
        }
        
        /** @brief Format to print all regressions as */
        static PrintType        FORMAT;

        /** @brief TreeNode representation for GPU processing */
        TreeNode* node = nullptr;

    private:

        /** @brief Penalty associated to the Model */
        double  penalty;                   

        /** @brief Error associated to the Model after running the loss function */
        double  error;

        /** @brief Fitnress of the model that multiplies penalty with error */
        double  fitness;

};

#endif

