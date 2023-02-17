
/**
 * @file
 * @author Haoyuan Sun <hsun2@caltech.edu>
 * @author Mohammad Haque <Mohammad.Haque@newcastle.edu.au>
 * @author Andrew Ciezak <andy@ium.solutions>
 * @version 1.0
 * @brief Entry point and harness for the memetic algorithm
 * 
 * @bug Need to add .devcontainer and .vscode configurations files to the list of files 
 * @bug Need to change all clone() functions to copy constructors
 * 
 */

// Local
#include <memetico/data.h>
#include <memetico/globals.h>
#include <memetico/rng.h>
#include <memetico/pop.h>
#include <memetico/agent.h>
#include <memetico/local_search.h>
#include <memetico/objective.h>
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac.h>
#include <memetico/models/cont_frac_dd.h>
// Std
#include <cstdlib>

using namespace std;

// Logic Globals
uint_fast32_t   memetico::SEED = 42;
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
long int        memetico::MAX_TIME = 10*60;


typedef Regression<double> DataType;
typedef ContinuedFractionDynamicDepth<DataType> ModelType;

size_t              memetico::POCKET_DEPTH = 1;
DynamicDepthType    memetico::DYNAMIC_DEPTH_TYPE = DynamicNone;
DiversityType       memetico::DIVERSITY_TYPE = DiversityNone;
size_t              memetico::DIVERSITY_COUNT = 3;

// File Globals
string          memetico::TRAIN_FILE = "sinx.csv";
string          memetico::TEST_FILE = "sinx.csv";
string          memetico::LOG_DIR = "out/";
ofstream        memetico::master_log;


// Technical Globals
size_t          memetico::PREC = 18;
bool            memetico::DEBUG = false;
size_t          memetico::RUN_TIME = 0;
bool            memetico::do_debug = false;
PrintType       memetico::FORMAT = memetico::PrintExcel;

// Global Heplers
RandReal        memetico::RANDREAL;
RandInt         memetico::RANDINT;

// Local Helpers
FILE*           std_out;
FILE*           std_err;
stringstream    args_out;

bool arg_exists(char** begin, char** end, string short_option, string long_option) {
    return find(begin, end, short_option) != end || find(begin, end, long_option) != end;
}

string arg_value(char** begin, char** end, string short_option, string long_option) {
        
    string ret;

    // Check for short string e.g. "-h" rather than "-help"
    char ** itr = find(begin, end, short_option);
    if (itr != end && ++itr != end)
        ret = string(*itr);
    
    // Check for long string e.g.  "-help"
    itr = find(begin, end, long_option);
    if (itr != end && ++itr != end)
        ret = string(*itr);

    // Build debug for VSC launch.json
    args_out << "    \"" << short_option << "\", \"" << ret << "\"," << endl;

    return ret;

}

void arg_seed(int argc, char * argv[]) {

    // Seed
    string arg_string = arg_value(argv, argv+argc, "-s", "--seed");
    if(arg_string != "")
        memetico::SEED = stol(arg_string);
    else  {
        RandInt seed_init = RandInt();
        memetico::SEED = seed_init(1, numeric_limits<int>::max());
    }
    memetico::RANDINT = RandInt(memetico::SEED);
    memetico::RANDREAL = RandReal(memetico::SEED);


}

void arg_log(int argc, char * argv[]) {

    // Log Directory
    string arg_string = arg_value(argv, argv+argc, string("-lt"), string("--log-to"));
    if(arg_string != "") {
        memetico::LOG_DIR = arg_string;
        if( memetico::LOG_DIR.back() != '/' )
            memetico::LOG_DIR += "/";

        // Change STD out if the log dir is specified. If not, use normal stdout / stderr
        std_out = freopen( (memetico::LOG_DIR + to_string(memetico::SEED) + ".Std.log").c_str(), "a", stdout);
        std_err = freopen( (memetico::LOG_DIR + to_string(memetico::SEED) + ".Err.log").c_str(), "a", stderr);

    }
}

void arg_local_search(int argc, char * argv[]) {
    // Local Search function
    string arg_string = arg_value(argv, argv+argc, "-ls", "--local-search");
    if( arg_string == "cnm" || arg_string == "")    Agent<ModelType>::LOCAL_SEARCH = local_search::custom_nelder_mead_redo<ModelType>;
    if( arg_string == "cnms" )                      Agent<ModelType>::LOCAL_SEARCH = local_search::custom_nelder_mead_alg4<ModelType>;  
}

void arg_objective(int argc, char * argv[]) {
    // Objective function
    string arg_string = arg_value(argv, argv+argc, "-o", "--objective");
    Agent<ModelType>::OBJECTIVE_NAME = arg_string;
    if( arg_string == "")                               Agent<ModelType>::OBJECTIVE_NAME = "mse";
    if( Agent<ModelType>::OBJECTIVE_NAME == "mse" )     Agent<ModelType>::OBJECTIVE = objective::mse<ModelType>;
}

void arg_dynamic_depth(int argc, char * argv[]) {
    // Dynamic Depth Type
    string arg_string = arg_value(argv, argv+argc, "-dd", "--dynamic-depth");
    if(arg_string == "none" || arg_string == "")    memetico::DYNAMIC_DEPTH_TYPE = DynamicNone;
    if(arg_string == "adp")                         memetico::DYNAMIC_DEPTH_TYPE = DynamicAdaptive;
    if(arg_string == "adp-mu")                      memetico::DYNAMIC_DEPTH_TYPE = DynamicAdaptiveMutation;
    if(arg_string == "rnd")                         memetico::DYNAMIC_DEPTH_TYPE = DynamicRandom;
}

