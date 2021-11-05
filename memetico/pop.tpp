
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Implementation of the Population class
*/

/**
 * Construct Population with a tree of memetico::POP_DEPTH and degree memetico::POP_DEGREE
 * 
 * @return  Population
 */
template <class U>
Population<U>::Population(DataSet* train_data) {

    // Debug
    if( memetico::DEBUG )
        cout << "Population()" << endl;

    // Set datasources
    data = train_data;
    Agent<U>::TRAIN = data;

    // Create root agent that continues to create all children for memetico::POP_DEPTH
    root_agent = new Agent<U>;

    // Recursively evaluate initialised functions
    evaluate();

    // Bubble better children up
    bubble();

    root_agent->show(cout, memetico::PREC,true);

}

/**
 * Construct Population
 *
 * @return  Population
 */
template <class U>
Population<U>::~Population() {
    delete root_agent;
}

/**
 * Run evoliations for memetico::GENERATIONS
 *
 * @return  void
 * @bug Current we re-initialise the pocket of the root, but this is extremely agreesive, different approach here?
 */
template <class U>
void Population<U>::run() {

    cout << "Starting Memetico..." << endl;

    // Start timer
    auto start_time = chrono::system_clock::now();

    // Setup helper vars
    size_t stale = 0;           // Number of stale generations
    double best_fitness = root_agent->members[Agent<U>::POCKET]->fitness;         // Best fitness known

    root_agent->show();

    // Loop for generations
    for( memetico::GEN = 0; memetico::GEN < memetico::GENERATIONS; memetico::GEN++ ) {

        // Start timer
        auto generation_start = chrono::system_clock::now();

        evolve();

        // Local search after the indicated interval
        if( memetico::GEN % memetico::LOCAL_SEARCH_INTERVAL == 0 )
            local_search();
        
        // Increment stale when the solution has not improved
        cout << "Population LastFitness " << best_fitness << " BestFitness: " << root_agent->members[Agent<U>::POCKET]->fitness << " Frac: "; root_agent->members[Agent<U>::POCKET]->show_min(cout, memetico::PREC, 0, memetico::PrintExcel, false); cout << endl;
        if( root_agent->members[Agent<U>::POCKET]->fitness >= best_fitness )
            stale++;
        else {    
            stale = 0;
            best_fitness = root_agent->members[Agent<U>::POCKET]->fitness;
        }
            
        // Reset root pocket on stale
        if( stale > memetico::STALE_RESET ) {
            cout << "\tResetting stale root " << endl;
            root_agent->renew();
            root_agent->evaluate(data);
            stale = 0;
            // Note only a single bubble, so if it is a terrible solution, it moves down to depth 1 straight away
            // but still mixes with all the other agents of the population
            bubble();
        }

        auto generation_end = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> generation_ms = generation_end-generation_start;
        cout << setw(5) << memetico::GEN << setw(10) << best_fitness << setw(10) << " duration: " << generation_ms.count() << "ms"  << endl;
        
        //root_agent->show_errors(cout, memetico::PREC, data);

        root_agent->log << "Root Agent at generation " << memetico::GEN << endl;
        root_agent->members[Agent<U>::POCKET]->show(root_agent->log, memetico::PREC, true);

        generational_summary();

    }

    // End timer
    auto end_time = chrono::system_clock::now();
    memetico::RUN_TIME = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();

    root_agent->members[Agent<U>::POCKET]->show_min(cout,memetico::PREC, 0, memetico::PrintExcel);
    root_agent->members[Agent<U>::POCKET]->show_min(cout,memetico::PREC, 0, memetico::PrintLatex);
    root_agent->members[Agent<U>::POCKET]->show_min(cout,memetico::PREC, 0, memetico::PrintNumpy);

    cout << endl << endl;
    cout << "Finished after " << memetico::RUN_TIME << " milliseconds" << endl;
    
}

/**
 * Evolve agent recusively by
 * - Undertakie mutation
 * - Undertake recombination for non-leaf nodes (using the agent just mutated) 
 * - Repeat for each agent in the population
 * 
 * @param agent agent to undertake the evolution process on
 *                  Defaults to root_agent
 * @return  void
 */
