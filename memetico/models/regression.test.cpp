#include "doctest.h"
#include <memetico/models/regression.h>
#include <string>
#include <sstream>
#include <memetico/helpers/rng.h>

void setup_regression_ivs(size_t size) {

    // Setup IVs in Regression
    MemeticModel<double>::IVS.clear();
    for(size_t i = 0; i < size-1; i++)
        Regression<double>::IVS.push_back("x"+to_string(i+1));

}

TEST_CASE("Regression: Regression<T>(), Regression<T>(r1) ") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // 1. Default Regression object has a default term object size in length
    Regression<double> m1 = Regression<double>(size);
    m1.set_fitness(5);
    m1.set_penalty(2);
    m1.set_error(16);
    AdditiveTerm<double> term = AdditiveTerm<double>(size);
    REQUIRE( m1.get_term() == term );

    // 2. Construct from existing regressors
    Regression<double> m2 = Regression<double>(m1);
    REQUIRE( m1.get_term() == term );
    REQUIRE( m1.get_fitness() == m2.get_fitness() );
    REQUIRE( m1.get_error() == m2.get_error() );
    REQUIRE( m1.get_penalty() == m2.get_penalty() );

}

TEST_CASE("Regression: set_active, set_value, get_active, get_value") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // 1. Default Regression object has a default term object size in length
    Regression<double> m1 = Regression<double>(size);
    m1.set_active(0, false);
    m1.set_active(1, true);
    m1.set_active(2, false);
    m1.set_active(3, true);
    m1.set_active(4, false);
    double d0 = 5.2;
    double d1 = 3.5;
    double d2 = 0;
    double d3 = -2.2;
    double d4 = -13;
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    // 1.1 Ensure all values match new values
    REQUIRE(m1.get_value(0) == d0);
    REQUIRE(m1.get_value(1) == d1);
    REQUIRE(m1.get_value(2) == d2);
    REQUIRE(m1.get_value(3) == d3);
    REQUIRE(m1.get_value(4) == d4);
    // 1.2. Check chaning the original element doesn't change object
    d0 = 7.8;
    REQUIRE(m1.get_value(0) != d0);

}

TEST_CASE("Regression: set_term, get_term, ==") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // 1. Compare empty regression objects
    Regression<double> m1 = Regression<double>(size);
    Regression<double> m2 = Regression<double>(size);
    REQUIRE(m1 == m2);
    Regression<double> m3 = Regression<double>(size+1);
    REQUIRE(!(m1 == m3));

    // 2. Create term, assign to Regression and check it is the same as building with set_active, set_value 
    // Setup Term
    AdditiveTerm<double> o1 = AdditiveTerm<double>(size);
    // Update all values
    double d0 = 5.2;
    double d1 = 3.5;
    double d2 = 0;
    double d3 = -2.2;
    double d4 = -13;
    o1.set_value(0, d0);
    o1.set_value(1, d1);
    o1.set_value(2, d2);
    o1.set_value(3, d3);
    o1.set_value(4, d4);
    // 2.1 Set m4 manually, set term in m5 and ensure they are equal
    Regression<double> m4 = Regression<double>(size);
    m4.set_value(0, d0);
    m4.set_value(1, d1);
    m4.set_value(2, d2);
    m4.set_value(3, d3);
    m4.set_value(4, d4);
    Regression<double> m5 = Regression<double>(size);
    m5.set_term(o1);
    REQUIRE( m4 == m5 );
    REQUIRE( m4.get_term() == m5.get_term() );
    // 2.2 Change the AdditiveTerm<T> passed in and ensure the object is not changed 
    o1.set_value(0, d4);
    REQUIRE( m4.get_term() == m5.get_term() );
    // 2.3 Change the reference returned by and ensure the underlying object is changed
    m5.get_term().set_value(0, d4);
    REQUIRE( !(m4.get_term() == m5.get_term()) );
    REQUIRE( m5.get_value(0) == d4 );

}

