
/** @file
 * @author andy@impv.au
 * @version 1.0
 * @brief A class extending Model to represent a solution in a genetic or memetic algorithm
 */

#ifndef MEMETICO_MODELS_MEMETICMODEL_H_
#define MEMETICO_MODELS_MEMETICMODEL_H_

using namespace std;

#include <memetico/model_base/model.h>
#include <memetico/helpers/rng.h>

/**
 * @brief A class extending Model to represent a solution in a genetic or memetic algorithm
 * 
 * A MemeticModel solution is effectively an equation that approximates the data.
 * 
 * Classes that extend a MemeticModel must implement.
 * - mutate (process to mutate the solution equation in genetic evolution)
 * - recombine (process to combine two solutions in genetic evolution)
 * - local_search (process to explore local minima/maxima in memetic evolution)
 * - objective (process for evaluating error of the solution)
 * 
 * The implementation of the Memetic Model can be tailored for simplicity and independence, 
 * depending on the specific solution required. As an example, the Mean Squared Error (MSE) 
 * can be utilized as the objective function while the Nelder-Mead method is employed for local search.
 * In order to maintain this simplicity and flexibility, static variables have been integrated in 
 * the MemeticModel for specifying the 'underlying' method. These variables are MemeticModel<T>::OBJECTIVE 
 * for the objective function, and MemeticModel<T>::LOCAL_SEARCH for the local search method. They can 
 * be invoked directly from their respective functions: objective() and local_search().
 * 
 * While these static variables offer a good starting point, it's also possible to customize them
 * in a class that extends MemeticModel<T>. This feature comes in handy when there's a need to 
 * adjust the operation of the model to meet specific requirements. For example, when working with 
 * ContinuedFraction, you might want to apply the local search method on a depth-by-depth basis. 
 * In such scenarios, you can override the local_search() function to implement this. Additionally,
 * the get_active_positions() function, which is required by the underlying local search method,
 * can also be modified to step through the problem depth-by-depth. This kind of customization
 * gives you a significant level of control over how the Memetic Model is applied.
 * 
 * Extended classes implement certain functions necessary for correctly accessing elements in the
 * solution equation. Essentially, these functions convert the position of variables in the solution
 * into a one-dimensional list. For instance, the function get_value(pos) works differently in a 
 * BranchedContinuedFraction compared to a standard ContinuedFraction. This is because the solution's
 * structure varies between these two, so the same position 'pos' would map to different values.
 * 
 * - set_active (turn off/on variables in the solution)
 * - get_active (determine if variable is off/on)
 * - set_value (set value of the variable in the equation)
 * - get_value (determine if varaiable is on/off)
 * - get_count_active (get the number of active variables in equation)
 * - get_active_positions (get the variable positions used to optimise by local search)
 * 
 * To avoid the need for static casting from a MemeticModel<T> to the derived class in functions like
 * OBJECTIVE and LOCALSEARCH, we provide print() and str() functions.
 * 
 */
template<typename T>
class MemeticModel : public Model {

    public:
        
        MemeticModel() : Model()                            {};
        MemeticModel(const MemeticModel<T>& m) : Model(m)   {};

        /** 
         * @brief setter for active flag at \a pos with value \a val
         * @param pos position of parameter
         * @param val value of active flag
         */
        virtual void    set_active(size_t pos, bool val)    {};
        
        /** 
         * @brief setter for value at \a pos with value \a val
         * @param pos position of parameter
         * @param val value of active flag
         */
        virtual void    set_value(size_t pos, T val)        {};

        /**  
         * @brief getter for active flag at \a pos
         * @param pos position of parameter
         * @returns active flag at \a pos 
         */
        virtual bool    get_active(size_t pos)              {return false;};
        
        /**  
         * @brief getter for parameter value at \a pos
         * @param pos position of parameter
         * @returns value at pos
         * @bug here we define the return as double. What if the parameter is an integer model
         * or we are using something like a coordinate representation for a parameter? We may 
         * need to extend here to a second template U but holding off for now.
         */
        virtual T       get_value(size_t pos)               {return 0;};
        
        /**  
         * @brief determine the number of parameters used in the model
         * @return number of parameters with an active flag
         */
        virtual size_t  get_count_active() { return 0; };

        /**  
         * @brief get positions of all parameters that have the active flag set
         * @returns vector of active positions
         */
        virtual vector<size_t>      get_active_positions()      {return vector<size_t>();};

        /**  
         * @brief mutate \a this MemeticModel and optionally consider another model m 
         * @param m another model to consider in the mutation
         * @bug we may want to overload the mutate function and remove the optional argument
         */
        virtual void    mutate(MemeticModel<T> & m) {};
        
        /**  
         * @brief recombine \a this MemeticModel considering two other MemeticModels
         * @param m1 first MemeticModel to consider
         * @param m2 second MemeticModel to consider
         * @param method_override force the use of a specific recombination method
         */
        virtual void    recombine(MemeticModel<T> * m1, MemeticModel<T> * m2, int method_override = -1) {};
        
        /** @brief Perform local search function on MemeticModel */
        virtual double  local_search(DataSet* data, vector<size_t> idx = vector<size_t>()) {
            return LOCAL_SEARCH(this, data, idx);
        };

        /** @brief Perform objective function on MemeticModel */
        virtual double  objective(DataSet* data, vector<size_t> idx = vector<size_t>()) {
            return OBJECTIVE(this, data, idx);
        };

        /** @brief Print the solution to stdout */
        virtual void    print()                             { cout << "model_meme" << endl;};

        /** @brief Get the string representation of the solution */
        virtual string  str()                               { return "model_meme"; };

        /** @brief Comparison operator for MemeticModel */
        bool operator== (Model& o)      { return  Model::operator==(o); }

        /** @brief Vector of independent variable names from the data source */
        static vector<string>   IVS;

        /** @brief local search function to explore local minima and maxima */
        static double   (*LOCAL_SEARCH)(MemeticModel<T>*, DataSet*, vector<size_t>&);

        /** @brief name of the objective function */
        static string   OBJECTIVE_NAME;

        /** @brief objective function to evaluate the solutions */
        static double   (*OBJECTIVE)(MemeticModel<T>*, DataSet*, vector<size_t>&);      
};

template<typename T>
vector<string>  MemeticModel<T>::IVS = {};

template<typename T>
double (*MemeticModel<T>::LOCAL_SEARCH)(MemeticModel<T>*, DataSet*, vector<size_t>&) = nullptr;

template<typename T>
string MemeticModel<T>::OBJECTIVE_NAME = "";

template<typename T>
double (*MemeticModel<T>::OBJECTIVE)(MemeticModel<T>*, DataSet*, vector<size_t>&) = nullptr;

#endif