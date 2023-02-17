
/** 
 * @file
 * @brief See agent.h
 */

template <class U>
Agent<U>::Agent(size_t agent_depth, size_t parent_number, size_t degree_number) {

    // Check Static variables are set
    if( Agent<U>::DEGREE == 0)
        throw runtime_error("Agent::DEGREE is not set before constructing agent");

    // We have created all the agents
    if(agent_depth > Agent<U>::MAX_DEPTH)
        return;
    
    // Set depth of current Agent
    depth = agent_depth;

    // Set agent number
    // For the root agent the number is 0
    if(depth == 0) {
        number = 0;
    }
    // For others, we need to determine the shift across sibling branches
    else {

        size_t parent_depth_start_node = 0;
        size_t parent_depth_items = pow(Agent<U>::DEGREE, depth-1);;
        size_t this_depth_start_node = 0;

        // Sum nodes at each depth to determine the first agent in the parent depth
        for(size_t i = 0; i < depth-1; i++)
            parent_depth_start_node += pow(Agent<U>::DEGREE, i);

        // Sum nodes at each depth to determine the first agent in the current depth
        this_depth_start_node = parent_depth_start_node+parent_depth_items;

        // Determin how many degrees right the current agent is from the left most agent, then multiply by the number of children for each sibling
        size_t sibling_shift = (parent_number - parent_depth_start_node)*Agent<U>::DEGREE;

        // From the start of this depth, jump over the sibilings, then jump the current childs degree across to determine the agent number
        number = this_depth_start_node + sibling_shift + degree_number;

    }

    // Create members of derived model type U
    members = new U*[2];
    members[Agent<U>::POCKET] = new U();
    members[Agent<U>::CURRENT] = new U();

    evaluate(Agent<U>::TRAIN);

    // Master Log of construction
    //memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentConstruct," << number;
    //memetico::master_log << ",\""; members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << members[Agent<U>::POCKET]->->get_fitness() << "," << members[Agent<U>::POCKET]->get_error();
    //memetico::master_log << ",\""; members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << members[Agent<U>::CURRENT]->get_fitness() << "," << members[Agent<U>::CURRENT]->get_error();
    //memetico::master_log << endl;

    // Assume root node that has no parent (we update this later)
    this->parent = nullptr;

    // If not a leaf node
    if ( !is_leaf() ) {

        // Allocate space for the children
        children = new Agent*[Agent::DEGREE];

        // Construct children and associate their parent
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            
            children[i] = new Agent<U>(depth+1, number, i);
            children[i]->parent = this;
        }
    } 
    // If it is a leaf node, set children to false
    else {
        children = nullptr;
    }
}

/**
 * @brief Deconstruct Agent recursively
 * 
 * - Deallocate pocket and current memebers, and th members array in this Agent
 * - Deallocate each child agent, and the child array in this Agent
 * 
 * @return  void
 */
template <class U>
Agent<U>::~Agent() {

    // Continue to delete all children
    if( !is_leaf() ) {

        // Deconstruct each child
        for (size_t i = 0; i < Agent::DEGREE; i++)
            delete children[i];

        // Deconstruct array for children
        delete[] children;
    }

    // Remove solutions
    delete members[Agent<U>::POCKET];
    delete members[Agent<U>::CURRENT];
    
    delete[] members;
    
}

template <class U>
void Agent<U>::bubble() {

    // Keep track of best
    double best_fitness = numeric_limits<double>::max();
    size_t best_child = 0;

    // Check all children
    for(size_t i = 0; i < Agent::DEGREE; i++) {

        children[i]->evaluate(Agent::TRAIN);

        // If best child so far, save
        if(children[i]->members[Agent::POCKET]->get_fitness() < best_fitness) {
            best_child = i;
            best_fitness = children[i]->members[Agent::POCKET]->get_fitness();
        }   
    }

    evaluate(Agent::TRAIN);
    
    // Change if best child beats parent
    if( best_fitness < members[Agent::POCKET]->get_fitness() ) {

        // Swap pockets of the parent and best child
        U* parent_pocket = members[Agent::POCKET];
        members[Agent::POCKET] = children[best_child]->members[Agent::POCKET];
        children[best_child]->members[Agent::POCKET] = parent_pocket;

        // Both the pocket and current of the child may be better than the pocket of the parent
        // When we bubble the child pocket up, and the parent pocket down, the childs pocket
        // may actually be less fit than the childs current, so we exchange there
        //children[best_child]->log_small << "Bubbling Agent with Parent " << number << " Pocket Fitness: " <<  members[Agent<U>::POCKET]->get_fitness() << " with Agent " << children[best_child]->number << " Pocket (Child: " << best_child << ") Child Pocket Fitness: " << children[best_child]->members[Agent<U>::POCKET]->get_fitness() << endl; 
        children[best_child]->exchange();
        
    }
}

template <class U>
void Agent<U>::show(ostream& out, size_t precision, bool minimal) {

    if( number == 0 ) {
        cout << "==================" << endl;
        cout << "Population" << endl;
        cout << "==================" << endl;
    }

    out << "   Agent " << number << endl;
    out << " Pocket: " << *members[Agent<U>::POCKET] << " error: " << members[Agent<U>::POCKET]->get_error() << " " << endl;
    out << "Current: " << *members[Agent<U>::CURRENT] << " error: " << members[Agent<U>::CURRENT]->get_error() << " " << endl << endl;

    // If not a leaf node
    if ( !is_leaf() ) {

        // Show children
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            children[i]->show(out, precision, minimal);
        }
    } 
}

template <class U>
void Agent<U>::show_errors(ostream& out, size_t precision, DataSet* data) {

    if( data == nullptr)
        throw runtime_error("Must provide data to show_errors()");

    size_t spacing = precision+5;

    out << "Pocket Error Statistics for Agent " << number << endl;
    out << "---------------------------------" << endl;

    vector<size_t> selection = vector<size_t>();

    out << setw(spacing) << " MSE " << objective::mse(members[Agent<U>::POCKET], data, selection) << endl;
    out << setw(spacing) << " NMSE " << objective::nmse(members[Agent<U>::POCKET], data, selection) << endl;

    out << endl;

}

template <class U>
void Agent<U>::show_solution(ostream& out, size_t precision, DataSet* train, DataSet* test) {

    // Make sure its without penalty
    double temp_penalty = memetico::PENALTY;
    memetico::PENALTY = 0;

    vector<size_t> selection = vector<size_t>();

    //out << "Seed, Duration, Model Excel, Model Numpy, Model Latex, Objective, t.Error, t.Penalty, t.Fitness, T.Error, T.Penalty, T.Fitness, t.MSE, t.NMSE, T.MSE, T.NMSE" << endl;
    out << "Seed,Train MSE,Test MSE,Dur,Model,Train X2,Train X2 %,Train NMSE,Test NMSE" << endl;
    
    out << memetico::SEED;
    out << "," << objective::mse(members[Agent<U>::POCKET], train, selection);

    if( test != nullptr )       out << "," << objective::mse(members[Agent<U>::POCKET], test, selection);
    else                        out << ",";

    out << "," << memetico::RUN_TIME/1000.0;

    out << ",\"" << *members[Agent<U>::POCKET] << "\"";
    out << ",,";

    out << "," << objective::nmse(members[Agent<U>::POCKET], train, selection);
    if( test != nullptr )       out << "," << objective::nmse(members[Agent<U>::POCKET], test, selection);
    else                        out << ",";

    memetico::PENALTY = temp_penalty;

}
