
#include "doctest.h"
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac.h>
#include <sstream>
#include <memetico/optimise/objective.h>
#include <memetico/models/mutation.h>

// Define the template strucutre of a model
template<
    typename T,         
    typename U, 
    template <typename, typename> class MutationPolicy>
struct Traits {
    using TType = T;                        // Term type, e.g. Regression<double>
    using UType = U;                        // Data type, e.g. double, should match T::TType
    template <typename V, typename W>
    using MPType = MutationPolicy<V, W>;    // Mutation Policy general class, e.g. MutateHardSoft<TermType, DataType>
};

typedef double DataType;
typedef Regression<DataType> TermType;
typedef ContinuedFraction<Traits<TermType, DataType, mutation::MutateHardSoft>> ModelType;

inline void setup_cont_frac_ivs(size_t size) {

    // Setup IVs in Regression
    ModelType::IVS.clear();
    for(size_t i = 0; i < size-1; i++)
        ModelType::IVS.push_back("x"+to_string(i+1));

}

inline TermType build_reg_frac(vector<bool> active, vector<double> vals, size_t params) {

    if( active.size() != params )
        throw invalid_argument("active.size() != params");

    if( vals.size() != params )
        throw invalid_argument("vals.size() != params");

    TermType o = TermType(params);
    for(size_t i = 0; i < params; i++) {
        o.set_active(i, active[i]);
        o.set_value(i, vals[i]);
    }

    return o;
}

inline ModelType build_cont_frac(vector<bool> active, vector<double> vals, vector<bool> globals, size_t params, size_t depth) {

    if( globals.size() != params)
        throw invalid_argument("globals.size() != params");

    if( active.size() != params*(2*depth+1) )
        throw invalid_argument("active.size() != params*2*depth");

    if( vals.size() != params*(2*depth+1) )
        throw invalid_argument("vals.size() != params*2*depth");

    ModelType c = ModelType(depth);

    for(size_t i = 0; i < params; i++)
        c.set_global_active(i, globals[i]);

    size_t terms = 2*depth+1;
    for(size_t i = 0; i < terms; i++) {

        vector reg_active(active.begin()+i*params, active.begin()+i*params+params);
        vector reg_vals(vals.begin()+i*params, vals.begin()+i*params+params);
        Regression<double> o = build_reg_frac(reg_active,reg_vals,params);
        c.set_terms(i, o);
    }

    return c;
}

inline ModelType frac_1() {

    // f(x) = x1 + 2x3 + 3x5 - 20 
    return build_cont_frac(
        {true, false, true, false, true, true},
        {1, 12, 2, -7, 3, -20},
        {true, false, true, false, true, true},
        6,
        0
    );

}

inline ModelType frac_2() {

    // f(x) = -3x2 + 4x4 + 3x5 - 3
    return build_cont_frac(
        {false, true, false, true, true, true},
        {0,-3,0,4,3,-3},
        {false, true, false, true, true, true},
        6,
        0
    );

}

inline ModelType frac_3() {

    // t1(x) =  x1 + 2x3 + 3x5 - 20
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  x1 + 2x3 + 3x5 - 20
    // f(x) = t1(x) + t2(x) / t3(x)
    return build_cont_frac(
        {
            true, false, true, false, true, true,
            false, true, false, true, true, true,
            true, false, true, false, true, true
        },
        {
            1, 12, 2, -7, 3, -20,
            0, -3, 0, 4, 3, -3,
            1, 12, 2, -7, 3, -20
        },
        {   true, true, true, true, true, true },
        6,
        1
    );
}