template <class U>
void Population<U>::evolve(Agent<U>* agent) {
    
    // Default to root if no argument
    if( agent == nullptr )        agent = root_agent;

    agent->evaluate(data);

    agent->log_small << "Evolving generation " << memetico::GEN << endl;
    agent->log_small << "\t  Pocket: "; agent->members[Agent<U>::POCKET]->show_min(agent->log_small, memetico::PREC, 0, memetico::PrintExcel, false); agent->log_small << " Fitness: " << agent->members[Agent<U>::POCKET]->fitness << " Error: " << agent->members[Agent<U>::POCKET]->error << endl;
    agent->log_small << "\t Current: "; agent->members[Agent<U>::CURRENT]->show_min(agent->log_small, memetico::PREC, 0, memetico::PrintExcel, false); agent->log_small << " Fitness: " << agent->members[Agent<U>::CURRENT]->fitness << " Error: " <<  agent->members[Agent<U>::CURRENT]->error << endl;
    

    // Mutate based on chance
    if( memetico::RANDREAL() < memetico::MUTATE_RATE ) {

        //cout << "Mutating Current of Agent " << agent->number << endl;
        agent->stat_mutate++;
        
        // Master Log Mutate
        memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentCurrentMutate," << agent->number;
        memetico::master_log << ",\""; agent->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << agent->members[Agent<U>::CURRENT]->fitness << "," << agent->members[Agent<U>::CURRENT]->error;

        agent->members[Agent<U>::CURRENT]->mutate(agent->members[Agent<U>::POCKET], agent->log_small);
        agent->evaluate(data);
        
        memetico::master_log << ",\""; agent->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << agent->members[Agent<U>::CURRENT]->fitness << "," << agent->members[Agent<U>::CURRENT]->error;
        memetico::master_log << endl;

        bubble();
    }
        
    // We can't recombine on the leaf depth as no children exist
    if( agent->is_leaf() )
        return;
    
    // Update parent current by recombing parent pocket and the last child current
    //cout << " leader " << agent->number << " and children[" << Agent<U>::DEGREE-1 << "]" << endl;

    memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentCurrentRecombine," << agent->number;
    memetico::master_log << ",\""; agent->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->members[Agent<U>::CURRENT]->fitness << "," << agent->members[Agent<U>::CURRENT]->error;

    agent->stat_recombine++;
    agent->log_small << "Recombine Agent " << agent->number << " Pocket with Agent " << agent->children[Agent<U>::DEGREE-1]->number << " Current ";
    agent->members[Agent<U>::CURRENT]->recombine(
        agent->members[Agent<U>::POCKET], 
        agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT],
        -1,
        4, 
        agent->log_small
    );
    agent->evaluate(data);
    memetico::master_log << ",\""; agent->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->members[Agent<U>::CURRENT]->fitness << "," << agent->members[Agent<U>::CURRENT]->error;
    memetico::master_log << endl;
    agent->exchange();

    // Update the child current

    memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentCurrentRecombine," << agent->children[Agent<U>::DEGREE-1]->number;
    memetico::master_log << ",\""; agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->fitness << "," << agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->error;

    agent->children[Agent<U>::DEGREE-1]->stat_recombine++;
    agent->log_small << "Recombine Agent " << agent->children[Agent<U>::DEGREE-1]->number << " Pocket with Agent " << agent->number << " Current ";
    agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->recombine(
        agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::POCKET], 
        agent->members[Agent<U>::CURRENT],
        -1,
        4, 
        agent->log_small
    );
    //cout << "Recombining Agent " << agent->children[Agent<U>::DEGREE-1]->number << endl;
    agent->children[Agent<U>::DEGREE-1]->evaluate(data);
    memetico::master_log << ",\""; agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->fitness << "," << agent->children[Agent<U>::DEGREE-1]->members[Agent<U>::CURRENT]->error;
    memetico::master_log << endl;
    agent->children[Agent<U>::DEGREE-1]->exchange();

    // Update the children currents by recombing the childs pocket with the leaders current
    for(size_t i = 0; i < Agent<U>::DEGREE-1; i++) {

        memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentCurrentRecombine," << agent->children[i]->number;
        memetico::master_log << ",\""; agent->children[i]->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->children[i]->members[Agent<U>::CURRENT]->fitness << "," << agent->children[i]->members[Agent<U>::CURRENT]->error;

        // Update parent current by recombing parent pocket and the last child 
        agent->children[i]->stat_recombine++;
        agent->log_small << "Recombine Agent " << agent->children[i]->number << " Pocket with Agent " <<  agent->children[i+1]->number << " Current ";
        agent->children[i]->members[Agent<U>::CURRENT]->recombine(
            agent->children[i]->members[Agent<U>::POCKET], 
            agent->children[i+1]->members[Agent<U>::CURRENT],
            -1,
            4, 
            agent->log_small
        );
        agent->children[i]->evaluate(data);

        memetico::master_log << ",\""; agent->children[i]->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->children[i]->members[Agent<U>::CURRENT]->fitness << "," << agent->children[i]->members[Agent<U>::CURRENT]->error;
        memetico::master_log << endl;

        agent->children[i]->exchange();

    }
    

    // Repeat the process for the children
    for(size_t i = 0; i < Agent<U>::DEGREE; i++)
        evolve(agent->children[i]);

    // Clear Agent LS stats
    agent->stat_local_search_current_nm_iters = 0;
    agent->stat_local_search_current_nm_time = 0;
    agent->stat_local_search_pocket_nm_iters = 0;
    agent->stat_local_search_pocket_nm_time = 0;

}