TEST_CASE("Regression: get_count_active, get_active_positions()") {

    size_t size = 6;
    setup_regression_ivs(size);

    // 1. Test increasing number of active variables
    Regression<double> r1 = Regression<double>(5);
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
    // 1.1 Default of 0 active
    REQUIRE(r1.get_count_active() == 0);
    REQUIRE(r1.get_active_positions().size() == 0);
    Element<double> d0 = Element<double>(false, 5.2);
    Element<double> d1 = Element<double>(true, 3.5);
    Element<double> d2 = Element<double>(false, 0);
    Element<double> d3 = Element<double>(true, -2.2);
    Element<double> d4 = Element<double>(false, -13);
    // 1.2 After setting 1 active
    o1.set_elem(0, d0);
    o1.set_elem(1, d1);
    r1.set_term(o1);
    REQUIRE(r1.get_count_active() == 1);
    REQUIRE(r1.get_active_positions().size() == 1);
    REQUIRE(r1.get_active_positions()[0] == 1);
    // 1.3 After setting 2 active
    o1.set_elem(2, d2);
    o1.set_elem(3, d3);
    o1.set_elem(4, d4);
    r1.set_term(o1);
    REQUIRE(r1.get_count_active() == 2);
    REQUIRE(r1.get_active_positions().size() == 2);
    REQUIRE(r1.get_active_positions()[0] == 1);
    REQUIRE(r1.get_active_positions()[1] == 3);
    // 1.3 After setting 3 active
    o1.set_elem(0, d3);
    r1.set_term(o1);
    REQUIRE(r1.get_count_active() == 3);
    REQUIRE(r1.get_active_positions().size() == 3);
    REQUIRE(r1.get_active_positions()[0] == 0);
    REQUIRE(r1.get_active_positions()[1] == 1);
    REQUIRE(r1.get_active_positions()[2] == 3);
    // 1.4 After setting 4 active
    o1.set_elem(2, d3);
    r1.set_term(o1);
    REQUIRE(r1.get_count_active() == 4);
    REQUIRE(r1.get_active_positions().size() == 4);
    REQUIRE(r1.get_active_positions()[0] == 0);
    REQUIRE(r1.get_active_positions()[1] == 1);
    REQUIRE(r1.get_active_positions()[2] == 2);
    REQUIRE(r1.get_active_positions()[3] == 3);
    // 1.5 After setting 5 active
    o1.set_elem(4, d3);
    r1.set_term(o1);
    REQUIRE(r1.get_count_active() == 5);
    REQUIRE(r1.get_active_positions().size() == 5);
    REQUIRE(r1.get_active_positions()[0] == 0);
    REQUIRE(r1.get_active_positions()[1] == 1);
    REQUIRE(r1.get_active_positions()[2] == 2);
    REQUIRE(r1.get_active_positions()[3] == 3);
    REQUIRE(r1.get_active_positions()[4] == 4);

}

TEST_CASE("Regression: ==") {

    size_t size = 6;
    setup_regression_ivs(size);

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = Regression<double>(size);
    double d0 = 1;
    double d1 = 12;
    double d2 = 2;
    double d3 = -7;
    double d4 = 3;
    double d5 = -20;
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    m1.set_value(5, d5);

    // Setup m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double> m2 = Regression<double>(size);
    d0 = 0;
    d1 = -3;
    d2 = 0;
    d3 = 4;
    d4 = 3;
    d5 = -3;
    m2.set_active(0, false);
    m2.set_active(1, true);
    m2.set_active(2, false);
    m2.set_active(3, true);
    m2.set_active(4, true);
    m2.set_active(5, true);
    m2.set_value(0, d0);
    m2.set_value(1, d1);
    m2.set_value(2, d2);
    m2.set_value(3, d3);
    m2.set_value(4, d4);
    m2.set_value(5, d5);

    // Setup m3 = m1 = f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m3 = Regression<double>(size);
    d0 = 1;
    d1 = 12;
    d2 = 2;
    d3 = -7;
    d4 = 3;
    d5 = -20;
    m3.set_active(0, true);
    m3.set_active(1, false);
    m3.set_active(2, true);
    m3.set_active(3, false);
    m3.set_active(4, true);
    m3.set_active(5, true);
    m3.set_value(0, d0);
    m3.set_value(1, d1);
    m3.set_value(2, d2);
    m3.set_value(3, d3);
    m3.set_value(4, d4);
    m3.set_value(5, d5);

    // 1. Compare two same objects
    REQUIRE(m1 == m1);
    REQUIRE(m2 == m2);
    // 2. Check two different objects in different directions
    REQUIRE(!(m1 == m2));
    REQUIRE(!(m2 == m1));
    // 3. Check new object exactly like m1 is identical (i.e. we are not checking reference between the to)
    REQUIRE(m1 == m3);
    REQUIRE(m3 == m1);
    // 4. Check empty object
    Regression<double> m4 = Regression<double>();
    Regression<double> m5 = Regression<double>();
    REQUIRE(m4 == m5);
    REQUIRE(m5 == m4);
    // 5. Check defaul object
    Regression<double> m6 = Regression<double>(size);
    Regression<double> m7 = Regression<double>(size);
    REQUIRE(m6 == m7);
    REQUIRE(m7 == m6);

}

