#include "doctest.h"


#include <memetico/globals.h>
#include <memetico/data.h>
#include <memetico/model_base/model.h>

#include <string>
#include <sstream>

TEST_CASE("Model: Construct, allocate()") {

    size_t size = 5;
    Model<double>* m = new Model<double>(size);

    // Check the default values
    REQUIRE(m->get_penalty() == 1);
    REQUIRE(m->get_error() == numeric_limits<double>::max());
    REQUIRE(m->get_fitness() == numeric_limits<double>::max());
    REQUIRE(m->get_count() == size);

    // Check the allocated space
    for(size_t i = 0; i < size; i++) {
        REQUIRE( m->get_param(i) == 0 );
        REQUIRE( m->get_active(i) == false );
    }

    delete m;

}

TEST_CASE("Model: Construct empty model") {

    // We can not check much here, just that it does not create an error
    // If we look at the memory at zero, no error will occur, we will just get a random memory cell
    // We can atleast confirm the constructor ran

    Model<double>* m = new Model<double>(0);

    // Check the default values
    REQUIRE(m->get_penalty() == 1);
    REQUIRE(m->get_error() == numeric_limits<double>::max());
    REQUIRE(m->get_fitness() == numeric_limits<double>::max());
    REQUIRE(m->get_count() == 0);

    delete m;

}

TEST_CASE("Model: clone()") {

    size_t size = 5;
    
    Model<double>* m1 = new Model<double>(size);
    m1->set_error(5.4);
    m1->set_fitness(4.2);
    m1->set_penalty(1.3);
    /*
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(3, false);
    m1->set_active(4, true);
    m1->set_param(0, 4);
    m1->set_param(1, 2);
    m1->set_param(2, 6);
    m1->set_param(3, 8);
    m1->set_param(4, 122);
    */

    Model<double>* m2 = new Model<double>(size);
    m2->set_error(4.5);
    m2->set_fitness(2.4);
    m2->set_penalty(3.1);
    /*
    m2->set_active(0, false);
    m2->set_active(1, true);
    m2->set_active(2, false);
    m2->set_active(3, true);
    m2->set_active(4, false);
    m2->set_param(0, 12);
    m2->set_param(1, 122);
    m2->set_param(2, 57);
    m2->set_param(3, 443);
    m2->set_param(4, 232);
    */
   
    Model<double>* clone = m1->clone();
    
    REQUIRE( m1->get_penalty() == clone->get_penalty() );
    REQUIRE( m1->get_error() == clone->get_error() );
    REQUIRE( m1->get_fitness() == clone->get_fitness() );
    REQUIRE( m1->get_count() == clone->get_count() );
    /*
    REQUIRE( m1->get_active(0) == clone->get_active(0) );
    REQUIRE( m1->get_active(1) == clone->get_active(1) );
    REQUIRE( m1->get_active(2) == clone->get_active(2) );
    REQUIRE( m1->get_active(3) == clone->get_active(3) );
    REQUIRE( m1->get_active(4) == clone->get_active(4) );
    REQUIRE( m1->get_param(0) == clone->get_param(0) );
    REQUIRE( m1->get_param(1) == clone->get_param(1) );
    REQUIRE( m1->get_param(2) == clone->get_param(2) );
    REQUIRE( m1->get_param(3) == clone->get_param(3) );
    REQUIRE( m1->get_param(4) == clone->get_param(4) );
    */
    
    delete clone;

    clone = m2->clone();

    REQUIRE( m2->get_penalty() == clone->get_penalty() );
    REQUIRE( m2->get_error() == clone->get_error() );
    REQUIRE( m2->get_fitness() == clone->get_fitness() );
    REQUIRE( m2->get_count() == clone->get_count() );
    /*
    REQUIRE( m2->get_active(0) == clone->get_active(0) );
    REQUIRE( m2->get_active(1) == clone->get_active(1) );
    REQUIRE( m2->get_active(2) == clone->get_active(2) );
    REQUIRE( m2->get_active(3) == clone->get_active(3) );
    REQUIRE( m2->get_active(4) == clone->get_active(4) );
    REQUIRE( m2->get_param(0) == clone->get_param(0) );
    REQUIRE( m2->get_param(1) == clone->get_param(1) );
    REQUIRE( m2->get_param(2) == clone->get_param(2) );
    REQUIRE( m2->get_param(3) == clone->get_param(3) );
    REQUIRE( m2->get_param(4) == clone->get_param(4) );
    */

    delete clone;
    delete m1;
    delete m2;

}

/*
TEST_CASE("Model: << ") {

    size_t size = 5;

    for(size_t i = 0; i < size; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    // f(x) = x1 + 2x3 + 3x5
    Model<double>* m1 = new Model<double>(size);
    m1->set_active(0, true);
    m1->set_active(2, true);
    m1->set_active(4, true);
    m1->set_param(0, 1);
    m1->set_param(2, 2);
    m1->set_param(4, 3);

    // f(x) = 4x1 + 5x2 + 6x3
    Model<double>* m2 = new Model<double>(size);
    m2->set_active(0, true);
    m2->set_active(1, true);
    m2->set_active(2, true);
    m2->set_active(4, true);
    m2->set_param(0, 4);
    m2->set_param(1, 5);
    m2->set_param(2, 6);
    m2->set_param(4, 7);

    stringstream ss;
    ss << *m1;
    REQUIRE( ss.str().compare("1 (x1) 2 (x3) 3 (x5) ") == 0);

    ss.str("");
    ss << *m2;
    REQUIRE( ss.str().compare("4 (x1) 5 (x2) 6 (x3) 7 (x5) ") == 0);

    delete m1;
    delete m2;
}*/

// We technically dont test "show_min", but we want to remove it
// We technically dont test "~Model"
// We could also test double type