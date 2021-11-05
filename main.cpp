
/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Entry point and harness for the memetic algorithm
 */

// Local
#include <memetico/data.h>
#include <memetico/globals.h>
#include <memetico/rng.h>
#include <memetico/pop.h>
#include <memetico/agent.h>
#include <memetico/local_search.h>
#include <memetico/objective.h>
// Std
#include <cstdlib>

using namespace std;

// Default Logic Globals
int             memetico::SEED = 42;
size_t          memetico::GENERATIONS = 200;
double          memetico::MUTATE_RATE = 0.2;
size_t          memetico::LOCAL_SEARCH_INTERVAL = 1;
size_t          memetico::STALE_RESET = 5;
bool            memetico::INT_ONLY = false;
size_t          memetico::LOCAL_SEARCH_RUNS = 4;
size_t          memetico::NELDER_MEAD_STALE = 10;
size_t          memetico::NELDER_MEAD_MOVES = 250;
double          memetico::LOCAL_SEARCH_DATA_PCT = 1;
double          memetico::PENALTY = 0.35;
size_t          memetico::GEN = 0;


// File Globals
string          memetico::TRAIN_FILE = "sinx.csv";
string          memetico::TEST_FILE = "sinx.csv";
string          memetico::LOG_DIR = "out/";
ofstream        memetico::master_log;


// Technical/Formatting Globals
size_t          memetico::PREC = 18;
bool            memetico::DEBUG = false;
size_t          memetico::RUN_TIME = 0;

// Global Heplers
RandReal        memetico::RANDREAL;
RandInt         memetico::RANDINT;

// Integer or Double models
typedef double DataType;
typedef ContinuedFraction<DataType> ModelType;

bool            memetico::do_debug = false;

/**
 */
void load_args(int argc, char * argv[]) {
   
    // Check argument exists
    auto arg_exists = [](char** begin, char** end, string short_option, string long_option) {
        return find(begin, end, short_option) != end || find(begin, end, long_option) != end;
    };

    // Get value after argument
    auto arg_value = [](char** begin, char** end, string short_option, string long_option) {
        
        // Check for short string e.g. "-h" rather than "-help"
        char ** itr = find(begin, end, short_option);
        if (itr != end && ++itr != end)
            return string(*itr);
        
        // Check for long string e.g.  "-help"
        itr = find(begin, end, long_option);
        if (itr != end && ++itr != end)
            return string(*itr);

        return string("");
    };

    
    // Temporary string
    string arg_string;
    
    // Seed
    arg_string = arg_value(argv, argv+argc, "-s", "--seed");
    if(arg_string != "")
        memetico::SEED = stoi(arg_string);
    else  {
        RandInt seed_init = RandInt();
        memetico::SEED = seed_init(1, numeric_limits<int>::max());
    }
    memetico::RANDINT = RandInt(memetico::SEED);
    memetico::RANDREAL = RandReal(memetico::SEED);
    
    // Log Directory
    arg_string = arg_value(argv, argv+argc, string("-lt"), string("--log-to"));
    if(arg_string != "") {
        memetico::LOG_DIR = arg_string;
        if( memetico::LOG_DIR.back() != '/' )
            memetico::LOG_DIR += "/";

        // Change STD out if the log dir is specified. If not, use normal stdout / stderr
        freopen( (memetico::LOG_DIR + to_string(memetico::SEED) + ".Std.log").c_str(), "a", stdout);
        freopen( (memetico::LOG_DIR + to_string(memetico::SEED) + ".Err.log").c_str(), "a", stderr);

    }
    // Master log
    memetico::master_log = ofstream(memetico::LOG_DIR+to_string(memetico::SEED)+".Master.log");
    
    // Redirect Std Out and Std In
    cout << "Random Seed: " << memetico::SEED << endl;
    
    // Train
    arg_string = arg_value(argv, argv+argc, string("-t"), string("--train"));
    if(arg_string != "")    memetico::TRAIN_FILE = arg_string;

    // Test
    arg_string = arg_value(argv, argv+argc, "-T", "--Test");
    if(arg_string != "")    memetico::TEST_FILE = arg_string;
    else                    memetico::TEST_FILE = memetico::TRAIN_FILE;
    


    // Mutation Rate
    arg_string = arg_value(argv, argv+argc, "-mr", "--mutate-rate");
    if(arg_string != "")    memetico::MUTATE_RATE = stod(arg_string);
    
    // Penalty
    arg_string = arg_value(argv, argv+argc, "-d", "--delta");
    if(arg_string != "")    memetico::PENALTY = stod(arg_string);

    // Generations
    arg_string = arg_value(argv, argv+argc, "-g", "--gens");
    if(arg_string != "")    memetico::GENERATIONS = stoi(arg_string);

    // Local Search Data
    arg_string = arg_value(argv, argv+argc, "-sd", "--split-data");
    if(arg_string != "")    Data::SPLIT = stod(arg_string);

    // Local Search Data
    arg_string = arg_value(argv, argv+argc, "-ld", "--local-data");
    if(arg_string != "")    memetico::LOCAL_SEARCH_DATA_PCT = stod(arg_string);
    
    // Local Search function
    arg_string = arg_value(argv, argv+argc, "-ls", "--local-search");
    if( arg_string == "cnm" ||
        arg_string == "")       Agent<ModelType>::LOCAL_SEARCH = local_search::custom_nelder_mead<ModelType>;
    
    // Objective function
    arg_string = arg_value(argv, argv+argc, "-o", "--objective");
    Agent<ModelType>::OBJECTIVE_NAME = arg_string;
    if( arg_string == "")                               Agent<ModelType>::OBJECTIVE_NAME = "mse";
    if( Agent<ModelType>::OBJECTIVE_NAME == "mse" )     Agent<ModelType>::OBJECTIVE = objective::mse<ModelType>;
    //if( Agent<ModelType>::OBJECTIVE_NAME == "nmse" )  Agent<ModelType>::OBJECTIVE = objective::mse<ModelType>;
    //if( Agent<ModelType>::OBJECTIVE_NAME == "cw" )    Agent<ModelType>::OBJECTIVE = objective::mse<ModelType>;
    //if( Agent<ModelType>::OBJECTIVE_NAME == "aic" )   Agent<ModelType>::OBJECTIVE = objective::mse<ModelType>;
    

    // CFR Specific
    // Depth
    arg_string = arg_value(argv, argv+argc, "-f", "--fracdepth");
    if(arg_string != "")    ContinuedFraction<DataType>::DEPTH = stoi(arg_string);
    
    
    // Help
    if(arg_exists(argv, argv+argc, "-h", "--help")) {
        
        cout << 
        
            R"(
                Memetico      
            

                    USAGE:

                        ./memetico-cpp/bin/main;

                    REQUIRED:

                        -lt --log-to <filedir>          Directory for output files
                                Defaults to <repo>/out/

                        -t --train <filepath>           Training full filepath


                    OPTIONAL:

                        -d --delta                      Penalty for the number of parameters within the solution (Real Number 0.0 <= d <= 1.0)
                                                        Expressed in the solution fitness. When no parameters are used Fitness = Error           
                                                        When parameters are used
                                                            Factor = 1+used_params()*Penalty
                                                            Fitness = Error*Factor
                                                        Defaults to 0.35


                        -f --fracdepth <integer>        Depth of continued fraction
                                                        Defaults to 4

                        -g --gens                       Number of generations to evolve the memetic population
                                                        Defaults to 200
                        
                        -ld --local-data                Percentage of data to use in local search between 0 and 1
                                                        Defaults to 1

                        -ls --local-search              Local Search method
                                                        Available Options: 
                                                            cnm
                                                        Defaults to "cnm

                        -o --objective                  Objective function identifier to drive learning
                                                        Available Options: 
                                                            mse, 
                                                            // nsme, 
                                                            // cw,
                                                            // aic
                                                        Defaults to "mse"

                        -mr --mutate-rate               Percentage of solutions that undergo mutation each generation between 0 and 1
                                                        Defaults to 0.2

                        -s --seed <integer>             Reproduction seed
                                                        Defaults to random integer between 1, numerical_limit<int>::max()


                        -T --Test <filepath>            Test data file
                                                        Defaults to training filepath from -t

                        
                                                        

            )";

            exit(EXIT_SUCCESS);
    }
}

