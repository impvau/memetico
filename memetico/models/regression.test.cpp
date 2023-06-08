#include "doctest.h"
#include <memetico/models/regression.h>
#include <string>
#include <sstream>
#include <memetico/helpers/rng.h>
#include <memetico/optimise/objective.h>
#include <memetico/optimise/local_search.h>

void setup_regression_ivs(size_t size) {

    // Setup IVs in Regression
    MemeticModel<double>::IVS.clear();
    for(size_t i = 0; i < size-1; i++)
        MemeticModel<double>::IVS.push_back("x"+to_string(i+1));

}

TEST_CASE("Regression: Regression<T>(), Regression<T>(r1), Regression<T>(active, vals) ") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // 1. Default Regression object has a default term object size in length
    Regression<double> m1 = Regression<double>(size);
    m1.set_fitness(5);
    m1.set_penalty(2);
    m1.set_error(16);
    for(size_t i = 0; i < m1.get_count(); i++) {
        REQUIRE( m1.get_active(i) == false );
        REQUIRE( m1.get_value(i) == 0);
    }

    // 2. Construct from existing regressors
    Regression<double> m2 = Regression<double>(m1);
    REQUIRE( m1.get_fitness() == m2.get_fitness() );
    REQUIRE( m1.get_error() == m2.get_error() );
    REQUIRE( m1.get_penalty() == m2.get_penalty() );
    for(size_t i = 0; i < m1.get_count(); i++) {
        REQUIRE( m1.get_active(i) == m2.get_active(i));
        REQUIRE( m1.get_value(i) == m2.get_value(i));
    }

    // 3. Construct based on active/vals
    vector<double> vals = {5.2, 3.5, 0, -2.2, -13};
    vector<bool> actives = {false, true, false, true, false};
    Regression<double> r3 = Regression<double>(actives, vals);
    // Actives
    REQUIRE( r3.get_active(0) == false);
    REQUIRE( r3.get_active(1) == true);
    REQUIRE( r3.get_active(2) == false);
    REQUIRE( r3.get_active(3) == true);
    REQUIRE( r3.get_active(4) == false);
    // Vals
    REQUIRE( r3.get_value(0) == 5.2);
    REQUIRE( r3.get_value(1) == 3.5);
    REQUIRE( r3.get_value(2) == 0);
    REQUIRE( r3.get_value(3) == -2.2);
    REQUIRE( r3.get_value(4) == -13);

}

TEST_CASE("Regression: set_active, set_value, get_active, get_value") {

    setup_regression_ivs(6);
    
    // Setup default object
    vector<double> vals = {5.2, 3.5, 0, -2.2, -13};
    vector<bool> actives = {false, true, false, true, false};
    Regression<double> m1 = Regression<double>(actives, vals);
    // 1.1 Change active
    REQUIRE(m1.get_active(0) == false );
    REQUIRE(m1.get_active(1) == true );
    REQUIRE(m1.get_active(2) == false );
    REQUIRE(m1.get_active(3) == true );
    REQUIRE(m1.get_active(4) == false );
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    REQUIRE(m1.get_active(0) == true );
    REQUIRE(m1.get_active(1) == false );
    REQUIRE(m1.get_active(2) == true );
    REQUIRE(m1.get_active(3) == false );
    REQUIRE(m1.get_active(4) == true );
    
    // 1.2 Change values
    REQUIRE(m1.get_value(0) == 5.2 );
    REQUIRE(m1.get_value(1) == 3.5 );
    REQUIRE(m1.get_value(2) == 0 );
    REQUIRE(m1.get_value(3) == -2.2 );
    REQUIRE(m1.get_value(4) == -13 );
    m1.set_value(0, 1.1);
    m1.set_value(1, 1.2);
    m1.set_value(2, 1.3);
    m1.set_value(3, 1.4);
    m1.set_value(4, 1.5);
    REQUIRE(m1.get_value(0) == 1.1 );
    REQUIRE(m1.get_value(1) == 1.2 );
    REQUIRE(m1.get_value(2) == 1.3 );
    REQUIRE(m1.get_value(3) == 1.4 );
    REQUIRE(m1.get_value(4) == 1.5 );

}