TEST_CASE("ContinuedFractions: set_depth, get_terms, set_terms, get_count_active, get_active, get_value") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 1;
    setup_cont_frac_ivs(params);
    ModelType o;

    // 1. Set depth
    o.set_depth(depth);
    REQUIRE(o.get_depth() == depth);
    Regression<double> m = Regression<double>(params);
    // Set new terms as empty (they are randomised by detaul)
    o.set_terms(0, m);
    o.set_terms(1, m);
    o.set_terms(2, m);
    REQUIRE(o.get_count_active() == 0);
    // 2. Set term 1
    // f(x) = x1 + 2x3 + 3x5 - 20
    Regression<double> m1 = frac_1().get_terms(0);
    // 2.1 Make sure term is set as expected
    o.set_terms(0,m1);
    REQUIRE(o.get_terms(0) == m1);
    REQUIRE(o.get_count_active() == 4);
    REQUIRE(o.get_active_positions().size() == 4);
    REQUIRE(o.get_active_positions()[0] == 0);
    REQUIRE(o.get_active_positions()[1] == 2);
    REQUIRE(o.get_active_positions()[2] == 4);
    REQUIRE(o.get_active_positions()[3] == 5);
    REQUIRE(o.get_active(0) == m1.get_active(0));
    REQUIRE(o.get_active(1) == m1.get_active(1));
    REQUIRE(o.get_active(2) == m1.get_active(2));
    REQUIRE(o.get_active(3) == m1.get_active(3));
    REQUIRE(o.get_active(4) == m1.get_active(4));
    REQUIRE(o.get_active(5) == m1.get_active(5));
    REQUIRE(o.get_value(0) == m1.get_value(0));
    REQUIRE(o.get_value(1) == m1.get_value(1));
    REQUIRE(o.get_value(2) == m1.get_value(2));
    REQUIRE(o.get_value(3) == m1.get_value(3));
    REQUIRE(o.get_value(4) == m1.get_value(4));
    REQUIRE(o.get_value(5) == m1.get_value(5));
    // 2.2 Make sure changing the original does not impact the fraction
    double val_save = m1.get_value(4);
    double val_new = 234;
    m1.set_value(4,val_new);
    REQUIRE(!(o.get_terms(0) == m1));
    // Revert changes to the term 1
    m1.set_value(4,val_save);
    // 3. Set term 2
    // f(x) = -3x2 + 4x4 + 3x5 - 3
    Regression<double> m2 = frac_2().get_terms(0);
    o.set_terms(1, m2);
    // 3.1 Check set term is as expected, and term 1 is not impacted
    REQUIRE(o.get_terms(0) == m1);
    REQUIRE(o.get_terms(1) == m2);
    REQUIRE(o.get_count_active() == 8);
    REQUIRE(o.get_active_positions().size() == 8);
    REQUIRE(o.get_active_positions()[0] == 0);
    REQUIRE(o.get_active_positions()[1] == 2);
    REQUIRE(o.get_active_positions()[2] == 4);
    REQUIRE(o.get_active_positions()[3] == 5);
    REQUIRE(o.get_active_positions()[4] == 7);
    REQUIRE(o.get_active_positions()[5] == 9);
    REQUIRE(o.get_active_positions()[6] == 10);
    REQUIRE(o.get_active_positions()[7] == 11);
    REQUIRE(o.get_active(0) == m1.get_active(0));
    REQUIRE(o.get_active(1) == m1.get_active(1));
    REQUIRE(o.get_active(2) == m1.get_active(2));
    REQUIRE(o.get_active(3) == m1.get_active(3));
    REQUIRE(o.get_active(4) == m1.get_active(4));
    REQUIRE(o.get_active(5) == m1.get_active(5));
    REQUIRE(o.get_active(6) == m2.get_active(0));
    REQUIRE(o.get_active(7) == m2.get_active(1));
    REQUIRE(o.get_active(8) == m2.get_active(2));
    REQUIRE(o.get_active(9) == m2.get_active(3));
    REQUIRE(o.get_active(10) == m2.get_active(4));
    REQUIRE(o.get_active(11) == m2.get_active(5));
    REQUIRE(o.get_value(0) == m1.get_value(0));
    REQUIRE(o.get_value(1) == m1.get_value(1));
    REQUIRE(o.get_value(2) == m1.get_value(2));
    REQUIRE(o.get_value(3) == m1.get_value(3));
    REQUIRE(o.get_value(4) == m1.get_value(4));
    REQUIRE(o.get_value(5) == m1.get_value(5));
    REQUIRE(o.get_value(6) == m2.get_value(0));
    REQUIRE(o.get_value(7) == m2.get_value(1));
    REQUIRE(o.get_value(8) == m2.get_value(2));
    REQUIRE(o.get_value(9) == m2.get_value(3));
    REQUIRE(o.get_value(10) == m2.get_value(4));
    REQUIRE(o.get_value(11) == m2.get_value(5));
    // 3.2 Make sure changing the original does not impact the fraction
    val_save = m2.get_value(4);
    m2.set_value(4,val_new);
    REQUIRE(!(o.get_terms(1) == m2));
    m2.set_value(4,val_save); // Revert
    // 4. Set term 3 as term 1
    // 4.1 Check term 3 is set and others are not impacted
    o.set_terms(2, m1);
    REQUIRE(o.get_terms(2) == m1);
    REQUIRE(o.get_terms(2) == o.get_terms(0));
    REQUIRE(o.get_terms(1) == m2);
    REQUIRE(o.get_terms(0) == m1);
    REQUIRE(o.get_count_active() == 12);
    REQUIRE(o.get_active_positions().size() == 12);
    REQUIRE(o.get_active_positions()[0] == 0);
    REQUIRE(o.get_active_positions()[1] == 2);
    REQUIRE(o.get_active_positions()[2] == 4);
    REQUIRE(o.get_active_positions()[3] == 5);
    REQUIRE(o.get_active_positions()[4] == 7);
    REQUIRE(o.get_active_positions()[5] == 9);
    REQUIRE(o.get_active_positions()[6] == 10);
    REQUIRE(o.get_active_positions()[7] == 11);
    REQUIRE(o.get_active_positions()[8] == 12);
    REQUIRE(o.get_active_positions()[9] == 14);
    REQUIRE(o.get_active_positions()[10] == 16);
    REQUIRE(o.get_active_positions()[11] == 17);
    REQUIRE(o.get_active(0) == m1.get_active(0));
    REQUIRE(o.get_active(1) == m1.get_active(1));
    REQUIRE(o.get_active(2) == m1.get_active(2));
    REQUIRE(o.get_active(3) == m1.get_active(3));
    REQUIRE(o.get_active(4) == m1.get_active(4));
    REQUIRE(o.get_active(5) == m1.get_active(5));
    REQUIRE(o.get_active(6) == m2.get_active(0));
    REQUIRE(o.get_active(7) == m2.get_active(1));
    REQUIRE(o.get_active(8) == m2.get_active(2));
    REQUIRE(o.get_active(9) == m2.get_active(3));
    REQUIRE(o.get_active(10) == m2.get_active(4));
    REQUIRE(o.get_active(11) == m2.get_active(5));
    REQUIRE(o.get_active(12) == m1.get_active(0));
    REQUIRE(o.get_active(13) == m1.get_active(1));
    REQUIRE(o.get_active(14) == m1.get_active(2));
    REQUIRE(o.get_active(15) == m1.get_active(3));
    REQUIRE(o.get_active(16) == m1.get_active(4));
    REQUIRE(o.get_active(17) == m1.get_active(5));
    REQUIRE(o.get_value(0) == m1.get_value(0));
    REQUIRE(o.get_value(1) == m1.get_value(1));
    REQUIRE(o.get_value(2) == m1.get_value(2));
    REQUIRE(o.get_value(3) == m1.get_value(3));
    REQUIRE(o.get_value(4) == m1.get_value(4));
    REQUIRE(o.get_value(5) == m1.get_value(5));
    REQUIRE(o.get_value(6) == m2.get_value(0));
    REQUIRE(o.get_value(7) == m2.get_value(1));
    REQUIRE(o.get_value(8) == m2.get_value(2));
    REQUIRE(o.get_value(9) == m2.get_value(3));
    REQUIRE(o.get_value(10) == m2.get_value(4));
    REQUIRE(o.get_value(11) == m2.get_value(5));
    REQUIRE(o.get_value(12) == m1.get_value(0));
    REQUIRE(o.get_value(13) == m1.get_value(1));
    REQUIRE(o.get_value(14) == m1.get_value(2));
    REQUIRE(o.get_value(15) == m1.get_value(3));
    REQUIRE(o.get_value(16) == m1.get_value(4));
    REQUIRE(o.get_value(17) == m1.get_value(5));
    // 4.2 Change originaly object and ensure the CF is not changed
    m1.set_active(0,false);
    REQUIRE(!(o.get_terms(2) == m1));
    REQUIRE(!(o.get_terms(0) == m1));
    REQUIRE(o.get_terms(0) == o.get_terms(2));
    m1.set_active(0, true);  // Revert
    // 4.3 Change one of the terms 0 or 2 and ensure the other does not change
    o.get_terms(0).set_active(0, false);
    REQUIRE(!(o.get_terms(0) == o.get_terms(2)));
    REQUIRE(o.get_terms(2) == m1);
    REQUIRE(!(o.get_terms(0) == m1));

}

