

/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Header for global variables and functions
 * 
 */

#ifndef MEMETICO_GLOBALS_H
#define MEMETICO_GLOBALS_H

#include <chrono>
#include <memetico/helpers/rng.h>
#include <regex>

/**
 * @brief Capture similarity data between two solutions and provide < operator
 * such that a vector<Similar> can be sorted
 */
struct Similar
{
    size_t i, id, iv, j, jd, jv;
    double dist;
    Similar(size_t ipos, size_t idepth, size_t iivs, 
            size_t jpos, size_t jdepth, size_t jivs, double dist_val) : 
                i(ipos), id(idepth), iv(iivs), 
                j(jpos), jd(jdepth),  jv(jivs), dist(dist_val) {};
    bool operator < (const Similar& s2) const
    {
        return (dist < s2.dist);
    }
};


/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * 
 * @brief Global variables for memetic algorithm
 * 
 */ 
namespace meme {

/** Flag to use GPU */
extern bool             GPU;

/** Program seed value for reproducibility */
extern uint_fast32_t    SEED;

/** Number of generations of evolution  */
extern size_t           GENERATIONS;

/** Chance of mutation for each agent during the evolution process  */
extern double           MUTATE_RATE;

/** Penalty factor added for each parameter used Fitness = Error * (1+params_used()*PENALTY) */
extern double           PENALTY;

/** Run local search after this many generations  */
extern size_t           LOCAL_SEARCH_INTERVAL;

/** Number of times to repeat local search for each agent each generation  */
extern size_t           LOCAL_SEARCH_RUNS;

/** Percentage of data to consider in local search  */
extern double           LOCAL_SEARCH_DATA_PCT;

/** Number of Neader-Mead  iterations before the model is stale  */
extern size_t           NELDER_MEAD_STALE;

/** Mumber of iterations of the Neader-Mead  */
extern size_t           NELDER_MEAD_MOVES;

/** Numnber of stagnant generates to re-randomise the currents */
extern size_t           STALE_RESET;

/** Integer only models */
extern bool             INT_ONLY;

/** File where data resides */
extern string           TRAIN_FILE;

/** File where data resides */
extern string           TEST_FILE;

/** Output folder */
extern string           LOG_DIR;

/** Global double randomisation object */
extern RandReal         RANDREAL;

/** Global integer randomisation object */
extern RandInt          RANDINT;

/** Global real precision within the application */
extern size_t           PREC; 

/** Global DEBUG flag for function tracing */
extern bool             DEBUG;

/** Global Current Generation */
extern size_t           GEN;

/** Maximum number of seconds the program can run for */
extern long int         MAX_TIME;

/** Run time of program */
extern long int         RUN_TIME;

extern bool             DEBUG;

/** Master log */
extern ofstream         master_log;

extern FILE*            STD_OUT;
extern FILE*            STD_ERR;

// Custom Globals

/* Current Pocket Depth for Adaptive fraction */
extern size_t           POCKET_DEPTH;


extern size_t                   DIVERSITY_COUNT;

}

#endif

