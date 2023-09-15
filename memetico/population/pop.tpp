
/** @file
 * @brief See pop.h
*/

template <class U>
Population<U>::Population(DataSet* train_data, size_t depth, size_t degree) {

    Population<U>::DEPTH = depth;
    Agent<U>::MAX_DEPTH = depth;
    Agent<U>::DEGREE = degree;

    // Set datasources
    data = train_data;

    // Create root agent that continues to create all children for meme::POP_DEPTH
    root_agent = new Agent<U>();

    // Recursively evaluate initialised functions
    evaluate();

    // Recursively exchange where appropriate
    exchange();

    // Bubble better children up    
    for(size_t i = 0; i < Population<U>::DEPTH; i++)
        bubble();
    
}

template <class U>
void Population<U>::run() {

    cout << "==================" << endl;
    cout << "Starting Memetico" << endl;
    cout << "==================" << endl;

    // Start timer
    auto start_time = chrono::system_clock::now();

    // Initialise population variables
    stale_count = 0, stale_times = 0;      
    set_best_soln(root_agent->get_pocket());     

    // Loop for generations
    for( meme::GEN = 0; meme::GEN < meme::GENERATIONS; meme::GEN++ ) {

        // Start timer
        auto generation_start = chrono::system_clock::now();

        evolve();

        // Local search after the indicated interval
        if( meme::GEN % meme::LOCAL_SEARCH_INTERVAL == 0 )
            local_search();
        
        // Run stale checks
        stale();

        auto generation_end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> generation_ms = generation_end-generation_start;
        cout << setw(5) << GEN << " " << setw(15) << best_soln.get_fitness() <<  " (" << setw(15) << best_soln.get_error() << ") " << setw(10) << " duration: " << setw(10) << generation_ms.count() << "ms root_depth: " << meme::POCKET_DEPTH << " frac: " << best_soln << endl;       

        if( best_soln.get_fitness() < 1e-10 ) {
            cout << "Solution found" << endl;
            break;
        }

        auto end_time = chrono::system_clock::now();
        if( MAX_TIME*1000 < chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count() ) {
            cout << "Maximum time (" << MAX_TIME << " s) reached on gen " << GEN << ". Exiting after " << chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count() << " ms" << endl;
            break;
        }

    }

    // End timer
    auto end_time = chrono::system_clock::now();
    meme::RUN_TIME = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
    
}

template <class U>
void Population<U>::evolve(Agent<U>* agent) {
    
    // Default to root if no argument
    if( agent == nullptr )        agent = root_agent;

    // Repeat the process for the children
    if( !agent->is_leaf() ) {
        for(size_t i = 0; i < Agent<U>::DEGREE; i++)
            evolve(agent->get_children()[i]);
    }

    vector<size_t> all;

    // Mutate based on chance
    if( RandReal::RANDREAL->rand() < MUTATE_RATE ) {

        agent->get_current().mutate(agent->get_pocket());

        // Perform search to refine mutated solution
        local_search_single(agent, true, all);       

        // Exchange and bubble if mutation results in better solution
        if(agent->get_current().get_fitness() < agent->get_pocket().get_fitness() ) {
            agent->exchange();
            for(size_t i = 0; i < Population<U>::DEPTH; i++)
                bubble();
        }
    }
        
    // We can't recombine on the leaf depth as no children exist
    if( agent->is_leaf() )
        return;
    
    //cout << "------" << endl;
    //root_agent->show(cout);

    // Update parent current by recombing parent pocket and the last child current
    agent->get_current().recombine(
        &agent->get_pocket(), 
        &agent->get_children()[Agent<U>::DEGREE-1]->get_current()
    );
    local_search_single(agent, true, all);

    // Recombination for parent and last child recombinations
    agent->get_children()[Agent<U>::DEGREE-1]->get_current().recombine(
        &agent->get_children()[Agent<U>::DEGREE-1]->get_pocket(), 
        &agent->get_current()
    );
    local_search_single(agent->get_children()[Agent<U>::DEGREE-1], true, all);

    // Recombination for children
    for(int i = Agent<U>::DEGREE-2; i >= 0; i--) {

        // Perform the similar recombination process
        agent->get_children()[i]->get_current().recombine(
            &agent->get_children()[i]->get_pocket(), 
            &agent->get_children()[i+1]->get_current()
        );
        local_search_single(agent->get_children()[i], true, all);
    }

    // As we exit on the root agent, re-align tree
    if(agent == root_agent) {
        exchange();
        for(size_t i = 0; i < Population<U>::DEPTH; i++)
            bubble();
    }
}

