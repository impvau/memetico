
/**
 * @file
 * @author Andrew Ciezak <andy@ium.solutions>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @version 1.0
 * @brief Implementation of the Agent class
 */

/**
 * @brief Construct Agent
 * 
 * - Set the agent degree to argument
 * - Create memebers solutuons in Agent<U>::POCKET and Agent<U>::CURRENT positions
 * - Assign agent number based on a zero value for the root and incrementing left to right, top to bottom
 * - For Agents that are not at Agent<U>::MAX_DEPTH
 * -- Create Agent<U>::DEGREE number of children (recursively calling this constructor)
 * -- Set the child Agents parent to this agent
 * - For Agents that are at Agent<U>::MAX_DEPTH, set the children to null
 * 
 * Exceptions
 * - throws runtime_error() When Agent::DEGREE is not assigned to a non-zero value before consutrction
 * 
 * @param agent_depth Zero-based agent tree depth
 * @param parent_number Zero-based agent number, from top to bottom, left to right
 * @param child_number Zero-based child position between 0 to Agent::DEGREE
 * 
 * @return  Agent
 */
template <class U>
Agent<U>::Agent(size_t agent_depth, size_t parent_number, size_t child_number) {

    // Stat Fields
    /*
    stat_local_search_pocket = 0;
    stat_local_search_pocket_improve = 0;
    stat_local_search_pocket_nm_iters = 0;
    stat_local_search_pocket_nm_time = 0;
    stat_local_search_current = 0;
    stat_local_search_current_improve = 0;
    stat_local_search_current_nm_iters = 0;
    stat_local_search_current_nm_time = 0;
    stat_mutate = 0;
    stat_recombine = 0;
    stat_pocket_update = 0;
    stat_renew = 0;
    stat_bubble_up = 0;
    */

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
        number = this_depth_start_node + sibling_shift + child_number;

    }

    //
    //log = ofstream(memetico::LOG_DIR+to_string(memetico::SEED)+".Agent."+to_string(number)+".csv");
    //log_small = ofstream(memetico::LOG_DIR+to_string(memetico::SEED)+".Agent."+to_string(number)+".min.csv");
    //if( !log.is_open() or !log_small.is_open())
    //    throw runtime_error("Unable to open agent " +to_string(number)+ " log" );

    // Create members of derived model type U
    members = new U*[2];
    members[Agent<U>::POCKET] = new U();
    members[Agent<U>::CURRENT] = new U();
    evaluate(Agent<U>::TRAIN);
    
    // Master Log of construction
    //memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentConstruct," << number;
    //memetico::master_log << ",\""; members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << members[Agent<U>::POCKET]->fitness << "," << members[Agent<U>::POCKET]->error;
    //memetico::master_log << ",\""; members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << members[Agent<U>::CURRENT]->fitness << "," << members[Agent<U>::CURRENT]->error;
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

    // Debug
    if( memetico::DEBUG )
        cout << "~Agent()" << endl;

    // Remove members memeory
    //if( members[Agent<U>::POCKET] != nullptr )      delete members[Agent<U>::POCKET];
    //if( members[Agent<U>::CURRENT] != nullptr )     delete members[Agent<U>::CURRENT];
    
    delete[] members;

    // Continue to delete all children
    if( !is_leaf() ) {

        // Deconstruct each child
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            delete children[i];
        }

        // Deconstruct array for children
        delete[] children;
    }
}

/**
 * @brief Determine if this Agent is a leaf Agent
 *
 * - Return true if depth is Agent<U>::MAX_DEPTH 
 *
 * @return  bool
 */
template <class U>
bool Agent<U>::is_leaf() {
    return depth == Agent<U>::MAX_DEPTH;
}

/**
 * @brief Swap pocket and current if the current solution is better than the pocket
 * 
 * @return  void
 */