TEST_CASE("Regression: get_count_active, get_active_positions()") {

    setup_regression_ivs(6);
    
    // Setup default object
    vector<double> vals = {5.2, 3.5, 0, -2.2, -13};
    vector<bool> actives = {false, true, false, true, false};
    Regression<double> m1 = Regression<double>(actives, vals);
    // 1.1 Check counts active
    REQUIRE(m1.get_count_active() == 2);
    REQUIRE(m1.get_active_positions().size() == 2);
    REQUIRE(m1.get_active_positions()[0] == 1);
    REQUIRE(m1.get_active_positions()[1] == 3);
    // 1.2 Change and check again
    m1.set_active(0, true);
    m1.set_active(1, false);
    m1.set_active(2, true);
    m1.set_active(3, false);
    m1.set_active(4, true);
    REQUIRE(m1.get_count_active() == 3);
    REQUIRE(m1.get_active_positions().size() == 3);
    REQUIRE(m1.get_active_positions()[0] == 0);
    REQUIRE(m1.get_active_positions()[1] == 2);
    REQUIRE(m1.get_active_positions()[2] == 4);
    // 1.3 Change to all
    m1.set_active(0, true);
    m1.set_active(1, true);
    m1.set_active(2, true);
    m1.set_active(3, true);
    m1.set_active(4, true);
    REQUIRE(m1.get_count_active() == 5);
    REQUIRE(m1.get_active_positions().size() == 5);
    REQUIRE(m1.get_active_positions()[0] == 0);
    REQUIRE(m1.get_active_positions()[1] == 1);
    REQUIRE(m1.get_active_positions()[2] == 2);
    REQUIRE(m1.get_active_positions()[3] == 3);
    REQUIRE(m1.get_active_positions()[4] == 4);
    // 1.4 Change to none
    m1.set_active(0, false);
    m1.set_active(1, false);
    m1.set_active(2, false);
    m1.set_active(3, false);
    m1.set_active(4, false);
    REQUIRE(m1.get_count_active() == 0);
    REQUIRE(m1.get_active_positions().size() == 0);

}

TEST_CASE("Regression: ==") {

    setup_regression_ivs(6);

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);

    // Setup m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    vals = {0,-3,0,4,3,-3};
    actives = {false, true, false, true, true, true};
    Regression<double> m2 = Regression<double>(actives, vals);
    
    // Setup m3 = m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vals = {1,12,2,-7,3,-20};
    actives = {true, false, true, false, true, true};
    Regression<double> m3 = Regression<double>(actives, vals);
   
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
    Regression<double> m6 = Regression<double>(6);
    Regression<double> m7 = Regression<double>(6);
    REQUIRE(m6 == m7);
    REQUIRE(m7 == m6);

}

TEST_CASE("Regression: randomise()") {

    size_t params = 6;
    setup_regression_ivs(params);

    // Seed
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);    
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // 1. Randomise variables independently
    vector<double> vals = {0,0,0,0,0,0};
    vector<bool> actives = {true, true, true, true, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);
    for(size_t i = 0; i < params; i++) {

        size_t n5 = 0, n4 = 0, n3 = 0, n2 = 0, n1 = 0, z = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
        for(size_t j = 0; j < 1000; j++) {
            m1.randomise(1, 5, i);

            int val = m1.get_value(i);
            switch(val) {
                case -5: n5++; break;
                case -4: n4++; break;
                case -3: n3++; break;
                case -2: n2++; break;
                case -1: n1++; break;
                case  0: z++;  break;
                case  1: p1++; break;
                case  2: p2++; break;
                case  3: p3++; break;
                case  4: p4++; break;
                case  5: p5++; break;
            }
        }

        REQUIRE(n5 > 50);
        REQUIRE(n4 > 50);
        REQUIRE(n3 > 50);
        REQUIRE(n2 > 50);
        REQUIRE(n1 > 50);
        REQUIRE(z == 0);
        REQUIRE(p1 > 50);
        REQUIRE(p2 > 50);
        REQUIRE(p3 > 50);
        REQUIRE(p4 > 50);
        REQUIRE(p5 > 50);
        
    }
    
    // 2. Randomise all varaibles
    size_t n5 = 0, n4 = 0, n3 = 0, n2 = 0, n1 = 0, z = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
    for(size_t i = 0; i < 1000; i++) {

        m1.randomise(1, 5);

        for(size_t j = 0; j < m1.get_count(); j++) {

            int val = m1.get_value(j);
            switch(val) {
                case -5: n5++; break;
                case -4: n4++; break;
                case -3: n3++; break;
                case -2: n2++; break;
                case -1: n1++; break;
                case  0: z++;  break;
                case  1: p1++; break;
                case  2: p2++; break;
                case  3: p3++; break;
                case  4: p4++; break;
                case  5: p5++; break;
            }
        }

    }
    REQUIRE(n5 > 50);
    REQUIRE(n4 > 50);
    REQUIRE(n3 > 50);
    REQUIRE(n2 > 50);
    REQUIRE(n1 > 50);
    REQUIRE(z == 0);
    REQUIRE(p1 > 50);
    REQUIRE(p2 > 50);
    REQUIRE(p3 > 50);
    REQUIRE(p4 > 50);
    REQUIRE(p5 > 50);
        
}

