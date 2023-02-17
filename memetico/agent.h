
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Agent representing a node in a memetic Population
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_AGENT_H
#define MEMETICO_AGENT_H

// Local
#include <memetico/globals.h>
#include <memetico/data.h>

// Std lib
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <chrono>

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

        /** @brief name of the objective function */
        static string   OBJECTIVE_NAME;

        /** @brief objective function to evaluate the solutions */
        static double   (*OBJECTIVE)(U*, DataSet*, vector<size_t>&);

        /** @brief local search function to explore local minima and maxima */
        static double   (*LOCAL_SEARCH)(U*, DataSet*, vector<size_t>&);

        /** Pointer to the training DataSet */
        static DataSet* TRAIN;
      
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

        /** Destruct Agent current memebers and children */
        ~Agent();

        /** @brief getter for Agent depth */
        size_t      get_depth()             { return depth; };

        /** @brief getter for Agent number */
        size_t      get_number()            { return number; };

        /** @brief getter for pocket solution */
        U*          get_pocket()            { return members[Agent<U>::POCKET]; };

        /** @brief getter for current solution */
        U*          get_current()           { return members[Agent<U>::CURRENT]; };

        /** @brief getter for Agent children */
        Agent**     get_children()          { return children; };

        /** @brief getter for Agent parent */
        Agent*      get_parent()            { return parent; };

        /** @brief setter for current */
        void        set_pocket(U* pocket)   { 
            delete members[Agent<U>::POCKET];
            members[Agent<U>::POCKET] = pocket;
        };

        /** @brief setter for pocket */
        void        set_current(U* current) { 
            delete members[Agent<U>::CURRENT];
            members[Agent<U>::CURRENT] = current;
        };

        /** @brief determine if current Agent is a lead node */
        bool        is_leaf()               { return depth == Agent<U>::MAX_DEPTH; };
        
        /** 
         * @brief Swap pocket and current if the current solution is better than the pocket 
         * @bug move the log code to another function
         */
        bool        exchange(bool doEval = true) {

            if( doEval ) 
                evaluate(Agent::TRAIN);

            if( members[Agent<U>::POCKET]->get_fitness() > members[Agent<U>::CURRENT]->get_fitness()) {

                // Master Log of construction
                memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentSwap," << number;
                memetico::master_log << ",\"" << *members[Agent<U>::POCKET] << "\"," << members[Agent<U>::POCKET]->get_fitness() << "," << members[Agent<U>::POCKET]->get_error();
                memetico::master_log << ",\"" << *members[Agent<U>::CURRENT] << "\"," << members[Agent<U>::CURRENT]->get_fitness() << "," << members[Agent<U>::CURRENT]->get_error();
                memetico::master_log << endl;

                //stat_pocket_update++;
                U* temp_model = members[Agent<U>::POCKET];
                members[Agent<U>::POCKET] = members[Agent<U>::CURRENT];
                members[Agent<U>::CURRENT] = temp_model;
                return true;
                
            }
            return false;
        }

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

        /** @brief Reinitialise the Agent current solution with a new U Model */
        void        renew() {
            delete members[Agent::CURRENT];
            members[Agent::CURRENT] = new U();
            exchange();
        };

        /**
         * @brief Evaluate Agent current solution and pocket solution based on just_current
         * @param train training DataSet
         */
        void        evaluate(DataSet* train, bool doPocket = true) {
            vector<size_t> selected = vector<size_t>();
            if( doPocket )
                Agent<U>::OBJECTIVE(members[Agent<U>::POCKET], train, selected);
            Agent<U>::OBJECTIVE(members[Agent<U>::CURRENT], train, selected);
        }

        /** 
         * @brief Ouput the Agent parameter names and values
         * 
         * @param out           Write stream 
         * @param precision     Decimal precision  
         * @param print_type    Output format from memetico::PrintType
         * @param minimal       Display smallest unit of information
         * @bug this is techncailly most appropariate to be divided between Agent and Population,
         *      but it is a little annoying to keep the function interface (without and Agent*)
         *      and look through the agents from Population. This is due to the manner we need to
         *      move from sibling to sibling. This can be solved, but put aside for the moment
         * @bug turn this into an output operator
         * 
         * @return void
         */
        void        show(ostream& out = cout, size_t precision = memetico::PREC, bool minimal = false);

        /** 
         * @brief Ouput the Agent error statistics
         * 
         * @param out           Write stream 
         * @param precision     Decimal precision  
         * 
         * @return void
         */
        void        show_errors(ostream& out = cout, size_t precision = memetico::PREC, DataSet* = nullptr);
        
        /** 
         * @brief Ouput the Agent error statistics
         * 
         * @param out           Write stream 
         * @param precision     Decimal precision  
         * 
         * @return void
         */
        void        show_solution(ostream& out, size_t precision, DataSet* train, DataSet* test);

    private:

        /** Zero-based tree depth of the Agent */
        size_t      depth;

        /** Zero-based Agent number, from left to right, top to bottom */
        size_t      number;

        /** Pocket and current member solutions */
        U**         members;

        /** Pointer to child Agents */
        Agent**     children;

        /** Pointer to parent agent */
        Agent*      parent;

};

template <class U>
size_t Agent<U>::DEGREE = 3;

template <class U>
size_t Agent<U>::MAX_DEPTH = 2;

template <class U>
size_t Agent<U>::POCKET = 0;

template <class U>
size_t Agent<U>::CURRENT = 1;

template <class U>
string Agent<U>::OBJECTIVE_NAME = "";

template <class U>
double (*Agent<U>::OBJECTIVE)(U*, DataSet*, vector<size_t>&) = nullptr;

template <class U>
double (*Agent<U>::LOCAL_SEARCH)(U*, DataSet*, vector<size_t>&) = nullptr;

template <class U>
DataSet* Agent<U>::TRAIN = nullptr;

#include <memetico/agent.tpp>

#endif