/**
 * Run local search onthe population from agent through all children
 *
 * @return  void
 */
template <class U>
void Population<U>::local_search(Agent<U>* agent) {
    
    // Default to root if no argument
    if( agent == nullptr )
        agent = root_agent;

    if( !agent->is_leaf() ) {
        for(size_t i = 0; i < Agent<U>::DEGREE; i++ ) {
            local_search(agent->children[i]);
        }   
    }

    U* temp_current = agent->members[Agent<U>::CURRENT]->clone();
    U* temp_pocket = agent->members[Agent<U>::POCKET]->clone();

    // Local Search current always
    for(size_t j = 0; j < memetico::LOCAL_SEARCH_RUNS; j++ ) {

        // Take selection of data
        vector<size_t> selected_idx;
        if( memetico::LOCAL_SEARCH_DATA_PCT < 1 )
            selected_idx = data->subset(memetico::LOCAL_SEARCH_DATA_PCT);
        else 
            selected_idx = vector<size_t>();

        double fitness = Agent<U>::LOCAL_SEARCH(temp_current, data, selected_idx);
        agent->stat_local_search_current++;
        agent->stat_local_search_current_nm_iters += local_search::nm_iters;
        agent->stat_local_search_current_nm_time += local_search::nm_duration;

        // Master Log of construction
        memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentCurrentLocalSearch," << agent->number;
        memetico::master_log << ",\""; agent->members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->members[Agent<U>::CURRENT]->fitness << "," << agent->members[Agent<U>::CURRENT]->error;
        memetico::master_log << ",\""; temp_current->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << temp_current->fitness << "," << temp_current->error;
        memetico::master_log << endl;

        agent->log_small << "LocalSearch Agent " << agent->number << " Current Time: " << local_search::nm_duration << " NMIters: " << local_search::nm_iters << " Fitness: " << agent->members[Agent<U>::CURRENT]->fitness << " NewFitness: " << fitness << " ModelObj: " << temp_current->fitness << endl;
        if(fitness < agent->members[Agent<U>::CURRENT]->fitness) {
            
            agent->log_small << "LocalSearch Updating Agent " << agent->number << " Current "; temp_current->show_min(agent->log_small, memetico::PREC, 0, memetico::PrintExcel, false); agent->log_small << endl;

            agent->members[Agent<U>::CURRENT] = temp_current;
            agent->stat_local_search_current_improve++;
            agent->evaluate(data);
        }
    
    }

    // Local Search Pocket for a chance to avoid replacement
    if( agent->members[Agent<U>::POCKET]->fitness > agent->members[Agent<U>::CURRENT]->fitness ) {

        agent->log_small << "LocalSearch Agent " << agent->number << " Current > Pocket, Giving Pocket a Chance to compete. Current: " << agent->members[Agent<U>::CURRENT]->fitness << " Pocket: " << agent->members[Agent<U>::POCKET]->fitness << endl;
        
        for(size_t j = 0; j < memetico::LOCAL_SEARCH_RUNS; j++ ) {

            // Take selection of data
            vector<size_t> selected_idx;
            if( memetico::LOCAL_SEARCH_DATA_PCT < 1 )
                selected_idx = data->subset(memetico::LOCAL_SEARCH_DATA_PCT);
            else 
                selected_idx = vector<size_t>();

            double fitness = Agent<U>::LOCAL_SEARCH(temp_pocket, data, selected_idx);

            agent->stat_local_search_pocket++;
            agent->stat_local_search_pocket_nm_iters += local_search::nm_iters;
            agent->stat_local_search_pocket_nm_time += local_search::nm_duration;

            agent->log_small << "LocalSearch Agent " << agent->number << " Pocket Time: " << local_search::nm_duration << " NMIters: " << local_search::nm_iters << " Fitness: " << agent->members[Agent<U>::POCKET]->fitness << " NewFitness: " << fitness << " ModelObj: " << temp_pocket->fitness << endl;

            // Master Log of construction
            memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentPocketLocalSearch," << agent->number;
            memetico::master_log << ",\""; agent->members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << agent->members[Agent<U>::POCKET]->fitness << "," << agent->members[Agent<U>::POCKET]->error;
            memetico::master_log << ",\""; temp_pocket->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << temp_pocket->fitness << "," << temp_pocket->error;
            memetico::master_log << endl;

            if(fitness < agent->members[Agent<U>::POCKET]->fitness) {
                
                agent->log_small << "LocalSearch Updating Agent " << agent->number << " Pocket "; temp_pocket->show_min(agent->log_small, memetico::PREC, 0, memetico::PrintExcel, false); agent->log_small << endl;

                agent->members[Agent<U>::POCKET] = temp_pocket;
                agent->stat_local_search_pocket_improve++;
                agent->evaluate(data);
            }
        }
    }

    bubble();
}