template <class U>
void Agent<U>::exchange() {

    if( members[Agent<U>::POCKET]->fitness > members[Agent<U>::CURRENT]->fitness) {

        // Master Log of construction
        memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN << ",AgentSwap," << number;
        memetico::master_log << ",\""; members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << members[Agent<U>::POCKET]->fitness << "," << members[Agent<U>::POCKET]->error;
        memetico::master_log << ",\""; members[Agent<U>::CURRENT]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintExcel, false); memetico::master_log << "\"," << members[Agent<U>::CURRENT]->fitness << "," << members[Agent<U>::CURRENT]->error;
        memetico::master_log << endl;

        //log << "Exchanging Pocket and Current... " << endl;
        //log_small << "Exchaning Pocket " <<  members[Agent<U>::POCKET]->fitness << " Current " << members[Agent<U>::CURRENT]->fitness << endl;
        //log << setw(20) << " Pocket: "; members[Agent<U>::POCKET]->show_min(log, memetico::PREC);
        //log << setw(20) << " Current: "; members[Agent<U>::CURRENT]->show_min(log, memetico::PREC);
        
        //stat_pocket_update++;
        U* temp_model = members[Agent<U>::POCKET];
        members[Agent<U>::POCKET] = members[Agent<U>::CURRENT];
        members[Agent<U>::CURRENT] = temp_model;

    }
}

/**
 * @brief Bubble a single Agent with its children
 * 
 * - Determine the child with the fittest pocket
 * - If the fittest child pocket is more fit than the parent pocket
 * -- Trade the child and parent pocket members
 * -- exchange() the child to ensure pocket and current are correct
 * 
 * @return  void
 */
template <class U>
void Agent<U>::bubble() {

    // Keep track of best
    double best_fitness = numeric_limits<double>::max();
    size_t best_child = 0;

    // Check all children
    for(size_t i = 0; i < Agent::DEGREE; i++) {

        // If best child so far, save
        if(children[i]->members[Agent::POCKET]->fitness < best_fitness) {
            best_child = i;
            best_fitness = children[i]->members[Agent::POCKET]->fitness;
        }   
    }

    // Change if best child beats parent
    if( best_fitness < members[Agent::POCKET]->fitness ) {

        //log_small << "Bubbling Agent " << number << " Pocket Fitness: " <<  members[Agent<U>::POCKET]->fitness << " with Agent " << children[best_child]->number << " Pocket (Child: " << best_child << ") Child Pocket Fitness: " << children[best_child]->members[Agent<U>::POCKET]->fitness << endl; 
        // Master Log of construction
        //memetico::master_log << duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() << "," << memetico::GEN <<  ",AgentBubbleUp," << number;
        //memetico::master_log << ",\""; members[Agent<U>::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << members[Agent<U>::POCKET]->fitness << "," << members[Agent<U>::POCKET]->error;
        //memetico::master_log << "," << children[best_child]->number;
        //memetico::master_log << ",\""; children[best_child]->members[Agent::POCKET]->show_min(memetico::master_log, memetico::PREC, 0, memetico::PrintNumpy, false); memetico::master_log << "\"," << children[best_child]->members[Agent::POCKET]->fitness << "," << children[best_child]->members[Agent::POCKET]->error;
        //memetico::master_log << endl;
        
        //log_small << "\t Parent: "; members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;
        //log_small << "\t  Child: "; children[best_child]->members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;

        // Swap pockets of the parent and best child
        U* parent_pocket = members[Agent::POCKET];
        //log_small << "\t Parent: "; parent_pocket->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;
        //log_small << "\t Pointers Temp:" << parent_pocket << " member:" << members[Agent::POCKET] << " child: " << children[best_child]->members[Agent::POCKET] << endl;
        members[Agent::POCKET] = children[best_child]->members[Agent::POCKET];
        children[best_child]->members[Agent::POCKET] = parent_pocket;

        //log_small << "After Switch" << endl;
        //log_small << "\t Parent: "; members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;
        //log_small << "\t  Child: "; children[best_child]->members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;

        // Both the pocket and current of the child may be better than the pocket of the parent
        // When we bubble the child pocket up, and the parent pocket down, the childs pocket
        // may actually be less fit than the childs current, so we exchange there
        //children[best_child]->log_small << "Bubbling Agent with Parent " << number << " Pocket Fitness: " <<  members[Agent<U>::POCKET]->fitness << " with Agent " << children[best_child]->number << " Pocket (Child: " << best_child << ") Child Pocket Fitness: " << children[best_child]->members[Agent<U>::POCKET]->fitness << endl; 
        children[best_child]->exchange();
        //children[best_child]->stat_bubble_up++;

        //log_small << "After Child Exchange" << endl;
        //log_small << "\t Parent: "; members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;
        //log_small << "\t  Child: "; children[best_child]->members[Agent::POCKET]->show_min(log_small, memetico::PREC, 0, memetico::PrintExcel, false); log_small << endl;

    }
}

/**
 * @brief Reinitialise the Agent CURRENT solution with a new U Model
 * 
 * @return  void
 */
