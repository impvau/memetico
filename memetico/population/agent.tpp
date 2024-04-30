
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

    // Set agent number e.g. for a depth 2 ternary tree
    // 
    //               0    
    //        1      2       3
    //     4 5 6   7 8 9  10 11 12
    // 
    // For the root agent the number is 0
    if(depth == 0) {
        number = 0;
        parent = nullptr;
    }
    // For others
    else {

        size_t parent_depth_start_node = 0;
        size_t parent_depth_items = power(Agent<U>::DEGREE, depth-1);;
        size_t this_depth_start_node = 0;

        // Sum nodes at each depth to determine the first agent in the parent depth
        for(size_t i = 0; i < depth-1; i++)
            parent_depth_start_node += power(Agent<U>::DEGREE, i);

        // Sum nodes at each depth to determine the first agent in the current depth
        this_depth_start_node = parent_depth_start_node+parent_depth_items;

        // Determin how many degrees right the current agent is from the left most agent, then multiply by the number of children for each sibling
        size_t sibling_shift = (parent_number - parent_depth_start_node)*Agent<U>::DEGREE;

        // From the start of this depth, jump over the sibilings, then jump the current childs degree across to determine the agent number
        number = this_depth_start_node + sibling_shift + degree_number;

    }

    // Create members of derived model type U
    U pocket = U();
    members.push_back(pocket);
    U current = U();
    members.push_back(current);

    //evaluate(Agent<U>::TRAIN);

    // If not a leaf node
    if ( !is_leaf() ) {

        // Construct children and associate their parent
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            
            // We can't get around dynamic allocation
            // If we do not, the object destructs by the end of the function
            // Thus we have ~Agent loop through all children and delete
            Agent<U>* child = new Agent<U>(depth+1, number, i);
            children.push_back(child);
            children[i]->parent = this;
        }
    } 
    
}

template <class U>
Agent<U>::~Agent() {

    // Recursively delete all children
    for (size_t i = 0; i < children.size(); i++) {
        delete children[i];
    }

}

template <class U>
void Agent<U>::bubble() {

    // Keep track of best
    double best_fitness = numeric_limits<double>::max();
    size_t best_child = 0;

    // Determine best child
    for(size_t i = 0; i < Agent::DEGREE; i++) {

        // If best child so far, save
        if(children[i]->get_pocket().get_fitness() < best_fitness) {
            best_child = i;
            best_fitness = children[i]->get_pocket().get_fitness();
        }   
    }
    
    // Change if best child beats parent
    if( best_fitness < get_pocket().get_fitness() ) {

        // Swap pockets of the parent and best child
        U parent = U(get_pocket());
        set_pocket(children[best_child]->get_pocket());
        children[best_child]->set_pocket(parent);

        // Just incase childs pocket and current are both more fit than the parent solution
        // We know that child pocket < parent pocket < parent current so no need to check parent
        if(children[best_child]->get_current().get_fitness() < children[best_child]->get_pocket().get_fitness())
            children[best_child]->exchange();

    }

}

template <class U>
void Agent<U>::exchange() {

    U pocket = get_pocket();
    set_pocket(get_current());
    set_current(pocket);
        
}

template <class U>
void Agent<U>::show(ostream& out, size_t precision, bool minimal) {

    if( number == 0 ) {
        cout << "==================" << endl;
        cout << "Population" << endl;
        cout << "==================" << endl;
    }

    out << "   Agent " << number << endl;
    out << " Pocket: " << members[Agent<U>::POCKET] << " fitness: " << members[Agent<U>::POCKET].get_fitness() << " " << endl;
    out << "Current: " << members[Agent<U>::CURRENT] << " fitness: " << members[Agent<U>::CURRENT].get_fitness() << " " << endl << endl;

    // If not a leaf node
    if ( !is_leaf() ) {

        // Show children
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            children[i]->show(out, precision, minimal);
        }
    } 
}
