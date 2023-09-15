
#include "doctest.h"
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac.h>
#include <memetico/models/branch_cont_frac_dd.h>
#include <sstream>
#include <memetico/optimise/objective.h>
#include <vector>
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
typedef ContinuedFraction<Traits<Regression<DataType>, DataType, mutation::MutateHardSoft>> TermType;
typedef BranchedContinuedFraction<Traits<Regression<DataType>, DataType, mutation::MutateHardSoft>> ModelType;

inline void setup_cont_frac_ivs(size_t size) {

    // Setup IVs in Regression
    ModelType::IVS.clear();
    for(size_t i = 0; i < size-1; i++)
        ModelType::IVS.push_back("x"+to_string(i+1));

}

inline Regression<double> build_reg_frac(vector<bool> active, vector<double> vals, size_t params) {

    if( active.size() != params )
        throw invalid_argument("active.size() != params");

    if( vals.size() != params )
        throw invalid_argument("vals.size() != params");

    Regression<double> o = Regression<double>(params);
    for(size_t i = 0; i < params; i++) {
        o.set_active(i, active[i]);
        o.set_value(i, vals[i]);
    }

    return o;
}

inline TermType build_cont_frac(vector<bool> active, vector<double> vals, vector<bool> globals, size_t params, size_t depth) {

    if( globals.size() != params)
        throw invalid_argument("globals.size() != params");

    if( active.size() != params*(2*depth+1) )
        throw invalid_argument("active.size() != params*2*depth");

    if( vals.size() != params*(2*depth+1) )
        throw invalid_argument("vals.size() != params*2*depth");

    TermType c = TermType(depth);

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

inline TermType frac_4() {

    // t1(x) =  x1 + 2x3 + 3x5 - 20
    // t2(x) = -3x2 + 4x4 + 3x5 - 3
    // t3(x) =  2x1 + x3 + x5 - 5
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
            2, 12, 1, -7, 1, -5
        },
        {   true, true, true, true, true, true },
        6,
        1
    );
}