TEST_CASE("Regression: randomise()") {

    size_t size = 6;
    setup_regression_ivs(size);

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = Regression<double>(size);

    // 1. Randomise each individual varaible 
    // 1.1 pos 3
    size_t pos = 3;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    // 1.2 pos 2
    pos = 2;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    // 1.3 pos 1
    pos = 1;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    // 1.4 pos 0
    pos = 0;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    // 1.5 pos 4
    pos = 4;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    // 1.5 pos 5
    pos = 5;
    REQUIRE(m1.get_value(pos) == 0);
    m1.set_active(pos, true);
    m1.randomise(-5, 5, pos);
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);

    // 2. Randomise the entire object
    m1 = Regression<double>(size);
    m1.set_active(0, true);
    m1.set_active(1, true);
    m1.set_active(2, true);
    m1.set_active(3, true);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.randomise(-5,5);
    pos = 0;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    pos = 1;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    pos = 2;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    pos = 3;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    pos = 4;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);
    pos = 5;
    REQUIRE(m1.get_value(pos) != 0);
    REQUIRE(m1.get_value(pos) >= -5);
    REQUIRE(m1.get_value(pos) <= 5);

    // 3. Randomise a large number of objects and ensure they are reasonably distributed
    // We expect 90 samples for each of the 11 categories to be completely uniformly distributed
    // We say 70 due to the random chance, so we have around 77%
    size_t neg5 = 0;
    size_t neg4 = 0;
    size_t neg3 = 0;
    size_t neg2 = 0;
    size_t neg1 = 0;
    size_t zero = 0;
    size_t pos1 = 0;
    size_t pos2 = 0;
    size_t pos3 = 0;
    size_t pos4 = 0;
    size_t pos5 = 0;
    m1 = Regression<double>(size);
    m1.set_active(0, true);
    pos = 0;
    for(size_t i = 0; i < 1000; i++) {
        m1.randomise(-5,5,pos);
        double val = m1.get_value(pos);
        if( val == -5 ) neg5++;
        if( val == -4 ) neg4++;
        if( val == -3 ) neg3++;
        if( val == -2 ) neg2++;
        if( val == -1 ) neg1++;
        if( val == 0 ) zero++;
        if( val == 1 ) pos1++;
        if( val == 2 ) pos2++;
        if( val == 3 ) pos3++;
        if( val == 4 ) pos4++;
        if( val == 5 ) pos5++;
    }
    REQUIRE(neg5 > 70);
    REQUIRE(neg4 > 70);
    REQUIRE(neg3 > 70);
    REQUIRE(neg2 > 70);
    REQUIRE(neg1 > 70);
    REQUIRE(zero > 70);
    REQUIRE(pos1 > 70);
    REQUIRE(pos2 > 70);
    REQUIRE(pos3 > 70);
    REQUIRE(pos4 > 70);
    REQUIRE(pos5 > 70);
}

