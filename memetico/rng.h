
/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * 
 * @brief Wrapper classes for global int and double random number generation
 * 
 */

using namespace std;

#ifndef RNG_H
#define RNG_H

#include <random>
#include <iostream>
#include <stdexcept>
#include <algorithm>

/**
 * Class to manage randomisation of integers
 * 
 */
class RandInt {

    private:
        mt19937 gen;

    public:

        /**
         * Construct RandInt with random seed
         * @return RandInt
         */
        RandInt() {
            random_device rd;
            gen.seed(rd());
        }

        /**
         * Construct RandInt with specified seed
         * @param seed value to initilise RandInt for reproducible random values
         * @return RandInt
         */
        RandInt(int seed) {
            gen.seed(seed);
        }

        /**
         * Return random value inclusively between min and max 
         * 
         * throws runtime_error() when min < max to avoid infinite loop
         * 
         * @param min lowest possible random int
         * @param max highest possible random int
         * @return int 
         */
        int operator()(int min, int max) {
            if(min > max) {
                throw runtime_error( "Invalid Range min:"+to_string(min)+" max:"+to_string(max) );
            }
            return uniform_int_distribution<> (min, max) (gen);
        }

        /**
         * Return unique amount of values between start and end 
         * 
         * @param amount number of samples between start and end to extract
         * @param start smallest number for selection
         * @param end largest number for selection
         * @return int 
         */
        vector<size_t> unique_set(size_t amount, size_t start, size_t end) {

            vector<size_t> population;
            for( size_t i = start; i < end; i++)
                population.push_back(i);

            vector<size_t> selection;
            sample(
                population.begin(), 
                population.end(), 
                back_inserter(selection),
                amount,
                gen);

            return selection;
        }
};

/**
 * Class to manage randomisation of doubles
 * 
 */
class RandReal {

    private:

        mt19937 gen;
        uniform_real_distribution<> dis;

    public:

        
        RandReal() {
            random_device rd;
            gen.seed(rd());
        }

        RandReal(int seed) {
            gen.seed(seed);
        }

        /**
         * Return random value inclusively between 0 and 1
         * 
         * @param min lowest possible random double
         * @param max highest possible random double
         * @return double
         */
        double operator()() {
            return uniform_real_distribution<> (0.0, 1.0) (gen);
        }

        /**
         * Return random value inclusively between min and max 
         * 
         * throws runtime_error() when min < max to avoid infinite loop
         * 
         * @param min lowest possible random double
         * @param max highest possible random double
         * @return double
         */
        double operator()(double min, double max) {
            
            if(min > max) 
                throw invalid_argument( "Invalid Range a:"+to_string(min)+" b:"+to_string(max) );
                
            return uniform_real_distribution<> (min, max) (gen);
        }
};

#endif
