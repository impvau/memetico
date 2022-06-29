
/** @file
 * @brief See pop.h
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

template <class U>
void Population<U>::run() {

    cout << "==================" << endl;
    cout << "Starting Memetico" << endl;
    cout << "==================" << endl;

    // Start timer
    auto start_time = chrono::system_clock::now();

    // Setup helper vars
    size_t stale = 0;           // Number of stale generations
    double best_fitness = root_agent->get_pocket()->get_fitness();         // Best fitness known

    // Loop for generations
    for( memetico::GEN = 0; memetico::GEN < memetico::GENERATIONS; memetico::GEN++ ) {

        // Start timer
        auto generation_start = chrono::system_clock::now();

        evolve();

        // Local search after the indicated interval
        if( memetico::GEN % memetico::LOCAL_SEARCH_INTERVAL == 0 )
            local_search();
        
        // Increment stale when the solution has not improved
        if( root_agent->get_pocket()->get_fitness() >= best_fitness )
            stale++;
        else {    
            stale = 0;
            best_fitness = root_agent->get_pocket()->get_fitness();
            POCKET_DEPTH = root_agent->get_pocket()->get_depth();
            master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",RootUpdated";
            master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
            master_log << endl;
        }
            
        // Reset root pocket on stale
        if( stale > STALE_RESET ) {
            cout << "\tResetting stale root " << endl;
            root_agent->renew();
            root_agent->evaluate(data);
            stale = 0;
            
            // Note only a single bubble, so if it is a terrible solution, it moves down to depth 1 straight away
            // but still mixes with all the other agents of the population
            bubble();
        }

        auto generation_end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> generation_ms = generation_end-generation_start;
        cout << setw(5) << GEN << setw(10) << best_fitness <<  " (" << setw(10) << root_agent->get_pocket()->get_error() << ") " << setw(10) << " duration: " << setw(10) << generation_ms.count() << "ms root_depth: " << memetico::POCKET_DEPTH << " frac:" << *root_agent->get_pocket() << endl;
        
        master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << GEN << ",PopBest,";
        master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
        master_log << endl;

    }

    // End timer
    auto end_time = chrono::system_clock::now();
    memetico::RUN_TIME = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();

    // Output best solution and execution time
    cout << *root_agent->get_pocket() << endl;
    cout << endl << endl;
    cout << "Finished after " << memetico::RUN_TIME << " milliseconds" << endl;
    
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

    // Mutate based on chance
    if( RANDREAL() < MUTATE_RATE ) {

        agent->get_current()->mutate(agent->get_pocket());
        agent->evaluate(data);

        // Add record when mutation causes the best solution to be beaten
        if(agent->get_current()->get_fitness() < root_agent->get_pocket()->get_fitness()) {
            master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",NewBest," << agent->get_number() << ",Mutation";
            master_log << ",\"" << *agent->get_current() << "\"," << agent->get_current()->get_fitness() << "," << agent->get_current()->get_error();
            master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
            master_log << endl;
        }
        
        bubble();
    }
        
    // We can't recombine on the leaf depth as no children exist
    if( agent->is_leaf() )
        return;
    
    // Update parent current by recombing parent pocket and the last child current
    agent->get_current()->recombine(
        agent->get_pocket(), 
        agent->get_children()[Agent<U>::DEGREE-1]->get_current()
    );

    // Evaluate agent
    agent->evaluate(data);

    // Add record when recombination causes the best solution to be beaten
    if(agent->get_current()->get_fitness() < root_agent->get_pocket()->get_fitness()) {
        master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << GEN << ",NewBest," << agent->get_number() << ",Recombine";
        master_log << ",\"" << *agent->get_current() << "\"," << agent->get_current()->get_fitness() << "," << agent->get_current()->get_error();
        master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
        master_log << endl;
    }

    if( agent->exchange() )
        bubble();

    // Recombine and update the last childs current solution which depends on the parent
    agent->get_children()[Agent<U>::DEGREE-1]->get_current()->recombine(
        agent->get_children()[Agent<U>::DEGREE-1]->get_pocket(), 
        agent->get_current()
    );
    agent->get_children()[Agent<U>::DEGREE-1]->evaluate(data);
    agent->get_children()[Agent<U>::DEGREE-1]->exchange();

    // Reverse the exchange, last child and parent
    agent->get_current()->recombine(
        agent->get_pocket(), 
        agent->get_children()[Agent<U>::DEGREE-1]->get_current()
    );
    agent->get_children()[Agent<U>::DEGREE-1]->evaluate(data);
    agent->get_children()[Agent<U>::DEGREE-1]->exchange();

    // For the other children
    //for(size_t i = 0; i < Agent<U>::DEGREE-1; i++) {
    for(int i = Agent<U>::DEGREE-2; i >= 0; i--) {

        // Perform the similar recombination process
        agent->get_children()[i]->get_current()->recombine(
            agent->get_children()[i]->get_pocket(), 
            agent->get_children()[i+1]->get_current()
        );
        
        agent->get_children()[i]->evaluate(data);
        
        // Add record when recombination causes the best solution to be beaten
        if(agent->get_children()[i]->get_current()->get_fitness() < root_agent->get_pocket()->get_fitness()) {
            master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << GEN << ",NewBest," << agent->get_children()[i]->get_number() << ",Recombine";
            master_log << ",\"" << *agent->get_children()[i]->get_current() << "\"," << agent->get_children()[i]->get_current()->get_fitness() << "," << agent->get_children()[i]->get_current()->get_error();
            master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
            master_log << endl;
        }
        
        if( agent->get_children()[i]->exchange() )
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

    // Create copies of the pocket and current solutions
    U* temp_current = agent->get_current()->clone();
    U* temp_pocket = agent->get_pocket()->clone();
    double fitness = numeric_limits<double>::max();

    // For the number of local search iterations configured
    for(size_t j = 0; j < LOCAL_SEARCH_RUNS; j++ ) {

        // Selected a uniform at random local search, or select all
        vector<size_t> selected_idx;
        if( LOCAL_SEARCH_DATA_PCT < 1 )
            selected_idx = data->subset(LOCAL_SEARCH_DATA_PCT);
        else 
            selected_idx = vector<size_t>(); 
        
        // Perform local search
        fitness = Agent<U>::LOCAL_SEARCH(temp_current, data, selected_idx);
        
    }

    
    if(fitness < agent->get_current()->get_fitness())
        agent->set_current(temp_current);

    // Add record when mutation causes the best solution to be beaten
    if(agent->get_current()->get_fitness() < root_agent->get_pocket()->get_fitness()) {
        master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << GEN << ",NewBest," << agent->get_number() << ",LocalSearch1";
        master_log << ",\"" << *agent->get_current() << "\"," << agent->get_current()->get_fitness() << "," << agent->get_current()->get_error();
        master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
        master_log << endl;
    }

    // Local Search Pocket for a chance to avoid replacement
    if( agent->get_pocket()->get_fitness() > agent->get_current()->get_fitness() ) {

        // For the number of local search iterations configured
        for(size_t j = 0; j < memetico::LOCAL_SEARCH_RUNS; j++ ) {

            // Selected a uniform at random local search, or select all
            vector<size_t> selected_idx;
            if( memetico::LOCAL_SEARCH_DATA_PCT < 1 )
                selected_idx = data->subset(LOCAL_SEARCH_DATA_PCT);
            else 
                selected_idx = vector<size_t>();

            // Perform local search
            fitness = Agent<U>::LOCAL_SEARCH(temp_pocket, data, selected_idx);
            
        }

        if(fitness < agent->get_pocket()->get_fitness())    
            agent->set_pocket(temp_pocket);
    }

    // Add record when mutation causes the best solution to be beaten
    if(agent->get_pocket()->get_fitness() < root_agent->get_pocket()->get_fitness()) {
        master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << GEN << ",NewBest," << agent->get_number() << ",LocalSearch2";
        master_log << ",\"" << *agent->get_pocket() << "\"," << agent->get_pocket()->get_fitness() << "," << agent->get_pocket()->get_error();
        master_log << ",\"" << *root_agent->get_pocket() << "\"," << root_agent->get_pocket()->get_fitness() << "," << root_agent->get_pocket()->get_error();
        master_log << endl;
    }

    if( agent->exchange() )
        bubble();

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
    agent->evaluate(data);

    if(agent->is_leaf())
        return;

    // Evaluate all children
    for( size_t i = 0; i < Agent<U>::DEGREE; i++ )
        evaluate(agent->get_children()[i]);
    
}