template <class U>
void Population<U>::local_search(Agent<U>* agent) {
    
    // Default to root if no argument
    if( agent == nullptr )
        agent = root_agent;

    // Perform local search on children
    if( !agent->is_leaf() ) {
        for(size_t i = 0; i < Agent<U>::DEGREE; i++ )
            local_search(agent->get_children()[i]);
    }

    local_search_agent(agent);
  
}

template <class U>
void Population<U>::local_search_agent(Agent<U>* agent) {

    // Create copies of the pocket and current solutions
    U temp_current = U( agent->get_current() );
    U temp_pocket = U( agent->get_pocket() );

    // Run LS for the number of configured times on the current solution
    for(size_t j = 0; j < LOCAL_SEARCH_RUNS; j++ ) {

        // Generate a new subset of data to run LS on
        vector<size_t> selected_idx;
        if( LOCAL_SEARCH_DATA_PCT < 1 )
            selected_idx = data->subset(LOCAL_SEARCH_DATA_PCT);

        temp_current.local_search(data,selected_idx);
        //U::LOCAL_SEARCH(&temp_current, data, selected_idx);

    }

    if( temp_current.get_fitness() < agent->get_current().get_fitness() )
        agent->set_current(temp_current);
    
    // Allow pocket same opportunity to retain its position 
    if( agent->get_current().get_fitness() < agent->get_pocket().get_fitness() ) {

        for(size_t j = 0; j < meme::LOCAL_SEARCH_RUNS; j++ ) {

            // Generate a new subset of data to run LS on
            vector<size_t> selected_idx;
            if( LOCAL_SEARCH_DATA_PCT < 1 )
                selected_idx = data->subset(LOCAL_SEARCH_DATA_PCT);

            // Search and update copy if fitter
            U::LOCAL_SEARCH(&temp_pocket, data, selected_idx);

        }         

        if( temp_pocket.get_fitness() < agent->get_pocket().get_fitness() )
            agent->set_pocket(temp_pocket);
   
    }

    // If current is better, exchange and bubble
    if( agent->get_current().get_fitness() < agent->get_pocket().get_fitness() ) {
        exchange();
        for(size_t i = 0; i < Population<U>::DEPTH; i++)
            bubble();
    }
}

template <class U>
void Population<U>::local_search_single(Agent<U> * agent, bool is_current, vector<size_t>& idx) {

    // Copy the solution that will be modified by LS
    // @bug When we try to use a pointer here I think LS is ineffective because the we have to delete
    // the reference by the end of the function. Even when attempting to create a new U object from the 
    // pointer and using this in set_pocket/current. Much greater performance how the code is now.
    // There should not be an impact other than slight ticks lost on U copy causing construction of the object 
    // including computation for dynamic depth, which is overridden by copy = 
    U copy;     
    if( is_current )    copy = U( agent->get_current() );
    else                copy = U( agent->get_pocket() );

    // Run LS
    copy.local_search(data, idx);

     // Set best soln if 
    if( idx.empty() && best_soln.get_fitness() < best_soln.get_fitness() )
        set_best_soln(copy);

    // Set current if fitness is better
    if( is_current && copy.get_fitness() < agent->get_current().get_fitness() ) {
        //cout << "improved from local search current " << agent->get_current().get_fitness() << endl;
        agent->set_current(copy);
    }
    
    // Set pocket if fitness is better
    if( !is_current && copy.get_fitness() < agent->get_pocket().get_fitness() ) {
        //cout << "improved from local search pocket " << agent->get_pocket().get_fitness() << endl;
        agent->set_pocket(copy);
    }

}

template <class U>
void Population<U>::bubble(Agent<U>* agent, size_t child_number) {
    
    // Move to the deepest and left-most parent
    if( agent == nullptr ) {

        // Move down left leg until the child is found
        agent = root_agent;
        while(!agent->is_leaf())
            agent = agent->get_children()[0];

        // Go back to the parent. From here we can buble
        agent = agent->get_parent();

    }

    // If a leaf node, exit
    if(agent->is_leaf())
        return;

    agent->bubble();

    if( agent->get_parent() == nullptr )
        return;

    // If we have processed all siblings for this level
    if( child_number+1 == Agent<U>::DEGREE )
        bubble(agent->get_parent(), 0);
    else
        bubble(agent->get_parent()->get_children()[child_number+1], child_number+1);

}

template <class U>
void Population<U>::evaluate(Agent<U>* agent) {
    
    // Start from the root agent
    if( agent == nullptr )
        agent = root_agent;
    
    // Evaluate the current agent
    vector<size_t> all;

    agent->get_pocket().objective(data, all);
    agent->get_current().objective(data,all);

    if(agent->is_leaf())
        return;

    // Evaluate all children
    for( size_t i = 0; i < Agent<U>::DEGREE; i++ )
        evaluate(agent->get_children()[i]);
    
}

