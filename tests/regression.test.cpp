#include "doctest.h"

#include <memetico/globals.h>
#include <memetico/models/regression.h>

#include <string>
#include <sstream>

TEST_CASE("Regression<T>: <<") {

    size_t size = 6;

    // Set the data 
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    // f(x) = x1 + 2x3 + 3x5 + 20
    Regression<double>* m1 = new Regression<double>(size);
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(4, true);
    m1->set_active(5, true);
    m1->set_param(0, 1);
    m1->set_param(1, 12);
    m1->set_param(2, 2);
    m1->set_param(4, 3);
    m1->set_param(5, -20);

    stringstream ss;
    ss << *m1;
    REQUIRE( ss.str().compare("x1+2*x3+3*x5-20") == 0);

    delete m1;
   
}

TEST_CASE("Regression<T>: mutate(), used()") {

    size_t size = 6;
    
    // Set the data 
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    // f(x) = x1 + 2x3 + 3x5 + 20
    Regression<double>* m1 = new Regression<double>(size);
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(4, true);
    m1->set_active(5, true);
    m1->set_param(0, 1);
    m1->set_param(1, 12);
    m1->set_param(2, 2);
    m1->set_param(4, 3);
    m1->set_param(5, -20);

    for(size_t i = 0; i < 10; i++) {
        size_t last_used = m1->used();
        m1->mutate();
        size_t now_used = m1->used();
        REQUIRE( ((last_used+1 == now_used) || (last_used-1 == now_used)) );
    }

    delete m1;

}

TEST_CASE("Regression<T>: recombine()") {

    size_t size = 6;
    
    // Set the data 
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    Regression<double>* m = new Regression<double>(size);

    // f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double>* m1 = new Regression<double>(size);
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(4, true);
    m1->set_active(5, true);
    m1->set_param(0, 1);
    m1->set_param(1, 12);
    m1->set_param(2, 2);
    m1->set_param(4, 3);
    m1->set_param(5, -20);

    // f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double>* m2 = new Regression<double>(size);
    m2->set_active(0, false);
    m2->set_active(1, true);
    m2->set_active(2, false);
    m2->set_active(3, true);
    m2->set_active(4, true);
    m2->set_active(5, true);
    m2->set_param(0, 0);
    m2->set_param(1, -3);
    m2->set_param(2, 0);
    m2->set_param(3, 4);
    m2->set_param(4, 3);
    m2->set_param(5, -3);
    
    memetico::RANDINT = RandInt(42);
    // Creates random numbers 1, 3, 4 => Type union
    // f(x) = x1 + 2x3 + 3x5 - 20
    // f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect x5,                   c
    // Expect 3+3*(3-3)/3 = 3,      -20+4*(-3--20)/3 = -8/3 ~= -2.667
    m->recombine(m1, m2);

    REQUIRE( m->get_active(0) == false );
    REQUIRE( m->get_active(1) == false );
    REQUIRE( m->get_active(2) == false );
    REQUIRE( m->get_active(3) == false );
    REQUIRE( m->get_active(4) == true );
    REQUIRE( m->get_active(5) == true );
    REQUIRE( m->get_param(4) == 3 );
    REQUIRE( to_string(m->get_param(5)) == to_string((double)8/3) );

    memetico::RANDINT = RandInt(43);
    // Creates random numbers 0, 1, 2 => Type intersetion
    // m1 = f(x) = x1 + 2x3 + 3x5 - 20
    // m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect       x1 - 3x2 + 2x3 + 4x4 +  ...x5               - ...
    // Expect                               3+1*(3-3)/3 = 3     -20+2*(-3--20)/3 = -26/3
    m->recombine(m1, m2);

    REQUIRE( m->get_active(0) == true );
    REQUIRE( m->get_active(1) == true );
    REQUIRE( m->get_active(2) == true );
    REQUIRE( m->get_active(3) == true );
    REQUIRE( m->get_active(4) == true );
    REQUIRE( m->get_active(5) == true );
    REQUIRE( m->get_param(0) == 1 );
    REQUIRE( m->get_param(1) == -3 );
    REQUIRE( m->get_param(2) == 2 );
    REQUIRE( m->get_param(3) == 4 );
    REQUIRE( m->get_param(4) == 3 );
    REQUIRE( to_string(m->get_param(5)) == to_string((double)-26/3) );

    memetico::RANDINT = RandInt(44);
    // Creates random numbers 2 => Exclusive or
    // m1 = f(x) = x1 + 2x3 + 3x5 - 20
    // m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect       x1 - 3x2 + 2x3 + 4x4
    m->recombine(m1, m2);

    REQUIRE( m->get_active(0) == true );
    REQUIRE( m->get_active(1) == true );
    REQUIRE( m->get_active(2) == true );
    REQUIRE( m->get_active(3) == true );
    REQUIRE( m->get_active(4) == false );
    REQUIRE( m->get_active(5) == false );
    REQUIRE( m->get_param(0) == 1 );
    REQUIRE( m->get_param(1) == -3 );
    REQUIRE( m->get_param(2) == 2 );
    REQUIRE( m->get_param(3) == 4 );
    
    delete m;
    delete m1;
    delete m2;

}

