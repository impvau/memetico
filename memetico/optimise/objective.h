
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Collection of objective functions
*/

#ifndef MEMETICO_OBJECTIVE_H
#define MEMETICO_OBJECTIVE_H

#include <memetico/helpers/safe_ops.h>
#include <memetico/data/data_set.h>
#include <memetico/model_base/model.h>
#include <memetico/model_base/model_meme.h>
#include <memetico/gpu/cuda.cuh>
#include <memetico/globals.h>

namespace objective {

// See Implementation for details

template <class U>
double mse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double mse2(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double cuda_mse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double nmse(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double compare(MemeticModel<U>* m1, MemeticModel<U>* m2, DataSet* train);

}

#include <memetico/optimise/objective.tpp>

#endif