TEST_CASE("ContinuedFractions: ==, ContinuedFractions<T>(ContinuedFractions<T>) ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 0;
    setup_cont_frac_ivs(params);
    ModelType o = frac_1();

    // 1. Compare to empty fraction
    ModelType o2 = ModelType();
    REQUIRE(!(o == o2));
    // 2. Compare to random fraction
    ModelType o3 = ModelType(depth);
    REQUIRE(!(o == o3));
    // 3. Compare to self
    REQUIRE( (o == o) );

    ModelType o4 = frac_1();
    // 4. Compare to same object
    REQUIRE( (o == o4) );

    // 5. Construct from existing object and compare
    o.set_fitness(5);
    o.set_penalty(2);
    o.set_error(16);
    ModelType o5 = ModelType(o);
    REQUIRE( (o == o5) );
    REQUIRE( o.get_fitness() == o5.get_fitness() );
    REQUIRE( o.get_error() == o5.get_error() );
    REQUIRE( o.get_penalty() == o5.get_penalty() );
    // 6. Change a term and ensure it does not change in both
    o.get_terms(0).set_active(0, !o.get_active(0));
    REQUIRE( !(o == o5) );

}

TEST_CASE("ContinuedFractions: evaluate ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 0;
    setup_cont_frac_ivs(params);

    // 1. Check single term fraction
    // f(x) = x1 + 2x3 + 3x5 - 20
    ModelType o = frac_1();
    // 1.1 Evaluate on int data
    vector<double> values;
    values.push_back(4);
    values.push_back(20);
    values.push_back(-3);
    values.push_back(15);
    values.push_back(-3);
    REQUIRE( o.evaluate(values) == -31 );
    // 1.1 Evaluate on double data
    values.clear();
    values.push_back(3.1);
    values.push_back(-43.2);
    values.push_back(-32.1);
    values.push_back(34.2);
    values.push_back(-3.1);
    REQUIRE( o.evaluate(values) == -90.4 );

    // 2. Check with three terms
    // t1(x) =  x1 + 2x3 + 3x5 - 20
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  x1 + 2x3 + 3x5 - 20
    ModelType o2 = frac_3();
    // 2.1 Evaluate on int data
    values.clear();
    values.push_back(4);
    values.push_back(20);
    values.push_back(-3);
    values.push_back(15);
    values.push_back(-3);
    // We expect -31 + (-12/-31) = 30.612903
    REQUIRE( (o2.evaluate(values) - -30.6129032258) < 0.00000001 );
    
    // 3. Add another four terms and evaluate with doubles
    // t1(x) =  x1 + 2x3 + 3x5 - 20 
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  x1 + 2x3 + 3x5 - 20
    // t4(x) =  x1 + 2x3 + 3x5 - 20
    // t5(x) =  x1 + 2x3 + 3x5 - 20
    // t6(x) =  x1 + 2x3 + 3x5 - 20
    // t7(x) =  -3x2 + 4x4 + 3x5 - 3
    depth = 3;
    o = ModelType(depth);
    o.set_terms(0, o2.get_terms(0) );
    o.set_terms(1, o2.get_terms(1));
    o.set_terms(2, o2.get_terms(0));
    o.set_terms(3, o2.get_terms(0));
    o.set_terms(4, o2.get_terms(0));
    o.set_terms(5, o2.get_terms(0));
    o.set_terms(6, o2.get_terms(1));
    values.clear();
    values.push_back(3.1);
    values.push_back(-43.2);
    values.push_back(-32.1);
    values.push_back(34.2);
    values.push_back(-3.1);
    // 3.1 Evaluate on float data
    // We expect -90.4 + (254.1)/(-90.4+(-90.4/(-90.4+-90.4/(-90.4+(254.1))))) ~ -93.242
    REQUIRE( (o.evaluate(values) - -93.242088857) < 0.00000001  );

}

