
#include "doctest.h"
#include <string>
#include <ostream>
#include <memetico/data/data_set.h>
#include <memetico/models/regression.h>
#include <memetico/gpu/tree_node.h>
#include <memetico/optimise/objective.h>

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


TEST_CASE("CUSR: calculateFitness ") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    DataSet::IVS.clear();

    string fn = "test_data.csv";
    init(fn);
    DataSet ds = DataSet(fn, true);
    ds.load();
    
    Regression<double> r1 = Regression<double>(2);
    r1.set_active(0, true);
    r1.set_active(1, true);
    double d0 = 1;
    double d1 = -20;
    r1.set_value(0, d0);
    r1.set_value(1, d1);

    TreeNode * node = new TreeNode();
    r1.get_node(node);
    Program p;
    get_prefix(p.prefix, node);
    p.length = p.prefix.size();
    vector<Program> pop;
    pop.push_back(p);

    // Thread per block = 512, so all threads are in a single block, therefore we pass in 1
    float res = calculateFitness(ds.device_data, 1, pop, metric_t::mean_square_error );
    REQUIRE(  abs(res-6845.240365625) < 0.001 );
    delete node;

}