/*
TEST_CASE("BranchedContinuedFractions: constructor, set_depth, get_terms, get_frac_terms, set_value, get_value") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 2;
    setup_cont_frac_ivs(params);

    // Test depth of 0 fraction
    meme::DEPTH = 0;
    ModelType o1(4);
    REQUIRE(o1.get_depth() == 0);
    TermType t = o1.get_terms(0);
    REQUIRE(o1.get_sub_depth() == 4);
    REQUIRE(t.get_depth() == 4);
    stringstream ss;
    ss << t;
    // Hard coded results from random seed 42
    string s = "(11*B2+19)+(-26*B2+22)/((-11*B2+1)+(-29*B2-1)/((22*B2+25)+(-2*B2-24)/((B2+6)+(30*B2+6)/((-12*B2-21)))))";
    REQUIRE(ss.str() == s);
    // Cont Frac
    ss.str("");
    ss << o1;
    REQUIRE(ss.str() == "("+s+")");
    REQUIRE(o1.get_frac_terms() == 1);
    REQUIRE(o1.get_terms(0) == t);
    REQUIRE(o1.get_param_count() == t.get_param_count());

    // Test depth of 2 fractions
    ri = RandInt(42);
    rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;
    meme::DEPTH = 1;
    ModelType o2(4);
    // Check Term 1 (strings based on initial seed = 42)
    TermType t1 = o2.get_terms(0);
    REQUIRE(t1.get_depth() == 4);
    ss.str("");
    ss << t1;
    string s1 = "(24*B2+19)+(6*B2+12)/((16*B2+30)+(18*B2+15)/((-29*B2-5)+(-12*B2-10)/((-25*B2-17)+(-29*B2+1)/((29*B2+29)))))";
    REQUIRE(ss.str() == s1);
    // Check Term 2
    TermType t2 = o2.get_terms(1);
    REQUIRE(t2.get_depth() == 4);
    ss.str("");
    ss << t2;
    string s2 = "(-2)+(17)/((-6)+(12)/((-21)+(-30)/((-28)+(7)/((21)))))";
    REQUIRE(ss.str() == s2);
    // Check Term 3
    TermType t3 = o2.get_terms(2);
    REQUIRE(t3.get_depth() == 4);
    ss.str("");
    ss << t3;
    string s3 = "(8*B2+14)+(21*B2-27)/((19*B2-16)+(-6*B2+2)/((-25*B2-3)+(12*B2-23)/((-25*B2-11)+(-8*B2-21)/((13*B2+16)))))";
    REQUIRE(ss.str() == s3);
    // Check Fraction
    ss.str("");
    ss << o2;
    string s4 = "("+s1+")+("+s2+")/(("+s3+"))";
    REQUIRE(o2.get_frac_terms() == 3);
    REQUIRE(o2.get_terms(0) == t1);
    REQUIRE(o2.get_terms(1) == t2);
    REQUIRE(o2.get_terms(2) == t3);
    REQUIRE(o2.get_param_count() == t1.get_param_count()+t2.get_param_count()+t3.get_param_count());
    REQUIRE(ss.str() == s4);

    // Check set/get values
    for(size_t i = 0; i < o2.get_param_count(); i++ )
        o2.set_value(i, i*1.0);

    // Check updated values - note that if the active is set to off, then get_value(i) = 0.0
    for(size_t i = 0; i < o2.get_param_count(); i++ )
        REQUIRE( (o2.get_value(i) == i*1.0 || o2.get_value(i) == 0.0) );

    // Check set/get active
    for(size_t i = 0; i < o2.get_param_count(); i++ ) {
        if( i % 2 == 0 )
            o2.set_active(i, true);
        else
            o2.set_active(i, false);
    }
    for(size_t i = 0; i < o2.get_param_count(); i++ ) {
        if( i % 2 == 0 )
            REQUIRE(o2.get_active(i) == true);
        else
            REQUIRE(o2.get_active(i) == false);
    }
    for(size_t i = 0; i < o2.get_param_count(); i++ ) {
        if( i % 2 == 0 )
            o2.set_active(i, false);
        else
            o2.set_active(i, true);
    }
    for(size_t i = 0; i < o2.get_param_count(); i++ ) {
        if( i % 2 == 0 )
            REQUIRE(o2.get_active(i) == false);
        else
            REQUIRE(o2.get_active(i) == true);
    }

}


TEST_CASE("BranchedContinuedFractions: evaluate ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 2;
    setup_cont_frac_ivs(params);

    meme::DEPTH = 1;
    ModelType o2(4);
    // ((24*B2+19)+(6*B2+12)/((16*B2+30)+(18*B2+15)/((-29*B2-5)+(-12*B2-10)/((-25*B2-17)+(-29*B2+1)/((29*B2+29))))))+((-2)+(17)/((-6)+(12)/((-21)+(-30)/((-28)+(7)/((21))))))/(((8*B2+14)+(21*B2-27)/((19*B2-16)+(-6*B2+2)/((-25*B2-3)+(12*B2-23)/((-25*B2-11)+(-8*B2-21)/((13*B2+16)))))))

    vector<double> vals = {4};
    REQUIRE( abs(115.2884968-o2.evaluate(vals)) < 0.000001 );
    
    vals = {20.15};
    REQUIRE( abs(502.9518641-o2.evaluate(vals)) < 0.000001 );
    
    vals = {-4.315};
    REQUIRE( abs(-83.97205100-o2.evaluate(vals)) < 0.000001 );

    // Maple code
    // f := B2 -> 24*B2 + 19 + (6*B2 + 12)/(16*B2 + 30 + (18*B2 + 15)/(-29*B2 - 5 + (-12*B2 - 10)/(-25*B2 - 17 + (-29*B2 + 1)/(29*B2 + 29)))) - 979/214*1/(8*B2 + 14 + (21*B2 - 27)/(19*B2 - 16 + (-6*B2 + 2)/(-25*B2 - 3 + (12*B2 - 23)/(-25*B2 - 11 + (-8*B2 - 21)/(13*B2 + 16)))));
}

TEST_CASE("BranchedContinuedFractions: BranchedContinuedFractions(BranchedContinuedFractions o), ==") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    Model::FORMAT = PrintLatex;

    size_t params = 2;
    setup_cont_frac_ivs(params);

    meme::DEPTH = 1;
    ModelType o2(4);
    
    ModelType o3 = ModelType(o2);
    REQUIRE(o2 == o3);

}
*/

/*TEST_CASE("ContinuedFractions: mutate") {
  
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 1;
    setup_cont_frac_ivs(params);

    ModelType o;
    o.set_depth(1);
    o.mutate(o);

}*/

/*TEST_CASE("ContinuedFractions: recombine") {
  
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 1;
    setup_cont_frac_ivs(params);

    ModelType o;
    o.set_depth(1);
    o.mutate(o);

}*/

/*TEST_CASE("ContinuedFractions: get node") {
  
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    size_t params = 6;
    size_t depth = 1;
    setup_cont_frac_ivs(params);

    ModelType o;
    o.set_depth(1);
    o.mutate(o);

}*/