/**
 * Entry point of the application
 * 
 * @param argc number of program arguments accessible in argv
 * @param argv array of arguments
 * @return int program succces
 */
int main(int argc, char *argv[]) {

    // Parse arguments
    load_args(argc, argv);

    // Parse data
    Data* store;
    if( memetico::TRAIN_FILE == memetico::TEST_FILE )
        store = new Data(memetico::TRAIN_FILE);
    else 
        store = new Data(memetico::TRAIN_FILE, memetico::TEST_FILE);

    // Train Output
    ofstream train_file(memetico::LOG_DIR+to_string(memetico::SEED)+".Train.csv");
    if (train_file.is_open())
        store->train->csv(train_file);
    
    // Test Output
    if(store->has_test) {
        ofstream test_file(memetico::LOG_DIR+to_string(memetico::SEED)+".Test.csv");
        if (test_file.is_open())
            store->test->csv(test_file);
    }
    else {
        ofstream test_file(memetico::LOG_DIR+to_string(memetico::SEED)+".Train.csv");
        if (test_file.is_open())
            store->test->csv(test_file);
    } 

    // Create Population
    Population<ModelType> p(store->train);

    // Run memetic algorithm on population
    p.run();

    // Display error metric scores
    p.root_agent->show_errors(cout, memetico::PREC, store->train);
    if(store->has_test)
        p.root_agent->show_errors(cout, memetico::PREC, store->test);

    cout << " Writing " << memetico::LOG_DIR+to_string(memetico::SEED)+".Run.csv" << endl;
    ofstream log(memetico::LOG_DIR+to_string(memetico::SEED)+".Run.csv");
    if (log.is_open())
        p.root_agent->show_solution(log, memetico::PREC, store->train, store->test);

    
    
    return EXIT_SUCCESS;
}
