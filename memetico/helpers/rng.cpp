
/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * 
 * @brief Wrapper classes for global int and double random number generation
 * 
 */

#include <memetico/helpers/rng.h>

RandInt*        RandInt::RANDINT = nullptr;
RandReal*       RandReal::RANDREAL = nullptr;