TEST_CASE("ContinuedFractions: mutate ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 1;
    setup_cont_frac_ivs(params);

    // Setup fractions with three terms
    // t1(x) =  x1 + 2x3 + 3x5 - 20
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  x1 + 2x3 + 3x5 - 20
    ModelType o1 = ModelType(depth);
    o1.set_global_active(0, false);
    o1.set_global_active(1, false);
    o1.set_global_active(2, false);
    o1.set_global_active(3, false);
    o1.set_global_active(4, false);
    o1.set_global_active(5, false);
    Regression<double> m1 = frac_1().get_terms(0);
    Regression<double> m2 = frac_2().get_terms(0);
    // Set the terms
    o1.set_terms(0, m1);
    o1.set_terms(1, m2);
    o1.set_terms(2, m1);

    // 1. Setup a fraction to trigger a hard mutate
    ModelType o2 = ModelType(o1);
    o2.set_fitness(1);
    o1.set_fitness(o2.get_fitness()*2.1);

    size_t count_global_active = 0;
    size_t last_global_active = 0;
    size_t last_params_active = o1.get_count_active();
    for(size_t i = 0; i < 1000; i++) {

        count_global_active = 0;
        o1.mutate(o2);
        for(size_t j = 0; j < params; j++) {
            if( o1.get_global_active(j) )
                count_global_active++;
        }
        REQUIRE( ((count_global_active == last_global_active+1) || (count_global_active == last_global_active-1)) );
        // We can't check that all terms change, because some may have local inactive etc.
        // Could be between +3 and 0 when the global flag changes on a depth 1 (3 term) fraction
        //REQUIRE( ((o1.get_count_active() == last_params_active+3) || (o1.get_count_active() == last_params_active-3)) );
        last_global_active = count_global_active;
        last_params_active = o1.get_count_active();

    }

    // 2. Setup fraction to trigger soft mutate
    // Set the global flags true, so all parameters are available to toggle 
    // (as turning on an element when the global flag is off does nothing)
    o1.set_global_active(0, true);
    o1.set_global_active(1, true);
    o1.set_global_active(2, true);
    o1.set_global_active(3, true);
    o1.set_global_active(4, true);
    o1.set_global_active(5, true);
    o2.set_fitness(1);
    o1.set_fitness(o2.get_fitness()*1.3);
    size_t last_count_active = o1.get_count_active();
    for(size_t i = 0; i < 1000; i++) {

        o1.mutate(o2);
        // This test only works when not santising results so that the terms at least have a constant
        //REQUIRE( ((o1.get_count_active() == 0) ||(o1.get_count_active() == last_count_active+1) || (o1.get_count_active() == last_count_active-1)) );
        last_count_active = o1.get_count_active();

    }
}

