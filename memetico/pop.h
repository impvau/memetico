
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Population of Agents
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_POPULATION_H
#define MEMETICO_POPULATION_H

// Local
#include <memetico/globals.h>
#include <memetico/objective.h>
#include <memetico/local_search.h>
#include <memetico/agent.h>
#include <chrono>

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

    public:

        /** Depth of the population where leaf nodes appear */
        static size_t   DEPTH;

        /** Dataset used for evolution */
        DataSet*        data;

        /** Agent node at the root of the population */
        Agent<U>*       root_agent;

        /**
         * @brief Construct Population with a tree of memetico::POP_DEPTH and degree memetico::POP_DEGREE
         * - Create a tree of \f$ \frac{o^{d+1}-1}{o-1} \f$ Agents where o is the n-ary order, POP_DEGREE and d is the zero-based depth of the tree, POP_DEPTH
         * - Assign data to \a train_data
         * - Evaluate and bubble solutions
         * - Print initial population
         * 
         * @param train_data dataset to evaluate the population
         */
        Population(DataSet* train_data);

        /** @brief Deconstruct Population by deleting agents, their children and the reference to the data */
        ~Population() {
            delete root_agent;
            delete data;
        };

        /** 
         * @brief run the generational process
         * - Run for memetico::GENERATIONS
         *  - Evolve the population
         *  - Undertake local search
         *  - Reset the root current solution when best fitness does not change formemetico::STALE_RESET iterations
         *  - Update the current depth of the best solution (POCKET_DEPTH)
         *  - Print result for generation
         * - Output optimisation result
         */
        void run();

        /**
         * @brief Run local search on the population
         * - Detect what part of the Population to evolve from
         *  - If \a agent is not specified, set \a agent to the root agent
         *  - If agent is specificed, apply from \a agent down the tree
         * - Traverse the child agents to the leaf nodes
         * - Perform local search on the current solution LOCAL_SEARCH_RUNS times
         *  - Select uniform at random, LOCAL_SEARCH_DATA_PCT of the data
         *  - Run local search
         * - Update the current solution if a fitter solution is found
         * - Perform local search on the pocket solution LOCAL_SEARCH_RUNS times
         *  - Select uniform at random, LOCAL_SEARCH_DATA_PCT of the data
         *  - Run local search
         * - Update the pocket solution if a fitter solution is found
         * - Exchange the pocket and current if the current is fitter
         * - Bubble the pocket if it was exchanged
         * 
         * @param agent the Agent to evolve recursively down the Population
         */
        void local_search(Agent<U>* agent = nullptr);

        /**
         * @brief evole all agents in the Population
         * - Detect what part of the Population to evolve from
         *  - If no agent is specified, apply to the entire Population
         *  - If an agent is specificed, apply from \a agent down the tree
         * - Traverse the child agents to the leaf nodes
         * - Perform mutation with probability of MUTATE_RATE
         * - If at a leaf node, exit
         * - Else perform recombination
         *   - Set the Agents last childs current to the recombination of the Agents pocket and the last childs current
         *   - Set the Agents first child to the recombination of the Agents first childs pocket and the next childs current  
         *   - Continue the last recombination for the degree of the node (excluding the last child as it is was previously set)
         *   - Note that the parent was already recombined
         * 
         * @param agent the Agent to evolve recursively down the Population
         */
        void evolve(Agent<U>* agent = nullptr);

        /**
         * Bubble fitter children pocket solutions up the population
         *
         * @return  void
         * @bug two solutions on depth 2 may increase and be better than the pocket. The best will be bubbled to the root
         *      and the previous root pocket will be placed on depth 1. In this event, it is not bubbled with the children
         *      of depth 2 which may bethe second solution that is fitter. This leads to an (unlikely) scenario where the 
         *      previous pocket on depth 1 is less fit than an agent on depth 2. This is corrected in the next iteration,
         *      but if utilising deeper trees this could lead to larger issues. This has been observed with seed 2073724172
         *      and code as of 210921
         */
        void bubble(Agent<U>* agent = nullptr, size_t child_number = 0);
        
        /** @brief evaluate the entire Population */
        void evaluate(Agent<U>* agent = nullptr);

};

template <class U>
size_t Population<U>::DEPTH = 2;

// We must include the cpp code for the compiler to detect possible templates
#include <memetico/pop.tpp>

#endif