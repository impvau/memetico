
/** @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief MemeticModel represents a solution with mutation and recombination operators
 * @copyright (C) 2022 Prof. Pablo Moscato, License CC-BY
 */

#ifndef MEMETICO_MODELS_MEMETICMODEL_H_
#define MEMETICO_MODELS_MEMETICMODEL_H_

using namespace std;

#include <memetico/model_base/model.h>

/**
 * @brief The MemeticModel class representing a solution that implements mutation and recombination 
 */
template<class T>
class MemeticModel : public Model {

    public:
        
        /** 
         * @brief setter for active flag at \a pos with value \a val
         * @param pos position of paramter
         * @param val value of active flag
         */
        virtual void    set_active(size_t pos, bool val)    {};
        
        /** 
         * @brief setter for value at \a pos with value \a val
         * @param pos position of parameter
         * @param val value of active flag
         */
        virtual void    set_param(size_t pos, T val)        {};

        /**  
         * @brief getter for active flag at \a pos
         * @param pos position of parameter
         * @returns active flag at \a pos 
         */
        virtual bool    get_active(size_t pos)              {return false;};
        
        /**  
         * @brief get positions of all parameters that have the active flag set
         * @returns vector of active positions
         */
        virtual vector<size_t>  get_active_positions(size_t pos)      {return vector<size_t>();};
        
        /**  
         * @brief getter for parameter value at \a pos
         * @param pos position of parameter
         * @returns value at pos
         * @bug here we define the return as double. What if the parameter is an integer model
         * or we are using something like a coordinate representation for a parameter? We may 
         * need to extend here to a second template U but holding off for now.
         */
        virtual double  get_param(size_t pos)               {return 0;};

        /**  
         * @brief mutate \a this MemeticModel and optionally consider another model m 
         * @param m another model to consider in the mutation
         * @bug we may want to overload the mutate function and remove the optional argument
         */
        virtual void    mutate(MemeticModel<T>* m = nullptr) {};
        
        /**  
         * @brief recombine \a this MemeticModel considering two other MemeticModels
         * @param m1 first MemeticModel to consider
         * @param m2 second MemeticModel to consider
         * @param method_override force the use of a specific recombination method
         */
        virtual void    recombine(MemeticModel<T>* m1, MemeticModel<T>* m2, int method_override = -1) {};
        
        /**  
         * @brief determine the number of parameters used in the model
         * @return number of parameters with an active flag
         */
        virtual size_t  get_count_active() { return 0; };
        
};

#endif