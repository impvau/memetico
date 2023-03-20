
#include "doctest.h"
#include <memetico/helpers/rng.h>
#include <memetico/population/agent.h>
#include <memetico/population/pop.h>
#include <memetico/optimise/objective.h>
#include <memetico/optimise/local_search.h>
#include <memetico/models/cont_frac.h>
#include <memetico/models/regression.h>
#include <sstream>
#include <stdexcept>

typedef ContinuedFraction<double> AgentModel;

inline void init(string filename) {

    // Create sample data
    string fn(filename);
    ofstream f;
    f.open(fn);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn);

    f << "y,x" << endl;
    // Argon dataset, code generated in excel with =CONCAT("f << ",CHAR(34),A2, ",", B2, CHAR(34), " << endl;")
    f << "363.94,3.1" << endl;
    f << "58.41,3.3" << endl;
    f << "-14.92,3.4" << endl;
    f << "-58.41,3.5" << endl;
    f << "-81.69,3.6" << endl;
    f << "-88.36,3.65" << endl;
    f << "-92.56,3.7" << endl;
    f << "-94.73,3.75" << endl;
    f << "-95.39,3.8" << endl;
    f << "-94.75,3.85" << endl;
    f << "-93.16,3.9" << endl;
    f << "-90.89,3.95" << endl;
    f << "-88.01,4" << endl;
    f << "-81.19,4.1" << endl;
    f << "-73.63,4.2" << endl;
    f << "-65.98,4.3" << endl;
    f << "-58.6,4.4" << endl;
    f << "-51.78,4.5" << endl;
    f << "-45.6,4.6" << endl;
    f << "-40.13,4.7" << endl;
    f << "-35.34,4.8" << endl;
    f << "-31.17,4.9" << endl;
    f << "-27.57,5" << endl;
    f << "-24.46,5.1" << endl;
    f << "-19.37,5.3" << endl;
    f << "-15.47,5.5" << endl;
    f << "-12.43,5.7" << endl;
    f << "-10,5.9" << endl;
    f << "-8.13,6.1" << endl;
    f << "-6.63,6.3" << endl;
    f << "-5.44,6.5" << endl;
    f << "-4.49,6.7" << endl;

    f.close();

}

TEST_CASE("Population: bubble, exchange, evaluate") {

    // Tests
    // 1. Constrcut populations

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup data
    string fn = "test_data.csv";
    init(fn);
    DataSet data = DataSet(fn);
    data.load();
    
    AgentModel::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        AgentModel::IVS.push_back(DataSet::IVS[i]);

    // 1. Construct depth 2 ternary population
    Model::OBJECTIVE_NAME = "mse";
    Model::OBJECTIVE = objective::mse<AgentModel>;

    // For 1000 random populations
    for( size_t i = 0; i < 1000; i++ ) {

        Population<AgentModel> p = Population<AgentModel>(&data, 2, 3);
        REQUIRE( Population<AgentModel>::DEPTH == 2);
        REQUIRE( Agent<AgentModel>::MAX_DEPTH == 2);
        REQUIRE( Agent<AgentModel>::DEGREE == 3);
        REQUIRE( p.data == &data );

        // THESE TESTS are premature; we are assessing Population<AgentModel>::bubble, ...::exchange, ...::evaluate as part of this
        // 1.1 We expect all pocket fitness < all currentness
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_current().get_fitness() );
        // Root children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_current().get_fitness() );
        // Roots left child children
        REQUIRE( p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_current().get_fitness() );
        // Roots middle child children
        REQUIRE( p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_current().get_fitness() );
        // Roots right child children
        REQUIRE( p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_current().get_fitness() );
        // 1.2 We expect each parent pocket fitness < any of their children
        // Root better than children 
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_pocket().get_fitness() );
        // Roots left child vs children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() );
        // Roots middle child vs children
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() );
        // Roots right child vs children
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() );

    }
}

