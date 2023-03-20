
#include "doctest.h"
#include <memetico/data/data_set.h>
#include <sstream>
#include <fstream>

TEST_CASE("DataSet: ") {

    // Tests
    // 1. Create DataSet given file
    string file_name = "test_data.csv";
    DataSet ds = DataSet(file_name);
    REQUIRE(ds.get_file() == file_name);

}

TEST_CASE("load() ") {

    // Tests
    // 1. Load dataset that doesnt exist
    // 2. Load basic datset
    // 3. Load dataset with uncertainty, weights etc.
    // Todo
    // Special character issues in file and filename
    // Load dataset missing y
    // Load dataset missing varaibles

    // 1. Load dataset that doesnt exist
    string fn1("Non-existing-file.csv");
    DataSet ds1 = DataSet(fn1);
    try {
        ds1.load();
    } catch (const exception& ex) {
        stringstream ss; 
        ss << ex.what();
        REQUIRE(ss.str() == "Unable to open file "+ fn1);
        // Weird -- need to reset errno else some other test in a random part of the application
        // throws an error with "No such file or directory"
        errno = 0;
    }

    // 2. Load basic datset
    string fn2("test_data.csv");
    ofstream f;
    f.open(fn2);
    if (!f.is_open())
        throw runtime_error("Unable to open file "+ fn2);
    f << "y,x1,x2,x3,x4,x5,x6" << endl;
    f << "5,10.4,335,123,356,12.4,16.2" << endl;
    f << "6,11.4,336,124,357,12.5,16.3" << endl;
    f.close();
    DataSet ds = DataSet(fn2);
    //
    ds.load();
    // 2.1 Check basic values
    REQUIRE( ds.get_file() == "test_data.csv" );
    REQUIRE( !ds.has_uncertainty() );
    REQUIRE( !ds.has_weight() );
    REQUIRE( ds.get_count() == 2 );
    REQUIRE( ds.weight.size() == 0 );
    REQUIRE( ds.dy.size() == 0 );
    REQUIRE( ds.y.size() == 2 );
    REQUIRE( ds.y[0] == 5 );
    REQUIRE( ds.samples.size() == 2 );
    REQUIRE( ds.samples[0].size() == 6 );
    // 2.2 Check saved samples
    REQUIRE( float(ds.samples[0][0]) == float(10.4) );
    REQUIRE( float(ds.samples[0][1]) == float(335) );
    REQUIRE( float(ds.samples[0][2]) == float(123) );
    REQUIRE( float(ds.samples[0][3]) == float(356) );
    REQUIRE( float(ds.samples[0][4]) == float(12.4) );
    REQUIRE( float(ds.samples[0][5]) == float(16.2) );
    REQUIRE( float(ds.samples[1][0]) == float(11.4) );
    REQUIRE( float(ds.samples[1][1]) == float(336) );
    REQUIRE( float(ds.samples[1][2]) == float(124) );
    REQUIRE( float(ds.samples[1][3]) == float(357) );
    REQUIRE( float(ds.samples[1][4]) == float(12.5) );
    REQUIRE( float(ds.samples[1][5]) == float(16.3) );
    // 2.3 Check varaibles 
    REQUIRE( DataSet::IVS.size() == 6 );
    REQUIRE( DataSet::IVS[0] == "x1" );
    REQUIRE( DataSet::IVS[1] == "x2" );
    REQUIRE( DataSet::IVS[2] == "x3" );
    REQUIRE( DataSet::IVS[3] == "x4" );
    REQUIRE( DataSet::IVS[4] == "x5" );
    REQUIRE( DataSet::IVS[5] == "x6" );
    
    // 3. Load dataset with uncertainty, weights etc.
    string fn3("test_data.csv");
    ofstream f3;
    f3.open(fn3);
    if (!f3.is_open())
        throw runtime_error("Unable to open file "+ fn3);
    f3 << "y,x1,x2,x3,x4,x5,x6,dy,w" << endl;
    f3 << "5,10.4,335,123,356,12.4,16.2,0.1,1.1" << endl;
    f3 << "6,11.4,336,124,357,12.5,16.3,0.2,1.2" << endl;
    f3 << "7,12.4,337,125,358,12.6,16.4,0.3,1.3" << endl;
    f3 << "8,13.4,338,126,359,12.7,16.5,0.4,1.4" << endl;
    f3.close();
    DataSet ds3 = DataSet(fn3);
    //
    ds3.load();
    // 3.1 Check basic values
    REQUIRE( ds3.get_file() == "test_data.csv" );
    REQUIRE( ds3.has_uncertainty() );
    REQUIRE( ds3.has_weight() );
    REQUIRE( ds3.get_count() == 4 );
    // Weight
    REQUIRE( ds3.weight.size() == 4 );
    REQUIRE( float(ds3.weight[0]) == float(1.1));
    REQUIRE( float(ds3.weight[1]) == float(1.2));
    REQUIRE( float(ds3.weight[2]) == float(1.3));
    REQUIRE( float(ds3.weight[3]) == float(1.4));
    // Uncertainty
    REQUIRE( ds3.dy.size() == 4 );
    REQUIRE( float(ds3.dy[0]) == float(0.1));
    REQUIRE( float(ds3.dy[1]) == float(0.2));
    REQUIRE( float(ds3.dy[2]) == float(0.3));
    REQUIRE( float(ds3.dy[3]) == float(0.4));
    // 3.1 Check target
    REQUIRE( ds3.y.size() == 4 );
    REQUIRE( float(ds3.y[0]) == float(5));
    REQUIRE( float(ds3.y[1]) == float(6));
    REQUIRE( float(ds3.y[2]) == float(7));
    REQUIRE( float(ds3.y[3]) == float(8));
    // 3.2 Check saved samples
    REQUIRE( ds3.samples.size() == 4 );
    REQUIRE( ds3.samples[0].size() == 6 );
    REQUIRE( float(ds3.samples[0][0]) == float(10.4) );
    REQUIRE( float(ds3.samples[0][1]) == float(335) );
    REQUIRE( float(ds3.samples[0][2]) == float(123) );
    REQUIRE( float(ds3.samples[0][3]) == float(356) );
    REQUIRE( float(ds3.samples[0][4]) == float(12.4) );
    REQUIRE( float(ds3.samples[0][5]) == float(16.2) );
    REQUIRE( float(ds3.samples[1][0]) == float(11.4) );
    REQUIRE( float(ds3.samples[1][1]) == float(336) );
    REQUIRE( float(ds3.samples[1][2]) == float(124) );
    REQUIRE( float(ds3.samples[1][3]) == float(357) );
    REQUIRE( float(ds3.samples[1][4]) == float(12.5) );
    REQUIRE( float(ds3.samples[1][5]) == float(16.3) );
    REQUIRE( float(ds3.samples[2][0]) == float(12.4) );
    REQUIRE( float(ds3.samples[2][1]) == float(337) );
    REQUIRE( float(ds3.samples[2][2]) == float(125) );
    REQUIRE( float(ds3.samples[2][3]) == float(358) );
    REQUIRE( float(ds3.samples[2][4]) == float(12.6) );
    REQUIRE( float(ds3.samples[2][5]) == float(16.4) );
    REQUIRE( float(ds3.samples[3][0]) == float(13.4) );
    REQUIRE( float(ds3.samples[3][1]) == float(338) );
    REQUIRE( float(ds3.samples[3][2]) == float(126) );
    REQUIRE( float(ds3.samples[3][3]) == float(359) );
    REQUIRE( float(ds3.samples[3][4]) == float(12.7) );
    REQUIRE( float(ds3.samples[3][5]) == float(16.5) );
    REQUIRE( DataSet::IVS.size() == 6 );
    REQUIRE( DataSet::IVS[0] == "x1" );
    REQUIRE( DataSet::IVS[1] == "x2" );
    REQUIRE( DataSet::IVS[2] == "x3" );
    REQUIRE( DataSet::IVS[3] == "x4" );
    REQUIRE( DataSet::IVS[4] == "x5" );
    REQUIRE( DataSet::IVS[5] == "x6" );

}