TEST_CASE("Regression: recombine()") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = Regression<double>(size);
    double d0 = 1;
    double d1 = 12;
    double d2 = 2;
    double d3 = -7;
    double d4 = 3;
    double d5 = -20;
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    m1.set_value(5, d5);

    // f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double> m2 = Regression<double>(size);
    d0 = 0;
    d1 = -3;
    d2 = 0;
    d3 = 4;
    d4 = 3;
    d5 = -3;
    m2.set_active(0, false);
    m2.set_active(1, true);
    m2.set_active(2, false);
    m2.set_active(3, true);
    m2.set_active(4, true);
    m2.set_active(5, true);
    m2.set_value(0, d0);
    m2.set_value(1, d1);
    m2.set_value(2, d2);
    m2.set_value(3, d3);
    m2.set_value(4, d4);
    m2.set_value(5, d5);
    
    // 1. Recombine using union
    Regression<double> m = Regression<double>(size);
    // Creates random numbers 1, 3, 4, so 1 = Type union, {3,4} are consts
    // f(x) = x1 + 2x3 + 3x5 - 20
    // f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect x5,                   c
    // Expect 3+3*(3-3)/3 = 3,      -20+4*(-3--20)/3 = -8/3 ~= -2.667
    m.recombine(&m1, &m2);
    REQUIRE( m.get_active(0) == false );
    REQUIRE( m.get_active(1) == false );
    REQUIRE( m.get_active(2) == false );
    REQUIRE( m.get_active(3) == false );
    REQUIRE( m.get_active(4) == true );
    REQUIRE( m.get_active(5) == true );
    REQUIRE( m.get_value(4) == 3 );
    REQUIRE( to_string(m.get_value(5)) == to_string((double)8/3) );
    
    // 2. Recombine using intersection
    ri = RandInt(43);
    // Creates random numbers 0, 1, 2. So 0 = Type intersetion, {1,2} are constants
    // m1 = f(x) = x1 + 2x3 + 3x5 - 20
    // m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect       x1 - 3x2 + 2x3 + 4x4 +  ...x5               - ...
    // Expect                               3+1*(3-3)/3 = 3     -20+2*(-3--20)/3 = -26/3
    m.recombine(&m1, &m2);
    REQUIRE( m.get_active(0) == true );
    REQUIRE( m.get_active(1) == true );
    REQUIRE( m.get_active(2) == true );
    REQUIRE( m.get_active(3) == true );
    REQUIRE( m.get_active(4) == true );
    REQUIRE( m.get_active(5) == true );
    REQUIRE( m.get_value(0) == 1 );
    REQUIRE( m.get_value(1) == -3 );
    REQUIRE( m.get_value(2) == 2 );
    REQUIRE( m.get_value(3) == 4 );
    REQUIRE( m.get_value(4) == 3 );
    REQUIRE( to_string(m.get_value(5)) == to_string((double)-26/3) );

    // 2. Recombine using XOR
    ri = RandInt(44);
    // Creates random numbers 2. So Type XOR
    // m1 = f(x) = x1 + 2x3 + 3x5 - 20
    // m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    // Expect       x1 - 3x2 + 2x3 + 4x4
    m.recombine(&m1, &m2);
    REQUIRE( m.get_active(0) == true );
    REQUIRE( m.get_active(1) == true );
    REQUIRE( m.get_active(2) == true );
    REQUIRE( m.get_active(3) == true );
    REQUIRE( m.get_active(4) == false );
    REQUIRE( m.get_active(5) == false );
    REQUIRE( m.get_value(0) == 1 );
    REQUIRE( m.get_value(1) == -3 );
    REQUIRE( m.get_value(2) == 2 );
    REQUIRE( m.get_value(3) == 4 );

}

// Test Overflow 3. below
// Test Underflow 4. below

