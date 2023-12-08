
#ifndef MEMETICO_MODELS_MUTATION_H
#define MEMETICO_MODELS_MUTATION_H

#include <vector>
#include <unordered_map>

#include <memetico/models/cont_frac_dd.h>   
#include <memetico/model_base/model_meme.h>  
#include <memetico/models/regression.h> 


namespace mutation {

    // Base MutationPolicy class
    template <typename U>
    struct MutationPolicy { };

    // Mutation Policies
    template <typename U, typename Derived>
    struct MutateHardSoft : public MutationPolicy<U> {

        MutateHardSoft (size_t frac_depth = 4) {
                   
            size_t params = MemeticModel<U>::IVS.size()+1;

            // add global flags for each parameter
            for(size_t i = 0; i < params; i++) {

                if(i == params-1)
                    global_active.push_back(true);
                
                else

                    // Randomly turn the IV on/off, always set the constant on
                    if( RandReal::RANDREAL->rand() < (2.0/3.0))
                        global_active.push_back(true);

                    else
                        global_active.push_back(false);
        
            }
        }

        void    initialise() {

            auto& model = static_cast<Derived&>(*this);

            for(size_t i = 0; i < model.get_params_per_term(); i++)
                model.set_global_active(i, model.get_global_active(i));

        }

        /* Policy mutation process */
        void    mutate(MemeticModel<U>& other) {
            
            auto& model = static_cast<Derived&>(*this);

            size_t param_pos;

            // Hard mutate when the fitness difference to the pocket is extreme (small or large)
            if( model.get_fitness() < 1.2 * other.get_fitness() || model.get_fitness() > 2* other.get_fitness() ) {

                // Select uniformly at random an independent variable (not a constant)
                param_pos = RandInt::RANDINT->rand(0, MemeticModel<U>::IVS.size()-1);
                
                // Toggle it globally
                model.set_global_active( param_pos, !model.get_global_active(param_pos) );

                // If the variable is globally active
                if(model.get_global_active(param_pos)) {

                    // With 50% chance randomise down the fraction with a non-zero value
                    for(size_t i = 0; i < model.get_frac_terms(); i++ )
                        model.get_terms(i).randomise();

                }

            } else {

                // Identify parameters that we can switch on i.e. not globally inactive
                vector<size_t> potential_params = model.get_active_positions();
                
                // Ensure there are params to change
                if(potential_params.size() == 0) {
                    model.sanitise();
                    return;
                }

                // Select a random parameter
                size_t param_pos = RandInt::RANDINT->rand(0, potential_params.size()-1);
            
                // Toggle value 
                model.set_active(potential_params[param_pos], !model.get_active(potential_params[param_pos]));
                
            }

            model.set_fitness(numeric_limits<double>::max());
            model.set_error(numeric_limits<double>::max());
            model.sanitise();
            
        }

        /** @brief Set global active flag for a specific data variable iv to val  */
        void    set_global_active(size_t iv, bool val) {
            
            auto& model = static_cast<Derived&>(*this);

            for(size_t i = 0; i < model.get_frac_terms(); i++) {
                size_t offset = i*model.get_params_per_term();
                model.set_active(offset+iv, val);            
            }

            global_active[iv] = val;
        };

        /** @brief Return number of pararmters globally turned on */
        bool    get_global_active(size_t iv) const  { return global_active[iv]; };

        vector<bool>   global_active;

    };

    template <typename U, typename Derived>
    struct MutateUniqueMask {

        MutateUniqueMask (size_t frac_depth = 4)  {

            hashes_by_size = unordered_map<size_t, unordered_set<size_t>>();        
            size = 0;    
        };

        // No copy constructor extension is neccessary as our data member is static
        void    initialise() {

            auto& model = static_cast<Derived&>(*this);
            size = model.get_params_per_term()*model.get_frac_terms();
            mutate(model);

        };

        /** @brief mutate the variable masks to an unseen set of masks */
        void mutate(MemeticModel<U>& other) {

            auto& model = static_cast<Derived&>(*this);

            // Re-determine size in case the fraction has been modified since initialise()
            size = model.get_params_per_term()*model.get_frac_terms();

            vector<bool> set = unique_vector();

            for(size_t i = 0; i < set.size(); i++)
                model.set_active(i, set[i]);
            
            return;
        };

        /** @brief return an unseen set of masks or repeat a mask after going through all possible combinations */
        vector<bool> unique_vector() {
            
            auto& model = static_cast<Derived&>(*this);
            
            // Determine the maximum number of combinations, and if we have seen them all, clear and re-process all possibilities again
            size_t max = pow(2, size);
            if (hashes_by_size[size].size() == max) {
                cout << "All possible vectors of size " << size << " have been generated, resetting hashes" << endl;
                hashes_by_size[size].clear();
            }

            // Try 50 times to find a unique vector for the fractions depth
            // We implicitly identify depth as the total number of parameters in the fraction
            // i.e. size
            vector<bool> set;
            for(size_t j = 0; j < 50; j++) {
                // Replace with your unique_bools function. Here's a placeholder.
                set = RandInt::RANDINT->unique_bools(size);

                if (hashes_by_size[size].find(hash_bools(set)) == hashes_by_size[size].end()) {
                    hashes_by_size[size].insert(hash_bools(set));
                    //cout << "Found on iteration: " << j << " hash: " << hash_bools(set) << " count:" << hashes_by_size[size].size() << endl;
                    return set;
                }
            }

            cout << "Failed to find unique vector of size " << size << endl;
            return set;
        };

        static unordered_map<size_t, unordered_set<size_t>> hashes_by_size;
        size_t size;
    
    };

    template <typename U, typename Derived>
    unordered_map<size_t, unordered_set<size_t>> MutateUniqueMask<U,Derived>::hashes_by_size;

};

#endif