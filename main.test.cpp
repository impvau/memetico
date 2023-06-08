
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <stdlib.h>

#include <memetico/globals.h>
#include <memetico/population/pop.h>
#include <memetico/models/cont_frac_dd.h>

double          meme::PENALTY = 0;
size_t          meme::PREC = 18;
size_t          meme::NELDER_MEAD_STALE = 10;
size_t          meme::NELDER_MEAD_MOVES = 250;
double          meme::LOCAL_SEARCH_DATA_PCT = 1;
size_t          meme::LOCAL_SEARCH_RUNS = 4;
size_t          meme::LOCAL_SEARCH_INTERVAL = 1;
double          meme::MUTATE_RATE = 0.2;
size_t          meme::GEN = 0;
size_t          meme::GENERATIONS = 20;
size_t          meme::POCKET_DEPTH = 4;
long int        meme::MAX_TIME = 6000;
long int        meme::RUN_TIME = 0;
size_t          meme::DIVERSITY_COUNT = 5;
size_t          meme::STALE_RESET = 10;
size_t          meme::DEPTH = 4;

RandReal        meme::RANDREAL;
RandInt         meme::RANDINT;