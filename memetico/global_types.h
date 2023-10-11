

/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief Header defining the model type
 * 
 */

#ifndef MEMETICO_GLOBAL_TYPES_H
#define MEMETICO_GLOBAL_TYPES_H

#include <memetico/globals.h>
#include <memetico/models/mutation.h>
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac.h>

typedef double DataType;                    // Model base type; integer or double
typedef Regression<DataType> TermType;      // Model terms are Regression<double> or Regression<int>

// We use ContinuedFractionTraits to enable changing of the CF template with ease
// The first two parameters will always be TermType and DataType
// - First is the term type e.g. Regression<double>
// - Second is the datatype e.g. double
// - The third defines the mutation strategy employed
// - The fourth defines the recombination strategy employed
// - The fifth defines the local search strategy employed
// - The sixth defines the objective strategy employed

//typedef ContinuedFraction<ContinuedFractionTraits<TermType, DataType, mutation::MutateUniqueMask>> ModelType;
typedef ContinuedFractionDynamicDepth<ContinuedFractionTraits<TermType, DataType, mutation::MutateUniqueMask>> ModelType;
//typedef BranchedContinuedFraction<ContinuedFractionTraits<TermType, DataType, mutation::MutateUniqueMask>> ModelType;

#endif
