
#include "doctest.h"
#include <memetico/data/data_set.h>
#include <memetico/models/regression.h>
#include <memetico/models/cont_frac.h>
#include <memetico/optimise/objective.h>
#include <string>
#include <ostream>
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


inline void init(string filename) {

    // Create sample data
    string fn(filename);
    ofstream f;
    f.open(fn);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn);

    f << "y,x" << endl;
    // Argon dataset, code generated in excel with =CONCAT("f << ",CHAR(34),A2, ",", B2, CHAR(34), " << endl;")
    f << "363.94,3.1" << endl;
    f << "58.41,3.3" << endl;
    f << "-14.92,3.4" << endl;
    f << "-58.41,3.5" << endl;
    f << "-81.69,3.6" << endl;
    f << "-88.36,3.65" << endl;
    f << "-92.56,3.7" << endl;
    f << "-94.73,3.75" << endl;
    f << "-95.39,3.8" << endl;
    f << "-94.75,3.85" << endl;
    f << "-93.16,3.9" << endl;
    f << "-90.89,3.95" << endl;
    f << "-88.01,4" << endl;
    f << "-81.19,4.1" << endl;
    f << "-73.63,4.2" << endl;
    f << "-65.98,4.3" << endl;
    f << "-58.6,4.4" << endl;
    f << "-51.78,4.5" << endl;
    f << "-45.6,4.6" << endl;
    f << "-40.13,4.7" << endl;
    f << "-35.34,4.8" << endl;
    f << "-31.17,4.9" << endl;
    f << "-27.57,5" << endl;
    f << "-24.46,5.1" << endl;
    f << "-19.37,5.3" << endl;
    f << "-15.47,5.5" << endl;
    f << "-12.43,5.7" << endl;
    f << "-10,5.9" << endl;
    f << "-8.13,6.1" << endl;
    f << "-6.63,6.3" << endl;
    f << "-5.44,6.5" << endl;
    f << "-4.49,6.7" << endl;

    f.close();

}

inline void init2(string filename) {

    // Create sample data
    string fn(filename);
    ofstream f;
    f.open(fn);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn);

        f << "y,w,x" << endl;  // Updated header to include weight column

    // Data with statically defined but varied weights
    f << "363.94,0.10,3.1" << endl;
    f << "58.41,0.11,3.3" << endl;
    f << "-14.92,0.12,3.4" << endl;
    f << "-58.41,0.13,3.5" << endl;
    f << "-81.69,0.14,3.6" << endl;
    f << "-88.36,0.15,3.65" << endl;
    f << "-92.56,0.16,3.7" << endl;
    f << "-94.73,0.17,3.75" << endl;
    f << "-95.39,0.18,3.8" << endl;
    f << "-94.75,0.19,3.85" << endl;
    f << "-93.16,0.20,3.9" << endl;
    f << "-90.89,0.21,3.95" << endl;
    f << "-88.01,0.22,4" << endl;
    f << "-81.19,0.23,4.1" << endl;
    f << "-73.63,0.24,4.2" << endl;
    f << "-65.98,0.25,4.3" << endl;
    f << "-58.6,0.26,4.4" << endl;
    f << "-51.78,0.27,4.5" << endl;
    f << "-45.6,0.28,4.6" << endl;
    f << "-40.13,0.29,4.7" << endl;
    f << "-35.34,0.30,4.8" << endl;
    f << "-31.17,0.31,4.9" << endl;
    f << "-27.57,0.32,5" << endl;
    f << "-24.46,0.33,5.1" << endl;
    f << "-19.37,0.34,5.3" << endl;
    f << "-15.47,0.35,5.5" << endl;
    f << "-12.43,0.36,5.7" << endl;
    f << "-10,0.37,5.9" << endl;
    f << "-8.13,0.38,6.1" << endl;
    f << "-6.63,0.39,6.3" << endl;
    f << "-5.44,0.40,6.5" << endl;
    f << "-4.49,0.41,6.7" << endl;

    f.close();

}

inline ModelType small_frac() {

    // f(x) = x1 - 20 
    size_t params = 2;
    size_t depth = 0;
    ModelType o  = ModelType(depth);
    Regression<double> m1 = Regression<double>(params);
    o.set_global_active(0, true);
    o.set_global_active(1, true);
    double d0 = 1;
    double d1 = -20;
    m1.set_active(0, true);
    m1.set_active(1, true);
    m1.set_value(0, d0);
    m1.set_value(1, d1);
    o.set_terms(0,m1);
    
    return o;
}

TEST_CASE("Objective: mse on CPU") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    DataSet::IVS.clear();

    string fn = "test_data.csv";
    init(fn);
    DataSet ds = DataSet(fn);
    ds.load();

    ModelType f1 = small_frac();

    ModelType::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        ModelType::IVS.push_back(DataSet::IVS[i]);

    vector<size_t> all;
    double res = objective::mse<DataType>(&f1, &ds, all);
    REQUIRE( abs(res-6845.240365625) < 0.0000001 );

    // Delete the file
    remove(fn.c_str());

}

TEST_CASE("Objective: mse on GPU") {

    meme::GPU = true;

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    DataSet::IVS.clear();

    string fn = "test_data.csv";
    init(fn);
    DataSet ds = DataSet(fn, true);
    ds.load();

    ModelType f1 = small_frac();

    ModelType::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        ModelType::IVS.push_back(DataSet::IVS[i]);

    vector<size_t> all;
    double res = objective::mse<DataType>(&f1, &ds, all);
    REQUIRE( abs(res-6845.240366) < 0.001 );

    // Delete the file
    remove(fn.c_str());

    
}

TEST_CASE("Objective: weighted mse on GPU") {

    meme::GPU = true;

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    DataSet::IVS.clear();

    string fn = "test_data.csv";
    init2(fn);
    DataSet ds = DataSet(fn,true);
    ds.load();

    ModelType f1 = small_frac();

    ModelType::IVS.clear();
    for(size_t i = 0; i < DataSet::IVS.size(); i++)
        ModelType::IVS.push_back(DataSet::IVS[i]);

    vector<size_t> all;
    double res = objective::mse<DataType>(&f1, &ds, all);
    REQUIRE( abs(res-3503.135844) < 0.001 );

    // Delete the file
    remove(fn.c_str());

}