
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief A Model representing a machine learning solution
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_MODEL_H_
#define MEMETICO_MODELS_MODEL_H_

using namespace std;

#include <memetico/globals.h>
#include <memetico/data.h>

/**
 * @brief The Model class representing the lowest-level representation of a solution
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

        /** */
        virtual ~Model() {};                   
        
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
        
        /**
         * @brief evaluate the fiteness of the model
         * @param values 
         * @return model error
         * @bug technically should be type T for values? What if we have a TSP problem that
         * takes a graph? Is double appropriate for an error score in all cases?
         */
        virtual double  evaluate(double* values) { return numeric_limits<double>::max(); }
        
        /** @brief output operator for a model */
        friend ostream& operator<<(ostream& os, Model& m) {
            os << "err: " << m.get_error() << " pen:" << m.get_penalty() << " fit:" << m.get_error();
            return os;
        }

        /** 
         * @brief clone a model
         * @bug need to make this a copy constructor
         */
        virtual Model* clone() {
            
            // Create new structure
            Model* o = new Model();

            // Copy fitness
            o->penalty = penalty;
            o->error = error;
            o->fitness = fitness;
    
            return o;
        }   

    private:

        /** @brief Penalty associated to the Model */
        double  penalty;                   

        /** @brief Error associated to the Model after running the loss function */
        double  error;

        /** @brief Fitnress of the model that multiplies penalty with error */
        double  fitness;

};

#endif