/**
 * Bubble fitter children pocket solutions up the population
 *
 * @return  void
 * @bug two solutions on depth 2 may increase and be better than the pocket. The best will be bubbled to the root
 *      and the previous root pocket will be placed on depth 1. In this event, it is not bubbled with the children
 *      of depth 2 which may bethe second solution that is fitter. This leads to an (unlikely) scenario where the 
 *      previous pocket on depth 1 is less fit than an agent on depth 2. This is corrected in the next iteration,
 *      but if utilising deeper trees this could lead to larger issues. This has been observed with seed 2073724172
 *      and code as of 210921
 */
template <class U>
void Population<U>::bubble(Agent<U>* agent, size_t child_number) {
    
    // Move to the deepest and left-most parent
    if( agent == nullptr ) {

        // Move down left leg until the child is found
        agent = root_agent;
        while(!agent->is_leaf())
            agent = agent->children[0];

        // Go back to the parent. From here we can buble
        agent = agent->parent;

    }

    if(agent->is_leaf())
        return;

    //cout << "Bubbling " << agent->number << endl;
    agent->bubble();

    if( agent->parent == nullptr )
        return;

    // If we have processed all siblings for this level
    if( child_number+1 == Agent<U>::DEGREE )
        bubble(agent->parent, 0);
    else
        bubble(agent->parent->children[child_number+1], child_number+1);
    
}

/**
 * Evaluate the entire Population
 *
 * @return  void
 * 
 */
template <class U>
void Population<U>::evaluate(Agent<U>* agent) {
    
    // Start from the root agent
    if( agent == nullptr )
        agent = root_agent;
    
    // Evaluate the current agent
    agent->evaluate(data);

    if(agent->is_leaf())
        return;

    // Evaluate all children
    for( size_t i = 0; i < Agent<U>::DEGREE; i++ )
        evaluate(agent->children[i]);
    
}

template <class U>
void Population<U>::generational_summary(Agent<U>* agent) {

    // Start from the root agent
    if( agent == nullptr )
        agent = root_agent;
    
    // Evaluate the current agent
    memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",PopulationBestFitnessPocket," << agent->number;
    memetico::master_log << ",\""; agent->members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << agent->members[Agent<U>::POCKET]->fitness << "," << agent->members[Agent<U>::POCKET]->error;
    memetico::master_log << endl;

    if(agent->is_leaf())
        return;

    // Evaluate all children
    for( size_t i = 0; i < Agent<U>::DEGREE; i++ )
        generational_summary(agent->children[i]);

}