TEST_CASE("Regression: evaluate()") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = Regression<double>(size);
    double d0 = 1;
    double d1 = 12;
    double d2 = 2;
    double d3 = -7;
    double d4 = 3;
    double d5 = -20;
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    m1.set_value(5, d5);
   
    // f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double> m2 = Regression<double>(size);
    d0 = 0;
    d1 = -3;
    d2 = 0;
    d3 = 4;
    d4 = 3;
    d5 = -3;
    m2.set_active(0, false);
    m2.set_active(1, true);
    m2.set_active(2, false);
    m2.set_active(3, true);
    m2.set_active(4, true);
    m2.set_active(5, true);
    m2.set_value(0, d0);
    m2.set_value(1, d1);
    m2.set_value(2, d2);
    m2.set_value(3, d3);
    m2.set_value(4, d4);
    m2.set_value(5, d5);

    // 1 Test evaluation of integer values
    vector<double> values;
    values.push_back(4);
    values.push_back(20);
    values.push_back(-3);
    values.push_back(15);
    values.push_back(-3);
    
    // 1.1 First at m1 = x1 + 2*x3 + 3*x5 - 20 
    // = 4 + 2*-2 + 3*-20
    REQUIRE( m1.evaluate(values) == -31 );

    // 1.2 Second at m2 = -3x2 + 4x4 + 3x5 - 3
    // = -12
    REQUIRE( m2.evaluate(values) == -12 );

    // 2. Evaluate for floats now
    values.clear();
    values.push_back(3.1);
    values.push_back(-43.2);
    values.push_back(-32.1);
    values.push_back(34.2);
    values.push_back(-3.1);

    // 2.1 First at m1 = x1 + 2*x3 +3*x5 - 20 
    // = 3.1 + 2*-32.1+3*-3.1-20
    // = -90.4
    REQUIRE( m1.evaluate(values) == -90.4 );

    // 2.2 Second at m1 = -3*x2 + 4*x4 + 3*x5 - 3 
    // = 254.1
    REQUIRE( to_string(m2.evaluate(values)) == to_string(254.1) );

    // 3. Test Overflow operation
    values.clear();
    values.push_back(numeric_limits<double>::max());
    values.push_back(0);
    values.push_back(0);
    values.push_back(0);
    values.push_back(0);

    // 3. Overflow -- returns Inf not exception like expected
    // m1 = f(x) = x1 + 2x3 + 3x5 - 20
    //d5 = numeric_limits<double>::max();
    //m1.set_value(5, d5);
    //cout << m1 << endl;
    //double ret = m1.evaluate(values);
    //REQUIRE( m1.evaluate(values) == numeric_limits<double>::max() );

    // 4. Underflow -- untested due to 3. need to test safe_ops first
}

TEST_CASE("Regression: mutate()") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // 1. Continually mutate and ensure 
    Regression<double> m = Regression<double>();
    Regression<double> m1 = Regression<double>(size);
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    size_t last_active = m1.get_count_active();
    for( size_t i = 0; i < 50; i++) {
        m1.mutate(m);
        REQUIRE( (m1.get_count_active() == last_active-1 || m1.get_count_active() == last_active+1) );
        last_active = m1.get_count_active();
    }
}

// Test for 0 value
// Test for 1, -1 explicitly 

TEST_CASE("Regression: <<") {

    size_t size = 6;
    setup_regression_ivs(size);

    // 1. Empty regression
    Regression<double> m = Regression<double>(size);
    stringstream ss;
    ss << m;
    REQUIRE(ss.str() == "");

    // 2.1 m1 = f(x) = x1 + 2x3 + 3x5 - 20
    //     m1 = f(x) = B2+2*D2+3*F2-20
    Regression<double> m1 = Regression<double>(size);
    double d0 = 1;
    double d1 = 12;
    double d2 = 2;
    double d3 = -7;
    double d4 = 3;
    double d5 = -20;
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    m1.set_value(5, d5);
    stringstream ss1;
    ss1 << m1;
    REQUIRE(ss1.str() == "B2+2*D2+3*F2-20");

    // 2.2 m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    //     m2 =        -3*C2+4*E2+3*F2-3
    Regression<double> m2 = Regression<double>(size);
    d0 = 0;
    d1 = -3;
    d2 = 0;
    d3 = 4;
    d4 = 3;
    d5 = -3;
    m2.set_active(0, false);
    m2.set_active(1, true);
    m2.set_active(2, false);
    m2.set_active(3, true);
    m2.set_active(4, true);
    m2.set_active(5, true);
    m2.set_value(0, d0);
    m2.set_value(1, d1);
    m2.set_value(2, d2);
    m2.set_value(3, d3);
    m2.set_value(4, d4);
    m2.set_value(5, d5);
    stringstream ss2;
    ss2 << m2;
    REQUIRE(ss2.str() == "-3*C2+4*E2+3*F2-3");

    // 2.3 -1, 1, 0 values m3 = -B2-F2-1
    Regression<double> m3 = Regression<double>(size);
    d0 = -1;
    d1 = 0;
    d2 = 0;
    d3 = 1;
    d4 = -1;
    d5 = -1;
    m3.set_active(0, true);
    m3.set_active(1, false);
    m3.set_active(2, true);
    m3.set_active(3, false);
    m3.set_active(4, true);
    m3.set_active(5, true);
    m3.set_value(0, d0);
    m3.set_value(1, d1);
    m3.set_value(2, d2);
    m3.set_value(3, d3);
    m3.set_value(4, d4);
    m3.set_value(5, d5);
    stringstream ss3;
    ss3 << m3;
    REQUIRE(ss3.str() == "-B2-F2-1");

}