template <class U>
void Population<U>::exchange(Agent<U>* agent) {
    
    // Start from the root agent
    if( agent == nullptr )
        agent = root_agent;
    
    // !!! So we shouldnt have to call objective here, however sometime the fitness is wrong
    // when we go to exchange, thus we have to re-run again. This forces it to work, but
    // there is somepoint where we change the fraction but do not evaluate the objective again
    agent->get_current().objective(data);
    agent->get_pocket().objective(data);

    // Only when current fitter than pocket, exchange
    if( agent->get_current().get_fitness() < agent->get_pocket().get_fitness() )
        agent->exchange();

    if(agent->is_leaf())
        return;

    // Evaluate all children
    for( size_t i = 0; i < Agent<U>::DEGREE; i++ )
        exchange(agent->get_children()[i]);
    
}

template <class U>
void Population<U>::stale() {

    // Increment stale when the solution has not improved
    if( root_agent->get_pocket().get_fitness() >= best_soln.get_fitness() )
        stale_count++;
    else {
        stale_count = 0;
        stale_times = 0;
        set_best_soln(root_agent->get_pocket());
        POCKET_DEPTH = best_soln.get_depth();
    }

    if (Population<U>::DIVERSITY_TYPE == DiversityEvery) {
        distinct(DIVERSITY_COUNT);
        for(size_t i = 0; i < Population<U>::DEPTH; i++)
            bubble();
    }

    // Reset root pocket on stale
    if( stale_count > STALE_RESET ) {

        // Standard diversity action
        if(DIVERSITY_TYPE == DiversityNone) {

            // Reset the roots current
            cout << "\tResetting stale root " << endl;
            U replace = U();
            vector<size_t> all;
            replace.objective(data,all);
            root_agent->set_current(replace);
            
        } else if (DIVERSITY_TYPE == DiversityStale)
            distinct(DIVERSITY_COUNT);

        else if (DIVERSITY_TYPE == DiversityStaleExtended) {
            
            // When stale twice, time to boom!
            if(stale_times == 2) {
                distinct(13);
                stale_times = 0;
            }
            else                    
                distinct(DIVERSITY_COUNT);
        }
    
        for(size_t i = 0; i < Population<U>::DEPTH; i++)
            bubble();

        stale_count = 0;
        stale_times++;
    } 
}

template <class U>
vector<U> Population<U>::to_soln_list() {
    vector<U> ret;

    // Hardcode for 13 agents pockets
    ret.push_back(root_agent->get_pocket());
    ret.push_back(root_agent->get_children()[0]->get_pocket());
    ret.push_back(root_agent->get_children()[1]->get_pocket());
    ret.push_back(root_agent->get_children()[2]->get_pocket());
    ret.push_back(root_agent->get_children()[0]->get_children()[0]->get_pocket());
    ret.push_back(root_agent->get_children()[0]->get_children()[1]->get_pocket());
    ret.push_back(root_agent->get_children()[0]->get_children()[2]->get_pocket());
    ret.push_back(root_agent->get_children()[1]->get_children()[0]->get_pocket());
    ret.push_back(root_agent->get_children()[1]->get_children()[1]->get_pocket());
    ret.push_back(root_agent->get_children()[1]->get_children()[2]->get_pocket());
    ret.push_back(root_agent->get_children()[2]->get_children()[0]->get_pocket());
    ret.push_back(root_agent->get_children()[2]->get_children()[1]->get_pocket());
    ret.push_back(root_agent->get_children()[2]->get_children()[2]->get_pocket());

    // Hardcode for 13 agents currents
    ret.push_back(root_agent->get_current());
    ret.push_back(root_agent->get_children()[0]->get_current());
    ret.push_back(root_agent->get_children()[1]->get_current());
    ret.push_back(root_agent->get_children()[2]->get_current());
    ret.push_back(root_agent->get_children()[0]->get_children()[0]->get_current());
    ret.push_back(root_agent->get_children()[0]->get_children()[1]->get_current());
    ret.push_back(root_agent->get_children()[0]->get_children()[2]->get_current());
    ret.push_back(root_agent->get_children()[1]->get_children()[0]->get_current());
    ret.push_back(root_agent->get_children()[1]->get_children()[1]->get_current());
    ret.push_back(root_agent->get_children()[1]->get_children()[2]->get_current());
    ret.push_back(root_agent->get_children()[2]->get_children()[0]->get_current());
    ret.push_back(root_agent->get_children()[2]->get_children()[1]->get_current());
    ret.push_back(root_agent->get_children()[2]->get_children()[2]->get_current());

    return ret;
}