TEST_CASE("ContinuedFractions: recombine ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 0;
    setup_cont_frac_ivs(params);
    
    // Tests
    // 1. That omething like a recombination occurs (up to Regression<T> that is already tested)
    // 2. Two fractions of different depth only recombine the depths they have in common

    // 1. 
    ModelType c = ModelType(depth);
    ModelType c1 = frac_1();
    ModelType c2 = frac_2();
    // Reset seed as CF constructors use generator
    // Now results will match those in Regression tests
    ri = RandInt(42);
    rr = RandReal(42);
    c.recombine(&c1, &c2);
    REQUIRE( c.get_active(0) == false );
    REQUIRE( c.get_active(1) == false );
    REQUIRE( c.get_active(2) == false );
    REQUIRE( c.get_active(3) == false );
    REQUIRE( c.get_active(4) == true );
    REQUIRE( c.get_active(5) == true );
    REQUIRE( c.get_value(4) == 3 );
    REQUIRE( to_string(c.get_value(5)) == to_string((double)8/3) );
    
    // 2. 
    depth = 1;
    c = ModelType(depth);
    ModelType c_copy = ModelType(c);
    ModelType c3 = frac_3();    // Depth 1
    ri = RandInt(42);
    rr = RandReal(42);
    c.recombine(&c1, &c3);
    // We expect depth 0 to be recombined (i.e. first term)
    // The next two terms should be the same as they are in frac_3()
    REQUIRE( !(c.get_terms(0) == c_copy.get_terms(0)) );
    REQUIRE( (c.get_terms(1) == c_copy.get_terms(1)) );
    REQUIRE( (c.get_terms(2) == c_copy.get_terms(2)) );
}