void arg_diversity(int argc, char * argv[]) {
    // Diversity Type
    string arg_string = arg_value(argv, argv+argc, "-dm", "--diversity-method");
    if(arg_string == "none" || arg_string == "")    memetico::DIVERSITY_TYPE = DiversityNone;
    if(arg_string == "every")                       memetico::DIVERSITY_TYPE = DiversityEvery;
    if(arg_string == "stale")                       memetico::DIVERSITY_TYPE = DiversityStale;
    if(arg_string == "stale-ext")                   memetico::DIVERSITY_TYPE = DiversityStaleExtended;
}

void arg_help(int argc, char * argv[]) {
     
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

                        -dc --diversity-count           Number of solutions to replace in 'stale' and 'every' --diversity-method's

                        -dd --dynamic-depth             Method of dynamic depth
                                                            none: hardset depth for the solution, -f
                                                            adp: randomise new solutions with pocket depth +- 1
                                                            adp-mu: as per adp, but during mutation re-randomise depth +- 1
                                                            rnd: randomise new solutions uniform at random between 0 and -f inclusive
                                                            
                        -dm --diversity-method          Method to replace similar solutions each generation
                                                        Compares all pocket and current solutions based on differences in MSE scores
                                                        Process performs a full local search after renewing
                                                            none (default): no replacement method
                                                            every: Every generation replace the --diversity-count most similar solutions
                                                            stale: Replace --diversity-count most similar solutions on every --stale 
                                                            stale-ext: As per stale but every two consecutive --stale's, 50% of the solutions are replaced                                                        Defaults to 0

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

                        -mt --max-time                  Number of seconds after which the best solution is returned

                        -s --seed <integer>             Reproduction seed
                                                        Defaults to random integer between 1, numerical_limit<int>::max()

                        -st --stale                     Stale count that triggers renew of the roots current solution

                        

                        -T --Test <filepath>            Test data file
                                                        Defaults to training filepath from -t

                        
                                                        

            )";
    }
}
    
void load_args(int argc, char * argv[]) {
   
    args_out << "==================" << endl;
    args_out << "Arguments" << endl;
    args_out << "==================" << endl;
    args_out << "Command: " << argv[0] << endl;
    args_out << "\"args\": [" << endl;
    
    string arg_string;

    arg_help(argc,argv);

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

    // Data split percentage
    arg_string = arg_value(argv, argv+argc, "-sd", "--split-data");
    if(arg_string != "")    Data::SPLIT = stod(arg_string);

    // Local Search Data
    arg_string = arg_value(argv, argv+argc, "-ld", "--local-data");
    if(arg_string != "")    memetico::LOCAL_SEARCH_DATA_PCT = stod(arg_string);

    // Stale Count
    arg_string = arg_value(argv, argv+argc, "-st", "--stale");
    if(arg_string != "")        memetico::STALE_RESET = stoi(arg_string);
    
    // Max time
    arg_string = arg_value(argv, argv+argc, "-mt", "--max-time");
    if(arg_string != "")        memetico::MAX_TIME = stoi(arg_string);

    arg_seed(argc, argv);
    arg_log(argc, argv);
    arg_local_search(argc, argv);
    arg_objective(argc, argv);
    
    // CFR Specific

    // Depth
    arg_string = arg_value(argv, argv+argc, "-f", "--fracdepth");
    if(arg_string != "")    ContinuedFraction<DataType>::DEPTH = stoi(arg_string);
    
    // Diversity Count
    arg_string = arg_value(argv, argv+argc, "-dc", "--diversity-count");
    if(arg_string != "")        memetico::DIVERSITY_COUNT = stoi(arg_string);

    arg_dynamic_depth(argc,argv);
    arg_diversity(argc,argv);

    // Master log
    memetico::master_log = ofstream(memetico::LOG_DIR+to_string(memetico::SEED)+".Master.log");
       
    // Output arguments in VSCode launch.json format
    args_out << "]," << endl;
    cout << args_out.str();

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

    // Write Run.log
    cout << " Writing " << memetico::LOG_DIR+to_string(memetico::SEED)+".Run.log" << endl;
    ofstream log(memetico::LOG_DIR+to_string(memetico::SEED)+".Run.log");
    if (log.is_open()) {
        log << setprecision(memetico::PREC);
        p.root_agent->show_solution(log, memetico::PREC, store->train, store->test);
    }

    // Write Training results
    ofstream train_log(memetico::LOG_DIR+to_string(memetico::SEED)+".TrainResults.csv");
    if (train_log.is_open()) {

        train_log << setprecision(memetico::PREC) << "y" << endl;
        for(size_t i = 0; i < store->TRAIN_COUNT; i++)
            train_log << p.root_agent->get_pocket()->evaluate(store->train->variables[i]) << endl;

    }

    // Write Testing results
    ofstream test_log(memetico::LOG_DIR+to_string(memetico::SEED)+".TestResults.csv");
    if (test_log.is_open()) {

        test_log << setprecision(memetico::PREC) << "y" << endl;
        for(size_t i = 0; i < store->TEST_COUNT; i++)
            test_log << p.root_agent->get_pocket()->evaluate(store->test->variables[i]) << endl;

    }

    return EXIT_SUCCESS;
}
