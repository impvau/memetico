
/**
 * @file
 * @author andy@impv.au
 * @version 1.0
 * @brief hashing tools
 * 
 */

#ifndef MEMETICO_HELPER_HASH_H_
#define MEMETICO_HELPER_HASH_H_

#include <string>
#include <vector>

/**
 * @brief Return a hash value for a given vector of bools 
 * @param v         vector of bools
 * @return          unique hash for combination of bools
 */
inline size_t hash_bools(const vector<bool>& v) {
    size_t value = 0;
    for (const bool& b : v) {
        value <<= 1;      // Shift the current value one position to the left.
        if (b) {
            value |= 1;  // Set the least-significant bit.
        }
    }
    return value;
}

#endif