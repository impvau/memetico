#include "doctest.h"

#include <memetico/globals.h>
#include <memetico/models/cont_frac.h>
#include <memetico/models/regression.h>

#include <string>
#include <sstream>
#include <typeinfo>

TEST_CASE("ContinuedFraction: construct, randomise()") {

    size_t size = 6;
    size_t frac_depth = 1;

    // Set the data, leaving the constant at the end
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    memetico::RANDREAL = RandReal(42);
    ContinuedFraction<Regression<double>>* c = new ContinuedFraction<Regression<double>>(frac_depth);    

    size_t terms = 2*frac_depth+1;
    size_t total_params = size*terms;

    // Ensure the variables are initialised as expected
    REQUIRE(c->get_depth() == frac_depth);
    REQUIRE(c->get_terms() == terms);
    REQUIRE(c->get_params_per_term() == Data::IVS.size()+1);
    REQUIRE(c->get_param_count() == total_params);

    // Ensure that all terms are regression objects    
    for(size_t term = 0; term < terms; term++) {
        string name = typeid(Regression<double>).name();
        REQUIRE( name.compare("10RegressionIdE") == 0 );
    }

    // Check Global Active flags after construction
    for(size_t param = 0; param < c->get_params_per_term(); param++) {

        // If globally on
        if( c->get_global_active(param)) {

            // For all terms
            for(size_t term = 0; term < terms; term++) {

                // Ensure that paramter is on
                REQUIRE( c->get_objs(term)->get_active(param) == true);

                // Ensure randomised value between limits
                REQUIRE((
                    (
                        c->get_objs(term)->get_active(param)  >= ContinuedFraction<double>::RAND_LOWER &&
                        c->get_objs(term)->get_active(param)  <= ContinuedFraction<double>::RAND_UPPER
                    ) || (
                        c->get_objs(term)->get_active(param)  >= ContinuedFraction<double>::RAND_UPPER*-1 &&
                        c->get_objs(term)->get_active(param)  <= ContinuedFraction<double>::RAND_LOWER*-1
                    )
                ));
            }
        } else {
        
            // If globally off, ensure all terms are off
            for(size_t term = 0; term < terms; term++) {
                REQUIRE( c->get_objs(term)->get_active(param) == false);
            }
        }
    }
}

TEST_CASE("ContinuedFraction: << ") {

    size_t size = 6;
    size_t frac_depth = 2;

    // Set the data, leaving the constant at the end
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    memetico::RANDREAL = RandReal(42);
    memetico::RANDINT = RandInt(42);
    ContinuedFraction<Regression<double>>* c = new ContinuedFraction<Regression<double>>(frac_depth);    

    // Terms based on Seed = 42
    //2*x2+3*x5+3
    //-3*x2+3*x5-1
    //-2*x2+2*x5-3
    //-2*x2-3*x5+1
    //x2-2*x5+3

    stringstream ss;
    ss << *c;
    REQUIRE( ss.str().compare("(2*x2+3*x5+3)+(-3*x2+3*x5-1)/((-2*x2+2*x5-3)+(-2*x2-3*x5+1)/((x2-2*x5+3)))") == 0);

}

TEST_CASE("ContinuedFraction<T>: evaluate()") {

    size_t size = 6;
    size_t frac_depth = 2;

    // Set the data, leaving the constant at the end
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    memetico::RANDREAL = RandReal(42);
    memetico::RANDINT = RandInt(42);
    ContinuedFraction<Regression<double>>* c = new ContinuedFraction<Regression<double>>(frac_depth);    

    // Terms based on Seed = 42
    // (2*x2+3*x5+3)+(-3*x2+3*x5-1)/((-2*x2+2*x5-3)+(-2*x2-3*x5+1)/((x2-2*x5+3)))

    double * data = new double[size];
    data[0] = 4;
    data[1] = 20;
    data[2] = -3;
    data[3] = 15;
    data[4] = -3;

    REQUIRE( to_string(c->evaluate(data)) == to_string((double)51364/1451) );

    delete c;
    delete data;

}

TEST_CASE("ContinuedFraction<T>: active_params()") {

    size_t size = 6;
    size_t frac_depth = 2;

    // Set the data, leaving the constant at the end
    Data::IVS = vector<string>();
    for(size_t i = 0; i < size-1; i++)
        Data::IVS.push_back("x"+to_string(i+1));
    
    memetico::RANDREAL = RandReal(42);
    memetico::RANDINT = RandInt(42);
    ContinuedFraction<Regression<double>>* c = new ContinuedFraction<Regression<double>>(frac_depth);    

    // Terms based on Seed = 42
    // (2*x2+3*x5+3)+(-3*x2+3*x5-1)/((-2*x2+2*x5-3)+(-2*x2-3*x5+1)/((x2-2*x5+3)))
    // 1, 4, 5 = 1, 4, 5
    // 1, 4, 5 = 7, 10, 11
    // 1, 4, 5 = 13, 16, 17
    // 1, 4, 5 = 19, 22, 23
    // 1, 4, 5 = 25, 28, 29

    vector<size_t> positions = c->get_active_positions();
    REQUIRE(positions[0] == 1);
    REQUIRE(positions[1] == 4);
    REQUIRE(positions[2] == 5);

    REQUIRE(positions[3] == 7);
    REQUIRE(positions[4] == 10);
    REQUIRE(positions[5] == 11);

    REQUIRE(positions[6] == 13);
    REQUIRE(positions[7] == 16);
    REQUIRE(positions[8] == 17);

    REQUIRE(positions[9] == 19);
    REQUIRE(positions[10] == 22);
    REQUIRE(positions[11] == 23);

    REQUIRE(positions[12] == 25);
    REQUIRE(positions[13] == 28);
    REQUIRE(positions[14] == 29);

    REQUIRE(positions.size() == 15);

    delete c;

}