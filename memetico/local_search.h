

/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Collection of local search functions
*/

#ifndef MEMETICO_LOCAL_SEARCH_H
#define MEMETICO_LOCAL_SEARCH_H

#include <memetico/agent.h>
#include <memetico/globals.h>
#include <limits>

namespace local_search {

template <class U>
double model_evaluate(vector<double>* params, vector<size_t> positions, U* model, DataSet* data, vector<size_t>&); 

template <class U>
double custom_nelder_mead(U* model, DataSet* data, vector<size_t>&);

size_t nm_iters;

unsigned int nm_duration;


}

#include <memetico/local_search.tpp>

#endif