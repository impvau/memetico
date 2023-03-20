
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
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
double mse(Model* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double mse2(MemeticModel<U>* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double cuda_mse(U* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double nmse(U* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double compare(U* m1, U* m2, DataSet* train);

}

#include <memetico/optimise/objective.tpp>

#endif