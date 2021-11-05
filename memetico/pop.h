
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Header for the Population class that manages Agents
*/

#ifndef MEMETICO_POPULATION_H
#define MEMETICO_POPULATION_H

// Local
#include <memetico/globals.h>
#include <memetico/objective.h>
#include <memetico/local_search.h>
#include <memetico/agent.h>
#include <chrono>

// Std lib

/**
 * Class managaing a population of Agents
 * 
 */

/**
 * @brief The Population class manages a series of Agents through a linked-list-like structure
 * 
 * The Population is controlled through the root_agent which contains a parent of nullptr and Agent<U>::DEGREE 
 * number of children. These intern, have Agent<U>::DEGREE children constructing a M-ary tree of Agent<U>::DEGREE
 * The leaf Agents contain nullptr children
 * 
 * The class generally manages the looping process that are executed on all Agents, and calls Agent memeber functions 
 * to achieve the outcome for each Agent individually. Examples are local_search, evolve, buble, and evaluate which 
 * loop from root to leaf, or leaf to root and trigger Agent member functions.
 * 
 * In addition, the population relates to the data attribute which is the training set that is passed into the constructor
 *
 * The Population class does not obersve the selected objective function or local search function, as these can vary based on user
 * input. Rather pointers to these functions are maintined in the Agent class as static variables. This placement is arguable,
 * but for sake of avoiding cyclic dependencies this is the current soliition (e.g. dependency is Agent requiring import of Population 
 * that contains the OBJECTIVE function)
 * 
 * The core function of the Population, and primary execution of the MA is the run member function that iterates for a number of 
 * generations and triggers the evolution and shuffling processes on the population
 * 
 */
template <class U>
class Population {

    private:


    public:

        /** Depth of the population where leaf nodes appear */
        static size_t   DEPTH;

        /** Dataset used for evolution */
        DataSet*        data;

        /** Agent node at the root of the population */
        Agent<U>*       root_agent;


        // See implementation for details on the following functions

        Population(DataSet* train_data);

        ~Population();

        void run();

        void local_search(Agent<U>* agent = nullptr);

        void evolve(Agent<U>* agent = nullptr);

        void bubble(Agent<U>* agent = nullptr, size_t child_number = 0);
        
        void evaluate(Agent<U>* agent = nullptr);

        void generational_summary(Agent<U>* agent = nullptr);

};

template <class U>
size_t Population<U>::DEPTH = 2;

// We must include the cpp code for the compiler to detect possible templates
#include <memetico/pop.tpp>

#endif