template <class U>
void Agent<U>::renew() {

    //stat_renew++;

    delete members[Agent::CURRENT];
    members[Agent::CURRENT] = new U();
    evaluate(Agent::TRAIN);

    //log << "Renewing Current.. " << endl;
    //log_small << "Renew Current " <<  members[Agent<U>::CURRENT]->error << endl; 
    //log << setw(20) << "Current Pocket: "; members[Agent<U>::CURRENT]->show_min(log, memetico::PREC);

    exchange();
}

/**
 * @brief Evaluate Agent current solution and pocket solution based on just_current
 * 
 * @param train training DataSet
 * @param just_current flag to only evaulate current solution and leave pocket unevaluated
 * 
 * @return void
 */
template <class U>
void Agent<U>::evaluate(DataSet* train) {
    
    vector<size_t> selected = vector<size_t>();

    Agent<U>::OBJECTIVE(members[Agent<U>::POCKET], train, selected);        
    Agent<U>::OBJECTIVE(members[Agent<U>::CURRENT], train, selected);

    memetico::do_debug = false;
}

/** 
 * @brief Ouput the Agent parameter names and values
 * 
 * @param out           Write stream 
 * @param precision     Decimal precision  
 * @param print_type    Output format from memetico::PrintType
 * @param minimal       Display smallest unit of information
 * @bug this is techncailly most appropariate to be divided between Agent and Population,
 *      but it is a little annoying to keep the function interface (without and Agent*)
 *      and look through the agents from Population. This is due to the manner we need to
 *      move from sibling to sibling. This can be solved, but put aside for the moment
 * 
 * @return void
 */
template <class U>
void Agent<U>::show(ostream& out, size_t precision, bool minimal) {

    out << "Agent " << number << endl;

    if( !minimal ) {
        members[Agent<U>::POCKET]->show(out, precision);
        members[Agent<U>::CURRENT]->show(out, precision);
    } else {
        out << setw(10) << " Pocket: "; members[Agent<U>::POCKET]->show_min(out, precision);
        out << endl;
        out << setw(10) << " Current: "; members[Agent<U>::CURRENT]->show_min(out, precision);
    }

    // If not a leaf node
    if ( !is_leaf() ) {

        // Show children
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            children[i]->show(out, precision, minimal);
        }
    } 
}

/** 
 * @brief Ouput the Agent statistics
 * 
 * @param out           Write stream 
 * @param precision     Decimal precision  
 * @param print_type    Output format from memetico::PrintType
 * @param minimal       Display smallest unit of information
 * @bug there is a separate class in the works <memetico/stats/stat.hpp> in the works,
 *      until then the information is stored within the Agent attributes
 * 
 * @return void
 */
/*
template <class U>
void Agent<U>::show_stats(ostream& out, size_t precision) {

    
    size_t big_space = 17;
    if( number == 0 ) {
        
        size_t full_width = big_space*16;

        // Approximate Centering
        cout << "Population Summary " << endl; 
        out << string(full_width+5,'-') <<endl;
        // Print Header
        out << setw(big_space)      << " Agent ";
        out << setw(big_space)      << " #.LS.Pock";
        out << setw(big_space)      << " #.LS+.Pock";
        out << setw(big_space)      << " #.LS.Pock.Time";
        out << setw(big_space)      << " #.LS.Pock.Iters";
        out << setw(big_space)      <<  " #.LS.Pock.Avg";
        out << setw(big_space)      << " #.LS.Cur";
        out << setw(big_space)      << " #.LS+.Cur";
        out << setw(big_space)      << " #.LS.Crt.Time";
        out << setw(big_space)      << " #.LS.Crt.Iters";
        out << setw(big_space)      << " #.LS.Crt.Avg";
        out << setw(big_space)      << " #.Mutates";
        out << setw(big_space)      << " #.Recomb";
        out << setw(big_space)      << " #.Pocket+";
        out << setw(big_space)      << " #.Renew";
        out << setw(big_space)      << " #.Propa";
        out << endl;
        out << string(full_width+5,'-') <<endl;

    }
    
    // Print Agent line
    out << setw(big_space)     << number;
    out << setw(big_space)     << stat_local_search_pocket;
    out << setw(big_space)     << stat_local_search_pocket_improve;
    out << setw(big_space)      << stat_local_search_pocket_nm_time;
    out << setw(big_space)      << stat_local_search_pocket_nm_iters;

    if(stat_local_search_pocket_nm_iters > 0)
        out << setw(big_space)      << stat_local_search_pocket_nm_time/stat_local_search_pocket_nm_iters;
    else
        out << setw(big_space)      << "-";

    out << setw(big_space)     << stat_local_search_current;
    out << setw(big_space)     << stat_local_search_current_improve;
    out << setw(big_space)      << stat_local_search_current_nm_time;
    out << setw(big_space)      << stat_local_search_current_nm_iters;
    if(stat_local_search_current_nm_iters > 0)
        out << setw(big_space)      << stat_local_search_current_nm_time/stat_local_search_current_nm_iters;
    else
        out << setw(big_space)      << "-";
    out << setw(big_space)     << stat_mutate;
    out << setw(big_space)     << stat_recombine;
    out << setw(big_space)     << stat_pocket_update;
    out << setw(big_space)     << stat_renew;
    out << setw(big_space)     << stat_bubble_up;
    out << endl;

    // If not a leaf node
    if ( !is_leaf() ) {

        // Show children
        for (size_t i = 0; i < Agent::DEGREE; i++) {
            children[i]->show_stats(out, precision);
        }
    } 

    // Add a break at the end of the table
    if( number == 0 )
        out << endl;
    
}
*/