TEST_CASE("Population: local_search, local_search_agents") {

    // Tests
    // 1. local search on a single agent
    // 2. local search on the entire population
    
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup data
    string fn = "test_data.csv";
    init(fn);
    DataSet data = DataSet(fn);
    data.load();
    
    AgentModel::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        AgentModel::IVS.push_back(DataSet::IVS[i]);

    // 1. Construct depth 2 ternary population
    Model::OBJECTIVE_NAME = "mse";
    Model::OBJECTIVE = objective::mse<AgentModel>;
    MemeticModel<double>::LOCAL_SEARCH = local_search::custom_nelder_mead_redo<MemeticModel<double>>;
    Population<AgentModel> p = Population<AgentModel>(&data, 2, 3);
    double best_score = p.best_soln.get_fitness();
    meme::LOCAL_SEARCH_DATA_PCT = 0.5;
    // Run LS 10 times, ensure that fitness stays the same or improves
    for(size_t i = 0; i < 20; i++) {
        double fitness = p.root_agent->get_pocket().get_fitness();
        p.local_search_agent(p.root_agent);
        REQUIRE(p.root_agent->get_pocket().get_fitness() <= fitness);

        // Ensure that local search results in  
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_current().get_fitness() );
        // Root children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_current().get_fitness() );
        // Roots left child children
        REQUIRE( p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_current().get_fitness() );
        // Roots middle child children
        REQUIRE( p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_current().get_fitness() );
        // Roots right child children
        REQUIRE( p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_current().get_fitness() );
        // 1.2 We expect each parent pocket fitness < any of their children
        // Root better than children 
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_pocket().get_fitness() );
        // Roots left child vs children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() );
        // Roots middle child vs children
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() );
        // Roots right child vs children
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() );
    }

    // 2. 
    for(size_t i = 0; i < 20; i++) {
        double fitness = p.root_agent->get_pocket().get_fitness();
        p.local_search(p.root_agent);
        REQUIRE(p.root_agent->get_pocket().get_fitness() <= fitness);

        // Ensure that local search results in  
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_current().get_fitness() );
        // Root children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_current().get_fitness() );
        // Roots left child children
        REQUIRE( p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_current().get_fitness() );
        // Roots middle child children
        REQUIRE( p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_current().get_fitness() );
        // Roots right child children
        REQUIRE( p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_current().get_fitness() );
        // 1.2 We expect each parent pocket fitness < any of their children
        // Root better than children 
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_pocket().get_fitness() );
        // Roots left child vs children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() );
        // Roots middle child vs children
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() );
        // Roots right child vs children
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() );

    }

}

TEST_CASE("Population: evolve") {

    // Tests
    
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup data
    string fn = "test_data.csv";
    init(fn);
    DataSet data = DataSet(fn);
    data.load();
    
    AgentModel::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        AgentModel::IVS.push_back(DataSet::IVS[i]);

    // 1. 
    Model::OBJECTIVE_NAME = "mse";
    Model::OBJECTIVE = objective::mse<AgentModel>;
    MemeticModel<double>::LOCAL_SEARCH = local_search::custom_nelder_mead_redo<MemeticModel<double>>;
    Population<AgentModel> p = Population<AgentModel>(&data, 2, 3);
    
    // Run LS many times, ensure that fitness stays the same or improves
    for(size_t i = 0; i < 20; i++) {

        double fitness = p.root_agent->get_pocket().get_fitness();
        p.evolve();
        REQUIRE(p.root_agent->get_pocket().get_fitness() <= fitness);

        // Ensure that local search results in  
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_current().get_fitness() );
        // Root children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_current().get_fitness() );
        // Roots left child children
        REQUIRE( p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_current().get_fitness() );
        // Roots middle child children
        REQUIRE( p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_current().get_fitness() );
        // Roots right child children
        REQUIRE( p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_current().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_current().get_fitness() );
        // 1.2 We expect each parent pocket fitness < any of their children
        // Root better than children 
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_pocket().get_fitness() );
        // Roots left child vs children
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[0]->get_pocket().get_fitness() < p.root_agent->get_children()[0]->get_children()[2]->get_pocket().get_fitness() );
        // Roots middle child vs children
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[1]->get_pocket().get_fitness() < p.root_agent->get_children()[1]->get_children()[2]->get_pocket().get_fitness() );
        // Roots right child vs children
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[0]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[1]->get_pocket().get_fitness() );
        REQUIRE( p.root_agent->get_children()[2]->get_pocket().get_fitness() < p.root_agent->get_children()[2]->get_children()[2]->get_pocket().get_fitness() );
    }    

}

/*
TEST_CASE("Population: run ") {

    // Tests
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup data
    string fn = "test_data.csv";
    init(fn);
    DataSet data = DataSet(fn);
    data.load();
    
    AgentModel::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        AgentModel::IVS.push_back(DataSet::IVS[i]);

    // 1. 
    Model::OBJECTIVE_NAME = "mse";
    Model::OBJECTIVE = objective::mse<AgentModel>;
    MemeticModel<double>::LOCAL_SEARCH = local_search::custom_nelder_mead_redo<MemeticModel<double>>;
    
    Population<AgentModel> p = Population<AgentModel>(&data, 2, 3);
    p.run();

}
*/