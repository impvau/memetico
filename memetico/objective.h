
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Collection of objective functions
*/

#ifndef MEMETICO_OBJECTIVE_H
#define MEMETICO_OBJECTIVE_H

#include <memetico/data.h>
#include <memetico/model_base/model.h>

namespace objective {

// See Implementation for details

template <class U>
double mse(U* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double nmse(U* model, DataSet* train, vector<size_t>& selected = vector<size_t>());

template <class U>
double compare(U* m1, U* m2, DataSet* train);

}

#include <memetico/objective.tpp>

#endif