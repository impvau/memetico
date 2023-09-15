
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Agent representing a node in a memetic Population
 */

#ifndef MEMETICO_AGENT_H
#define MEMETICO_AGENT_H

// Local
#include <memetico/data/data_set.h>
#include <memetico/helpers/safe_ops.h>

// Std lib
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <chrono>

using namespace std;
using namespace chrono;

/**
 * @brief Agent represents a node in the memetic Population tree and links parent and children Agents
 *
 * The root Agent (agent 0) is the agent at depth 0 that contains no parent pointer. The root agents number of children
 * are defined by Agent<U>::DEGREE and the chain is terminated at Agent<U>::MAX_DEPTH where the children will be a
 * nullptr.
 * 
 * Each agent will contain a 'pocket' as the best solution for the agent and a 'current' which is solution being modified
 * by the MA and replaces the pocket when it betters the pocket in terms of fitness. These are stored within the memebers
 * attribute and are of template U type that are derived classes from Model<T> and override the mutation,
 * recombination, evalutation, show, etc. functions such that new types are introduced without modification to the core 
 * memetic algorithm.
 * 
 * The Agent class takes on responsibility of swapping pocket and current solutions and bubbling child solutions of the current Agent
 * with the parent solution. Other tree behaviors are regulated by the Population class, including the recusive bubbling process that
 * spans the entire tree and not just an individual Agent and its children.
 * 
 * The static memebers Agent<U>::OBJECTIVE and Agent<U>::LOCAL_SEARCH hold the functions that the tree uses to evaluate the class U Models
 * i.e. they take in a U* and a DataSet* to perform these functions
 *  
 */
template <class U>
class Agent {

    public:

        /** @brief Agent degree i.e. number of childern per Agent */
        static size_t   DEGREE;

        /** @brief Maximum agent depth, identical to Population<U>::DEPTH and indicating leaf node depth */
        static size_t   MAX_DEPTH;

        /** @brief Members position of pocket solution */
        static size_t   POCKET;

        /** @brief Members position of current solution */
        static size_t   CURRENT;

        /** Pointer to the training DataSet */
        //static DataSet* TRAIN;
      
        /**
         * @brief Construct Agent with the provided depth, parentnumber, degree number (0 to DEGREE)
         * - Set \a depth to \a agent_depth
         * - Set the agent number e.g. for a depth 3 tree
         *  - On the zeroth depth, 0 for the root agent
         *  - On the first depth, 1, 5, 9
         *  - On the second depth 2,3,4 6,7,8 10,11,12
         * - Create memeber solutions of type U for the pocket and current positions
         * - Recursively construct the children agents
         * - Set the childs parent
         * - Set leaf nodes children to nullptr
         * - Set root nodes parent to nullptr
         *
         * Exceptions
         * - throws runtime_error() When Agent::DEGREE is not assigned to a non-zero value before consutrction
         * 
         * @param agent_depth Zero-based agent tree depth
         * @param parent_number Zero-based agent number, from top to bottom, left to right
         * @param degree_number Zero-based child position between 0 to Agent::DEGREE
         * 
         * @return  Agent
         */
        Agent(size_t agent_depth = 0, size_t parent_number = 0, size_t degree_number = 0);

        ~Agent();

        /** @brief getter for Agent depth */
        size_t      get_depth()             { return depth; };

        /** @brief getter for Agent number */
        size_t      get_number()            { return number; };

        /** @brief getter for pocket solution */
        U&          get_pocket()            { return members[Agent<U>::POCKET]; };

        /** @brief getter for current solution */
        U&          get_current()           { return members[Agent<U>::CURRENT]; };

        /** @brief Return list of memebers */
        vector<U>&  get_members()           { return members; };

        /** @brief getter for Agent children */
        vector<Agent<U>*> get_children()    { return children; };

        /** @brief getter for Agent parent */
        Agent<U>*   get_parent()            { return parent; };

        /** @brief setter for current */
        void        set_pocket(U& pocket)   { members[Agent<U>::POCKET] = pocket; };

        /** @brief setter for pocket */
        void        set_current(U& current) { members[Agent<U>::CURRENT] = current; };

        /** @brief determine if current Agent is a lead node */
        bool        is_leaf()               { return depth == Agent<U>::MAX_DEPTH; };
        
        /** 
         * @brief Swap pocket and current if the current solution is better than the pocket 
         * @bug move the log code to another function
         */
        void        exchange();

        /**
         * @brief Bubble a single Agent with its children
         * 
         * - Determine the child with the fittest pocket
         * - If the fittest child pocket is more fit than the parent pocket
         * -- Trade the child and parent pocket members
         * -- exchange() the child to ensure pocket and current are correct
         * 
         * @return  void
         */
        void        bubble();

        /** 
         * @brief Ouput the Agent parameter names and values
         * 
         * @param out           Write stream 
         * @param precision     Decimal precision  
         * @param print_type    Output format from meme::PrintType
         * @param minimal       Display smallest unit of information
         * @bug this is techncailly most appropariate to be divided between Agent and Population,
         *      but it is a little annoying to keep the function interface (without and Agent*)
         *      and look through the agents from Population. This is due to the manner we need to
         *      move from sibling to sibling. This can be solved, but put aside for the moment
         * @bug turn this into an output operator
         * 
         * @return void
         */
        void        show(ostream& out = cout, size_t precision = 18, bool minimal = false);

    private:

        /** Zero-based tree depth of the Agent */
        size_t      depth;

        /** Zero-based Agent number, from left to right, top to bottom */
        size_t      number;

        /** Pocket and current member solutions */
        vector<U>   members;

        /** Pointer to child Agents */
        vector<Agent<U>*> children;

        /** Pointer to parent agent */
        Agent<U>*   parent;

};

template <class U>
size_t Agent<U>::DEGREE = 3;

template <class U>
size_t Agent<U>::MAX_DEPTH = 2;

template <class U>
size_t Agent<U>::POCKET = 0;

template <class U>
size_t Agent<U>::CURRENT = 1;

#include <memetico/population/agent.tpp>

#endif
