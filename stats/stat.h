
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Header for Stat class
*/

#ifndef STATS_STAT_H
#define STATS_STAT_H

// Local
#include <chrono>

// Std lib
#include <stdexcept>
#include <stdio.h>

/**
 * Class managing general statistics
 * 
 */
class Stat {

    private:

        double*         values;

        size_t          count;

        vecor<string>   names;

        vecor<string>   display;

    public:

        // See Implementation

        Stat();

        ~Stat();

        void show(ostream& out = cout, size_t precision = 5);

};

#endif