/** 
 * @brief Ouput the Agent error statistics
 * 
 * @param out           Write stream 
 * @param precision     Decimal precision  
 * 
 * @return void
 */
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

/** 
 * @brief Ouput the Agent error statistics
 * 
 * @param out           Write stream 
 * @param precision     Decimal precision  
 * 
 * @return void
 */
template <class U>
void Agent<U>::show_solution(ostream& out, size_t precision, DataSet* train, DataSet* test) {

    vector<size_t> selection = vector<size_t>();

    //out << "Seed, Duration, Model Excel, Model Numpy, Model Latex, Objective, t.Error, t.Penalty, t.Fitness, T.Error, T.Penalty, T.Fitness, t.MSE, t.NMSE, T.MSE, T.NMSE" << endl;
    out << "Seed,Train MSE,Test MSE,Dur,Model,Train X2,Train X2 %,Train NMSE,Test NMSE" << endl;
    
    out << memetico::SEED;
    out << "," << objective::mse(members[Agent<U>::POCKET], train, selection);

    if( test != nullptr )       out << "," << objective::mse(members[Agent<U>::POCKET], test, selection);
    else                        out << ",";

    out << "," << memetico::RUN_TIME/1000.0;

    out << ",\""; members[Agent<U>::POCKET]->show_min(out, precision, 0, memetico::PrintExcel, false); out << "\"";
    out << ",,";

    out << "," << objective::nmse(members[Agent<U>::POCKET], train, selection);
    if( test != nullptr )       out << "," << objective::nmse(members[Agent<U>::POCKET], test, selection);
    else                        out << ",";

    /*
    // Fanciness to force excel to treat as string
    out << ",\""; members[Agent<U>::POCKET]->show_min(out, precision, 0, memetico::PrintExcel, false); out << "\"";
    out << ",\""; members[Agent<U>::POCKET]->show_min(out, precision, 0, memetico::PrintNumpy, false); out << "\"";
    out << ",\""; members[Agent<U>::POCKET]->show_min(out, precision, 0, memetico::PrintLatex, false); out << "\"";

    out << "," << Agent<U>::OBJECTIVE_NAME;
    Agent<U>::OBJECTIVE(members[Agent<U>::POCKET], train, selection);
    out << "," << members[Agent<U>::POCKET]->error;
    out << "," << members[Agent<U>::POCKET]->penalty;
    out << "," << members[Agent<U>::POCKET]->fitness;

    if( test != nullptr ) {

        Agent<U>::OBJECTIVE(members[Agent<U>::POCKET], test, selection);
        out << "," << members[Agent<U>::POCKET]->error;
        out << "," << members[Agent<U>::POCKET]->penalty;
        out << "," << members[Agent<U>::POCKET]->fitness;

    }

    out << "," << objective::mse(members[Agent<U>::POCKET], train, selection);
    out << "," << objective::nmse(members[Agent<U>::POCKET], train, selection);

    if( test != nullptr ) {

        out << "," << objective::mse(members[Agent<U>::POCKET], test, selection);
        out << "," << objective::nmse(members[Agent<U>::POCKET], test, selection);
    }
     
    out << endl;
    */

}