TEST_CASE("ContinuedFractions: << ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 0;
    setup_cont_frac_ivs(params);
    
    // Tests
    // 1. Print simple depth 0 fraction
    // 2. Print different simple depth 0 fraction
    // 3. Print with more terms

    // 1.
    ModelType c = frac_1();
    stringstream ss1;
    ss1 << c;
    REQUIRE( ss1.str() == "(B2+2*D2+3*F2-20)" );
    // 2.
    c = frac_2();
    stringstream ss2;
    ss2 << c;
    REQUIRE( ss2.str() == "(-3*C2+4*E2+3*F2-3)" );
    // 3. 
    depth = 3;
    ModelType c1 = ModelType(depth);
    Regression<double> t1 = frac_1().get_terms(0);
    Regression<double> t2 = frac_2().get_terms(0);
    c1.set_terms(0, t1);
    c1.set_terms(1, t2);
    c1.set_terms(2, t1);
    c1.set_terms(3, t1);
    c1.set_terms(4, t1);
    c1.set_terms(5, t1);
    c1.set_terms(6, t2);
    stringstream ss3;
    ss3 << c1;
    REQUIRE( ss3.str() == "(B2+2*D2+3*F2-20)+(-3*C2+4*E2+3*F2-3)/((B2+2*D2+3*F2-20)+(B2+2*D2+3*F2-20)/((B2+2*D2+3*F2-20)+(B2+2*D2+3*F2-20)/((-3*C2+4*E2+3*F2-3))))" );

}

TEST_CASE("ContinuedFractions: get_node ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 0;
    setup_cont_frac_ivs(params);
    
    // Test
    // 1. Single 

    // f(x) = x1 + 2x3 + 3x5 - 20 
    ModelType c1 = frac_1();
    TreeNode* n1 = new TreeNode();
    TreeNode* n2 = new TreeNode();
    c1.get_terms(0).get_node(n1);
    c1.get_node(n2);
    REQUIRE( *n1 == *n2 );
    delete n1;
    delete n2;

    
    // t1(x) =  x1 + 2x3 + 3x5 - 20
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  x1 + 2x3 + 3x5 - 20
    ModelType c2 = frac_3();
    TreeNode * frac_node = new TreeNode();
    c2.get_node(frac_node);
    
    n1 = new TreeNode();
    n2 = new TreeNode();
    TreeNode* n3 = new TreeNode();
    c2.get_terms(0).get_node(n1);
    c2.get_terms(1).get_node(n2);
    c2.get_terms(2).get_node(n3);

    TreeNode * local_node = new TreeNode();
    local_node->node.node_type = NodeType::BFUNC;
    local_node->node.function = Function::ADD;
    local_node->left = n1;
    local_node->right = new TreeNode();
    local_node->right->node.node_type = NodeType::BFUNC;
    local_node->right->node.function = Function::DIV;
    local_node->right->left = n2;
    local_node->right->right = n3;

    REQUIRE( *frac_node == *local_node );
    // n1, n2, n3 are within local_node, thus deleted by its deconstructor
    //delete n1;
    //delete n2;
    //delete n3;
    delete local_node;
    delete frac_node;

    // Add 2 terms, which are previous terms with constants turned off
    c2.set_depth(c2.get_depth()+1);
    Regression<double> t4 = Regression<double>(c2.get_terms(0));
    t4.set_active(5, false);
    Regression<double> t5 = Regression<double>(c2.get_terms(1));
    t5.set_active(5, false);
    c2.set_terms(3, t4);
    c2.set_terms(4, t5);
    frac_node = new TreeNode();
    c2.get_node(frac_node);

    n1 = new TreeNode();
    n2 = new TreeNode();
    n3 = new TreeNode();
    TreeNode * n4 = new TreeNode();
    TreeNode * n5 = new TreeNode();
    c2.get_terms(0).get_node(n1);
    c2.get_terms(1).get_node(n2);
    c2.get_terms(2).get_node(n3);
    c2.get_terms(3).get_node(n4);
    c2.get_terms(4).get_node(n5);

    local_node = new TreeNode();
    local_node->node.node_type = NodeType::BFUNC;
    local_node->node.function = Function::ADD;

    local_node->left = n1;

    local_node->right = new TreeNode();
    local_node->right->node.node_type = NodeType::BFUNC;
    local_node->right->node.function = Function::DIV;

    local_node->right->left = n2;

    local_node->right->right = new TreeNode();
    local_node->right->right->node.node_type = NodeType::BFUNC;
    local_node->right->right->node.function = Function::ADD;

    local_node->right->right->left = n3;

    local_node->right->right->right = new TreeNode();
    local_node->right->right->right->node.node_type = NodeType::BFUNC;
    local_node->right->right->right->node.function = Function::DIV;

    local_node->right->right->right->left = n4;
    local_node->right->right->right->right = n5;

    REQUIRE( *local_node == *frac_node );

    // deleteing n1, n2, n3, n4, n5 is done by the deconstructor of local_node
    delete local_node;
    delete frac_node;

}
