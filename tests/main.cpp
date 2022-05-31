
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <stdlib.h>

#include <memetico/globals.h>

// Default Logic Globals
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

bool            memetico::do_debug = false;