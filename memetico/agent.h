
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Header for Agent representation
*/

#ifndef MEMETICO_AGENT_H
#define MEMETICO_AGENT_H

// Local
#include <memetico/globals.h>
#include <forms/model.h>
#include <forms/cont_frac.h>
#include <memetico/data.h>

// Std lib
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <chrono>

using namespace chrono;

/**
 * @brief The Agent class represent the population tree by linking Agents children and parents
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

    private:

        /** Zero-based tree depth of the Agent */
        size_t      depth;

    public:

        //ofstream    log;
        
        //ofstream    log_small;

        /**  */
        //size_t      stat_local_search_current;

        /**  */
        //size_t      stat_local_search_current_nm_iters;

        /**  */
        //size_t      stat_local_search_current_improve;

        /**  */
        //double      stat_local_search_current_nm_time;

        /**  */
        //size_t      stat_local_search_pocket;

        /**  */
        //size_t      stat_local_search_pocket_nm_iters;

        /**  */
        //double      stat_local_search_pocket_nm_time;

        /**  */
        //size_t      stat_local_search_pocket_improve;

        /**  */
        //size_t      stat_mutate;

        /**  */
        //size_t      stat_recombine;

        /**  */
        //size_t      stat_pocket_update;

        /**  */
        //size_t      stat_renew;

        /**  */
        //size_t      stat_bubble_up;

        /** Zero-based Agent number, from left to right, top to bottom */
        size_t          number;

        /** Pocket and current member solutions */
        U**             members;

        /** Pointer to child Agents */
        Agent**         children;

        /** Pointer to parent agent */
        Agent*          parent;

        /** Agent degree i.e. number of childern per Agent */
        static size_t   DEGREE;

        /** Maximum agent depth, identical to Population<U>::DEPTH and indicating leaf node depth */
        static size_t   MAX_DEPTH;

        /** Members position of pocket */
        static size_t   POCKET;

        /** Members position of current */
        static size_t   CURRENT;

        static DataSet* TRAIN;

        // See Implementation for details on the following function

        Agent(size_t agent_depth = 0, size_t parent_number = 0, size_t child_number = 0);

        ~Agent();

        bool        is_leaf();  
        
        void        exchange();

        void        bubble();

        void        renew();

        void        evaluate(DataSet* train);

        void        show(ostream& out = cout, size_t precision = memetico::PREC, bool minimal = false);

        //void        show_stats(ostream& out = cout, size_t precision = memetico::PREC);

        void        show_errors(ostream& out = cout, size_t precision = memetico::PREC, DataSet* = nullptr);
        
        void        show_solution(ostream& out, size_t precision, DataSet* train, DataSet* test);

        static      double (*OBJECTIVE)(U*, DataSet*, vector<size_t>&);

        static      double (*LOCAL_SEARCH)(U*, DataSet*, vector<size_t>&);

        static      string OBJECTIVE_NAME;

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
double (*Agent<U>::OBJECTIVE)(U*, DataSet*, vector<size_t>&) = nullptr;

template <class U>
double (*Agent<U>::LOCAL_SEARCH)(U*, DataSet*, vector<size_t>&) = nullptr;

template <class U>
string Agent<U>::OBJECTIVE_NAME = "";

template <class U>
DataSet* Agent<U>::TRAIN = nullptr;

#include <memetico/agent.tpp>

#endif
