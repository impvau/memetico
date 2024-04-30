#include "doctest.h"

//#include <memetico/globals.h>
//#include <memetico/data.h>
#include <memetico/model_base/model.h>

#include <string>
#include <sstream>

TEST_CASE("Model: Model(), get_penalty, get_error, get_count, get_fitness ") {

    Model m = Model();
    REQUIRE(m.get_penalty() == 1);
    REQUIRE(m.get_error() == numeric_limits<double>::max());
    REQUIRE(m.get_fitness() == numeric_limits<double>::max());

}

TEST_CASE("Model: set_error, set_penalty, set_fitness") {

    Model m = Model();
    m.set_error(5.1);
    m.set_penalty(1.5);
    m.set_fitness(2.5);

    REQUIRE(m.get_penalty() == 1.5);
    REQUIRE(m.get_error() == 5.1);
    REQUIRE(m.get_fitness() == 2.5);

}

TEST_CASE("Model: Model( Model )") {

    Model m1 = Model();
    m1.set_error(5.1);
    m1.set_penalty(1.5);
    m1.set_fitness(2.5);

    Model m2 = Model(m1);
    REQUIRE(m2.get_penalty() == 1.5);
    REQUIRE(m2.get_error() == 5.1);
    REQUIRE(m2.get_fitness() == 2.5);

}