// Test TreeNode
TEST_CASE("Regression: coeff_node") {

    size_t size = 6;
    setup_regression_ivs(size);

    Regression<double> m1 = Regression<double>(size);

    // Node 1

    TreeNode * node = new TreeNode();
    m1.coeff_node(node, 2, 0);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node of 2
    REQUIRE(node->left->node.constant == 2);
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index 0
    REQUIRE(node->right->node.variable == 0);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;
    
    // Node 2
    node = new TreeNode();
    m1.coeff_node(node, -1, 1);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node
    REQUIRE(node->left->node.constant == -1);
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index
    REQUIRE(node->right->node.variable == 1);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;

    // Node 3
    node = new TreeNode();
    m1.coeff_node(node, 777, 2);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node
    REQUIRE(node->left->node.constant == 777);
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index
    REQUIRE(node->right->node.variable == 2);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;

    // Node 4
    node = new TreeNode();
    m1.coeff_node(node, -777, 3);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node
    REQUIRE(node->left->node.constant == -777);
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index
    REQUIRE(node->right->node.variable == 3);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;

    // Node 5
    node = new TreeNode();
    m1.coeff_node(node, 0, 4);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node
    REQUIRE(node->left->node.constant == 0);
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index
    REQUIRE(node->right->node.variable == 4);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;
    
    // Node 6
    node = new TreeNode();
    m1.coeff_node(node, double(52.5), 5);
    // Multiply node that has two nodes
    REQUIRE(node->node.function == Function::MUL);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    // Constant left node
    REQUIRE(double(node->left->node.constant) == double(52.5));
    REQUIRE(node->left->node.node_type == NodeType::CONST);
    // Variable right node for index
    REQUIRE(node->right->node.variable == 5);
    REQUIRE(node->right->node.node_type == NodeType::VAR);
    delete node;

}

