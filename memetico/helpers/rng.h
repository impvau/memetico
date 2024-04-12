
/**
 * @file
 * @author andy@impv.au
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

/** Class to manage randomisation of integers  */
class RandInt {

    private:
        mt19937 gen;

    public:

        /** @brief Construct RandInt with a random device seed */
        RandInt() {
            random_device rd;
            gen.seed(rd());
        }

        /** @brief Construct RandInt with specified seed */
        RandInt(int seed) {
            gen.seed(seed);
        }

        /** @brief Return random integer between min and max */
        int operator()(int min, int max) {
            return rand(min,max);
        }

        /** @brief Return random integer between min and max */
        int rand(int min, int max) {
            if(min > max) {
                throw runtime_error( "Invalid Range min:"+to_string(min)+" max:"+to_string(max) );
            }
            return uniform_int_distribution<> (min, max) (gen);
        }

        /** @brief Return unique list of random values between start and end */
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

        /** @brief Return unique list of random values between start and end */
        vector<bool> unique_bools(size_t amount) {

            vector<bool> set;
            for( size_t i = 0; i < amount; i++)
                set.push_back(rand(0,1));

            return set;
        }

        /** @brief Reference to global int randomiser */
        static RandInt*         RANDINT;
};

/** Class to manage randomisation of doubles */
class RandReal {

    private:

        mt19937 gen;
        uniform_real_distribution<> dis;

    public:

        /** @brief Construct RandReal with a random device seed */
        RandReal() {
            random_device rd;
            gen.seed(rd());
        }

        /** @brief Construct RandInt with specified seed */
        RandReal(int seed) {
            gen.seed(seed);
        }

        /** @brief Return random real between 0 and 1 */
        double operator()() {
            return rand(0.0, 1.0);
        }

        /** @brief Return random real between min and max */
        double operator()(double min, double max) {
            return rand(min,max);
        }

        /** @brief Return random real between min and max */
        double rand(double min = 0, double max = 1) {
            
            if(min > max) 
                throw invalid_argument( "Invalid Range a:"+to_string(min)+" b:"+to_string(max) );
                
            return uniform_real_distribution<> (min, max) (gen);
        }

        /** @brief Reference to global real randomiser */
        static RandReal*        RANDREAL;

};

#endif