TEST_CASE("Regression<T>: evaluate()") {

    size_t size = 6;
    
    // Set the data 
    if( Data::IVS.size() == 0)
        for(size_t i = 0; i < size-1; i++)
            Data::IVS.push_back("x"+to_string(i+1));
    
    // f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double>* m1 = new Regression<double>(size);
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(4, true);
    m1->set_active(5, true);
    m1->set_param(0, 1);
    m1->set_param(1, 12);
    m1->set_param(2, 2);
    m1->set_param(4, 3);
    m1->set_param(5, -20);
   
    // f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double>* m2 = new Regression<double>(size);
    m2->set_active(0, false);
    m2->set_active(1, true);
    m2->set_active(2, false);
    m2->set_active(3, true);
    m2->set_active(4, true);
    m2->set_active(5, true);
    m2->set_param(0, 0);
    m2->set_param(1, -3);
    m2->set_param(2, 0);
    m2->set_param(3, 4);
    m2->set_param(4, 3);
    m2->set_param(5, -3);

    double * data = new double[size];
    data[0] = 4;
    data[1] = 20;
    data[2] = -3;
    data[3] = 15;
    data[4] = -3;

    // x1 + 2*x3 + 3*x5 - 20 = 4 + 2*-2 + 3*-20
    REQUIRE( m1->evaluate(data) == -31 );

    // -3x2 + 4x4 + 3x5 - 3 = -12
    REQUIRE( m2->evaluate(data) == -12 );

    data[0] = 3.1;
    data[1] = -43.2;
    data[2] = 32.1;
    data[3] = 34.2;
    data[4] = -3.1;

    // x1 + 2*x3 +3*x5 - 20 = 38
    REQUIRE( m1->evaluate(data) == 38 );

    // -3*x2 + 4*x4 + 3*x5 - 3 = 254.1
    REQUIRE( to_string(m2->evaluate(data)) == to_string(254.1) );

    delete m1;
    delete m2;
    delete data;

}

TEST_CASE("Regression<T>: randomise()") {

    size_t size = 6;
    
    // Set the data 
    if( Data::IVS.size() == 0)
        for(size_t i = 0; i < size-1; i++)
            Data::IVS.push_back("x"+to_string(i+1));
    
    Regression<double>* m1 = new Regression<double>(size);
    m1->set_active(0, true);
    m1->set_active(1, false);
    m1->set_active(2, true);
    m1->set_active(4, false);
    m1->set_active(5, true);

    Regression<double>* clone = m1->clone();

    int min = 1;
    int max = 4;

    // For a number of times
    for(size_t i = 0; i < 20; i++ ) {

        // Randomise the fraction
        m1->randomise(min, max);

        // Check all parameters
        for(size_t param = 0; param < size; param++) {

            // Ensure no change occurs for inactive coeffs
            if( !m1->get_active(param) )
                REQUIRE( m1->get_param(param) == clone->get_param(param) );

            // Ensure change between the expected range for active coeffs
            else 
                REQUIRE((
                        (m1->get_param(param) >= min || m1->get_param(param) <= max) ||
                        (m1->get_param(param) >= max*-1 || m1->get_param(param) <= min*-1) 
                    ));

        }
    }
}


// Test Overflow
// Test Underflow
// Test partial sum

// Leaving at the moment 
// Test Show min
// Test Show 