// Test TreeNode
TEST_CASE("Regression: get_node") {


    // Tests
    // 1. Only the constant
    // 2. Only 1 coeff
    // 3. Even number of coeffs
    // 4. Even number of coeffs with constant
    // 5. Odd number of coeffs
    // 5. Odd number of with constant

    size_t size = 6;
    setup_regression_ivs(size);
    
    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = Regression<double>(size);
    double d0 = 1;
    double d1 = 12;
    double d2 = 2;
    double d3 = -7;
    double d4 = 3;
    double d5 = -20;
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    m1.set_value(2, d2);
    m1.set_value(3, d3);
    m1.set_value(4, d4);
    m1.set_value(5, d5);

    // 1. Only the constant
    m1.set_active(0, false);
    m1.set_active(1, false);
    m1.set_active(2, false);
    m1.set_active(3, false);
    m1.set_active(4, false);
    m1.set_active(5, true);
    //
    TreeNode * n1 = new TreeNode();
    m1.get_node(n1);
    // Root Node
    REQUIRE(n1->node.node_type == NodeType::CONST);
    REQUIRE(n1->node.constant == -20);
    delete n1;
    
    // 2. Only 1 coeff
    m1.set_active(0, false);
    m1.set_active(1, false);
    m1.set_active(2, false);
    m1.set_active(3, true);
    m1.set_active(4, false);
    m1.set_active(5, false);
    TreeNode* n2 = new TreeNode();
    m1.get_node(n2);
    //
    //       *
    //     /   \
    //   -7     3
    //
    // Root Node
    REQUIRE(n2->node.node_type == NodeType::BFUNC);
    REQUIRE(n2->node.function == Function::MUL);
    REQUIRE(n2->left->node.node_type == NodeType::CONST);
    REQUIRE(n2->left->node.constant == -7);
    REQUIRE(n2->right->node.node_type == NodeType::VAR);
    REQUIRE(n2->right->node.variable == 3);
    delete n2;

    // 3. Even number of coeffs
    m1.set_active(0, false);
    m1.set_active(1, false);
    m1.set_active(2, false);
    m1.set_active(3, true);
    m1.set_active(4, true);
    m1.set_active(5, false);
    TreeNode* n3 = new TreeNode();
    m1.get_node(n3);
    //
    //
    //            +
    //         /     \
    //       *         *
    //     /   \     /   \
    //   -7     3   3     4
    //
    REQUIRE(n3->node.node_type == NodeType::BFUNC);
    REQUIRE(n3->node.function == Function::ADD);
    // Root left
    REQUIRE(n3->left->node.node_type == NodeType::BFUNC);
    REQUIRE(n3->left->node.function == Function::MUL);
    REQUIRE(n3->left->left->node.node_type == NodeType::CONST);
    REQUIRE(n3->left->left->node.constant == -7);
    REQUIRE(n3->left->right->node.node_type == NodeType::VAR);
    REQUIRE(n3->left->right->node.variable == 3);
    // Root right
    REQUIRE(n3->right->node.node_type == NodeType::BFUNC);
    REQUIRE(n3->right->node.function == Function::MUL);
    REQUIRE(n3->right->left->node.node_type == NodeType::CONST);
    REQUIRE(n3->right->left->node.constant == 3);
    REQUIRE(n3->right->right->node.node_type == NodeType::VAR);
    REQUIRE(n3->right->right->node.variable == 4);
    delete n3;

    // 4. Even number of coeffs with constant
    m1.set_active(0, false);
    m1.set_active(1, false);
    m1.set_active(2, false);
    m1.set_active(3, true);
    m1.set_active(4, true);
    m1.set_active(5, true);
    TreeNode* n4 = new TreeNode();
    m1.get_node(n4);
    //
    //
    //            +
    //         /     \
    //       *         + 
    //     /   \     /   \
    //   -7     3   *     -20
    //             / \
    //            3   4
    //
    REQUIRE(n4->node.node_type == NodeType::BFUNC);
    REQUIRE(n4->node.function == Function::ADD);
    // Root left
    REQUIRE(n4->left->node.node_type == NodeType::BFUNC);
    REQUIRE(n4->left->node.function == Function::MUL);
    REQUIRE(n4->left->left->node.node_type == NodeType::CONST);
    REQUIRE(n4->left->left->node.constant == -7);
    REQUIRE(n4->left->right->node.node_type == NodeType::VAR);
    REQUIRE(n4->left->right->node.variable == 3);
    // Root right
    REQUIRE(n4->right->node.node_type == NodeType::BFUNC);
    REQUIRE(n4->right->node.function == Function::ADD);
    // Root right left
    REQUIRE(n4->right->left->node.node_type == NodeType::BFUNC);
    REQUIRE(n4->right->left->node.function == Function::MUL);
    REQUIRE(n4->right->left->left->node.node_type == NodeType::CONST);
    REQUIRE(n4->right->left->left->node.constant == 3);
    REQUIRE(n4->right->left->right->node.node_type == NodeType::VAR);
    REQUIRE(n4->right->left->right->node.variable == 4);
    // Root right right
    REQUIRE(n4->right->right->node.node_type == NodeType::CONST);
    REQUIRE(n4->right->right->node.constant == -20);
    delete n4;

    // 5. Odd number of coeffs
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, false);
    TreeNode* n5 = new TreeNode();
    m1.get_node(n5);
    //
    //                +
    //             /     \ 
    //       (A)  *         +  (B)
    //          / \        /    \ 
    //         1   0      * (C)     * (D)
    //                   / \      /    \
    //                  2  2     3     4
    //                       
    // Root Node
    REQUIRE(n5->node.function == Function::ADD);
    REQUIRE(n5->node.node_type == NodeType::BFUNC);
    
    // Root Left node (A)
    REQUIRE(n5->left->node.function == Function::MUL);
    REQUIRE(n5->left->node.node_type == NodeType::BFUNC);
    // Left left node is const
    REQUIRE(n5->left->left->node.constant == 1);
    REQUIRE(n5->left->left->node.node_type == NodeType::CONST);
    // Left right node is var 0
    REQUIRE(n5->left->right->node.variable == 0);
    REQUIRE(n5->left->right->node.node_type == NodeType::VAR);

    // Root right node (B)
    REQUIRE(n5->right->node.function == Function::ADD);
    REQUIRE(n5->right->node.node_type == NodeType::BFUNC);

    // Root right left node (C)
    REQUIRE(n5->right->left->node.function == Function::MUL);
    REQUIRE(n5->right->left->node.node_type == NodeType::BFUNC);
    REQUIRE(n5->right->left->left->node.constant == 2);
    REQUIRE(n5->right->left->left->node.node_type == NodeType::CONST);
    REQUIRE(n5->right->left->right->node.variable == 2);
    REQUIRE(n5->right->left->right->node.node_type == NodeType::VAR);

    // Root right right node (D)
    REQUIRE(n5->right->right->node.function == Function::MUL);
    REQUIRE(n5->right->right->node.node_type == NodeType::BFUNC);

    // Root right right left node (E)    
    REQUIRE(n5->right->right->left->node.constant == 3);
    REQUIRE(n5->right->right->left->node.node_type == NodeType::CONST);
    REQUIRE(n5->right->right->right->node.variable == 4);
    REQUIRE(n5->right->right->right->node.node_type == NodeType::VAR);
    delete n5;

    // 5. Odd number of coeffs with constant
    //
    //                +
    //             /     \ 
    //       (A)  *         +  (B)
    //          / \        /    \ 
    //         1   0      * (C)     + (D)
    //                   / \      /    \
    //                  2  2    * (E)   -20   (F)
    //                        / \         
    //                       3   4     
    //
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    m1.set_active(5, true);
    TreeNode * node = new TreeNode();
    m1.get_node(node);
    // Root Node
    REQUIRE(node->node.function == Function::ADD);
    REQUIRE(node->node.node_type == NodeType::BFUNC);
    
    // Root Left node (A)
    REQUIRE(node->left->node.function == Function::MUL);
    REQUIRE(node->left->node.node_type == NodeType::BFUNC);
    // Left left node is const
    REQUIRE(node->left->left->node.constant == 1);
    REQUIRE(node->left->left->node.node_type == NodeType::CONST);
    // Left right node is var 0
    REQUIRE(node->left->right->node.variable == 0);
    REQUIRE(node->left->right->node.node_type == NodeType::VAR);

    // Root right node (B)
    REQUIRE(node->right->node.function == Function::ADD);
    REQUIRE(node->right->node.node_type == NodeType::BFUNC);

    // Root right left node (C)
    REQUIRE(node->right->left->node.function == Function::MUL);
    REQUIRE(node->right->left->node.node_type == NodeType::BFUNC);
    REQUIRE(node->right->left->left->node.constant == 2);
    REQUIRE(node->right->left->left->node.node_type == NodeType::CONST);
    REQUIRE(node->right->left->right->node.variable == 2);
    REQUIRE(node->right->left->right->node.node_type == NodeType::VAR);

    // Root right right node (D)
    REQUIRE(node->right->right->node.function == Function::ADD);
    REQUIRE(node->right->right->node.node_type == NodeType::BFUNC);

    // Root right right left node (E)
    REQUIRE(node->right->right->left->node.function == Function::MUL);
    REQUIRE(node->right->right->left->node.node_type == NodeType::BFUNC);
    REQUIRE(node->right->right->left->left->node.constant == 3);
    REQUIRE(node->right->right->left->left->node.node_type == NodeType::CONST);
    REQUIRE(node->right->right->left->right->node.variable == 4);
    REQUIRE(node->right->right->left->right->node.node_type == NodeType::VAR);

    // Root right right right node (F)
    REQUIRE(node->right->right->right->node.constant == -20);
    REQUIRE(node->right->right->right->node.node_type == NodeType::CONST);    
    delete node;

}
