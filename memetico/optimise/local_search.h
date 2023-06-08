

/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Collection of local search functions
*/

#ifndef MEMETICO_LOCAL_SEARCH_H
#define MEMETICO_LOCAL_SEARCH_H

#include <memetico/population/agent.h>
#include <memetico/globals.h>
#include <limits>

using namespace meme;

namespace local_search {

template <class U>
double model_evaluate(vector<double>* params, vector<size_t> positions, U* model, DataSet* data, vector<size_t>&); 

template <class U>
double custom_nelder_mead(U* model, DataSet* data, vector<size_t>&);

template <class U>
double custom_nelder_mead_redo(U* model, DataSet* data, vector<size_t>&);

template <class U>
double custom_nelder_mead_alg4(U* model, DataSet* data, vector<size_t>&);

//size_t nm_iters;
//unsigned int nm_duration;

template <typename Operator>
vector<double> vo(vector<double> a, vector<double> b, Operator);
vector<double> vm(vector<double> a, double b);
vector<double> contr(vector<double> a, vector<double> b);
vector<double> refl(vector<double> a, vector<double> b);
vector<double> expa(vector<double> a, vector<double> b);

}

//template <class T>
//ostream& operator << (ostream& os, const vector<T>& v);

#include <memetico/optimise/local_search.tpp>

#endif