TEST_CASE("Regression: recombine()") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);

    // Setup m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    vals = {0,-3,0,4,3,-3};
    actives = {false, true, false, true, true, true};
    Regression<double> m2 = Regression<double>(actives, vals);
    
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

// @todo check overflow/underflow in evaluate()
// Test Overflow 3. below
// Test Underflow 4. below

TEST_CASE("Regression: evaluate()") {

    size_t size = 6;
    setup_regression_ivs(size);
    
    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);

    // Setup m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    vals = {0,-3,0,4,3,-3};
    actives = {false, true, false, true, true, true};
    Regression<double> m2 = Regression<double>(actives, vals);
    
    // 1 Test evaluation of integer values
    vector<double> values = {4,20,-3,15,-3};
    
    // 1.1 First at m1 = x1 + 2*x3 + 3*x5 - 20 
    // = 4 + 2*-2 + 3*-20
    REQUIRE( m1.evaluate(values) == -31 );

    // 1.2 Second at m2 = -3x2 + 4x4 + 3x5 - 3
    // = -12
    REQUIRE( m2.evaluate(values) == -12 );

    // 2. Evaluate for floats now
    values = {3.1,-43.2,-32.1,34.2,-3.1};

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

TEST_CASE("Regression: objective()") {

    Regression<double>::OBJECTIVE_NAME = "mse";
    MemeticModel<double>::OBJECTIVE = objective::mse<double>;

    // Load dataset
    string fn("test_data.csv");
    ofstream f;
    f.open(fn);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn);
    f << "y,x1,x2,x3,x4,x5" << endl;
    f << "5,10.4,335,123,356,12.4" << endl;
    f << "6,11.4,336,124,357,12.5" << endl;
    f << "7,12.4,337,125,358,12.6" << endl;
    f << "8,13.4,338,126,359,12.7" << endl;
    f.close();
    DataSet ds = DataSet(fn);
    ds.load();

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);
    vector<size_t> idx;
    REQUIRE( abs(m1.objective(&ds, idx)-74017.815) < 0.0001  );

}

TEST_CASE("Regression: local_search()") {

    Regression<double>::OBJECTIVE_NAME = "mse";
    MemeticModel<double>::OBJECTIVE = objective::mse<double>;
    MemeticModel<double>::LOCAL_SEARCH = local_search::custom_nelder_mead_redo<MemeticModel<double>>;

    // Load dataset
    string fn("test_data.csv");
    ofstream f;
    f.open(fn);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn);
    f << "y,x1,x2,x3,x4,x5" << endl;
    f << "5,10.4,335,123,356,12.4" << endl;
    f << "6,11.4,336,124,357,12.5" << endl;
    f << "7,12.4,337,125,358,12.6" << endl;
    f << "8,13.4,338,126,359,12.7" << endl;
    f.close();
    DataSet ds = DataSet(fn);
    ds.load();

    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);

    // 1.1 Check there is a reduction in error
    vector<size_t> idx;
    double new_err = m1.local_search(&ds, idx);
    REQUIRE( new_err <= 74017.815);
    // 1.2 Check the evaluation of the new model is true
    REQUIRE( m1.objective(&ds, idx) == new_err);

}

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
    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vector<double> vals = {1, 12,2,-7,3,-20};
    vector<bool> actives = {true, false, true, false, true, true};
    Regression<double> m1 = Regression<double>(actives, vals);
    stringstream ss1;
    ss1 << m1;
    REQUIRE(ss1.str() == "B2+2*D2+3*F2-20");

    // 2.2 m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    //     m2 =        -3*C2+4*E2+3*F2-3
    // Setup m2 = f(x) = -3x2 + 4x4 + 3x5 - 3
    vals = {0,-3,0,4,3,-3};
    actives = {false, true, false, true, true, true};
    Regression<double> m2 = Regression<double>(actives, vals);
    stringstream ss2;
    ss2 << m2;
    REQUIRE(ss2.str() == "-3*C2+4*E2+3*F2-3");

    // 2.3 -1, 1, 0 values m3 = -B2-F2-1
    // Setup m1 = f(x) = x1 + 2x3 + 3x5 - 20
    vals = {-1, 0, 0, 1, -1, -1};
    actives = {true, false, true, false, true, true};
    Regression<double> m3 = Regression<double>(actives, vals);
    stringstream ss3;
    ss3 << m3;
    REQUIRE(ss3.str() == "-B2-F2-1");

}

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