template <class U>
void Population<U>::distinct(size_t count) {

    // List of all solutions to compare (pockets and currents)
    vector<U> agents = to_soln_list();

    // Vector of results
    vector<Similar> vec;

    // Determine the distance for every pair of solutions 
    for(size_t i = 0; i < agents.size(); i++) {
        for(size_t j = i+1; j < agents.size(); j++) {
            double dist = objective::compare(&agents.at(i), &agents.at(j), data);
            vec.push_back(
                Similar(i, agents.at(i).get_depth(), agents.at(i).get_count_active(),
                        j, agents.at(j).get_depth(), agents.at(j).get_count_active(),
                        dist)
            );

        }
    }

    // Sort list by ascending similarity
    sort(vec.begin(), vec.end());

    // For all solutions
    for(size_t i = 0; i < vec.size(); i++) {
        
        // For the first count most similar solutions
        if( i <= count) {

            size_t pos;

            // If equal depth
            if( vec.at(i).id == vec.at(i).jd ) {

                // If equal number of parameters, select uniform at random
                if( vec.at(i).iv == vec.at(i).jv ) {
                    if( RandReal::RANDREAL->rand() < 0.5 )  pos = vec.at(i).j;
                    else                                    pos = vec.at(i).i;
                }
                // If less in i, use j
                else if (vec.at(i).iv < vec.at(i).jv) {
                    pos = vec.at(i).j;
                }
                // If less in j, use i
                else {
                    pos = vec.at(i).i;    
                }

            // If not of equal depth, replace the larger depth in i or j
            } else if ( vec.at(i).id < vec.at(i).jd ) {
                pos = vec.at(i).j;
            } else {
                pos = vec.at(i).i;
            }
            U rand_sol = U();

            // Perform full local search on the new result
            vector<size_t> all;
            rand_sol.local_search(data, all);
            //U::LOCAL_SEARCH(&rand_sol, data, all);

            // Replace the underlying soln
            if( pos == 0 )    root_agent->set_pocket(rand_sol);
            if( pos == 1 )    root_agent->get_children()[0]->set_pocket(rand_sol);
            if( pos == 2 )    root_agent->get_children()[1]->set_pocket(rand_sol);
            if( pos == 3 )    root_agent->get_children()[2]->set_pocket(rand_sol);
            if( pos == 4 )    root_agent->get_children()[0]->get_children()[0]->set_pocket(rand_sol);
            if( pos == 5 )    root_agent->get_children()[0]->get_children()[1]->set_pocket(rand_sol);
            if( pos == 6 )    root_agent->get_children()[0]->get_children()[2]->set_pocket(rand_sol);
            if( pos == 7 )    root_agent->get_children()[1]->get_children()[0]->set_pocket(rand_sol);
            if( pos == 8 )    root_agent->get_children()[1]->get_children()[1]->set_pocket(rand_sol);
            if( pos == 9 )    root_agent->get_children()[1]->get_children()[2]->set_pocket(rand_sol);
            if( pos == 10 )   root_agent->get_children()[2]->get_children()[0]->set_pocket(rand_sol);
            if( pos == 11 )   root_agent->get_children()[2]->get_children()[1]->set_pocket(rand_sol);
            if( pos == 12 )   root_agent->get_children()[2]->get_children()[2]->set_pocket(rand_sol);
            
            if( pos == 13 )    root_agent->set_current(rand_sol);
            if( pos == 14 )    root_agent->get_children()[0]->set_current(rand_sol);
            if( pos == 15 )    root_agent->get_children()[1]->set_current(rand_sol);
            if( pos == 16 )    root_agent->get_children()[2]->set_current(rand_sol);
            if( pos == 17 )    root_agent->get_children()[0]->get_children()[0]->set_current(rand_sol);
            if( pos == 18 )    root_agent->get_children()[0]->get_children()[1]->set_current(rand_sol);
            if( pos == 19 )    root_agent->get_children()[0]->get_children()[2]->set_current(rand_sol);
            if( pos == 20 )    root_agent->get_children()[1]->get_children()[0]->set_current(rand_sol);
            if( pos == 21 )    root_agent->get_children()[1]->get_children()[1]->set_current(rand_sol);
            if( pos == 22 )    root_agent->get_children()[1]->get_children()[2]->set_current(rand_sol);
            if( pos == 23 )    root_agent->get_children()[2]->get_children()[0]->set_current(rand_sol);
            if( pos == 24 )    root_agent->get_children()[2]->get_children()[1]->set_current(rand_sol);
            if( pos == 25 )    root_agent->get_children()[2]->get_children()[2]->set_current(rand_sol);

        }
    }    
}