#include "doctest.h"
#include <memetico/model_base/additive_term.h>
#include <sstream>

TEST_CASE("AdditiveTerm: AdditiveTerm<double>(), get_elem(), get_count()") {

    // 1. Zero element
    AdditiveTerm<double> o1 = AdditiveTerm<double>();
    REQUIRE(o1.get_count() == 0);

    // 2. Multi element
    AdditiveTerm<double> o2 = AdditiveTerm<double>(5);
    Element<double> empty = Element<double>();
    // 2.1 Element count correct
    REQUIRE(o2.get_count() == 5);
    // 2.2 All elements are initialised empty
    for( size_t i = 0; i < o2.get_count(); i++) {
        Element<double> &e1 = o2.get_elem(i);
        REQUIRE(e1 == empty);
    }
    
}

TEST_CASE("AdditiveTerm: set_value, set_active, get_value, get_active") {

    // 1. Set multiple elements value
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
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
    // 1.1 Ensure all values match new values
    REQUIRE(o1.get_value(0) == d0);
    REQUIRE(o1.get_value(1) == d1);
    REQUIRE(o1.get_value(2) == d2);
    REQUIRE(o1.get_value(3) == d3);
    REQUIRE(o1.get_value(4) == d4);

    o1.set_active(0, false);
    o1.set_active(1, true);
    o1.set_active(2, false);
    o1.set_active(3, true);
    o1.set_active(4, false);
    // 1.1 Ensure all values match new values
    REQUIRE(o1.get_active(0) == false);
    REQUIRE(o1.get_active(1) == true);
    REQUIRE(o1.get_active(2) == false);
    REQUIRE(o1.get_active(3) == true);
    REQUIRE(o1.get_active(4) == false);

}

TEST_CASE("AdditiveTerm: AdditiveTerm<double>( &AdditiveTerm<double> )") {

    // 1. Construct from multi element object
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
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
    o1.set_active(0, false);
    o1.set_active(1, true);
    o1.set_active(2, false);
    o1.set_active(3, true);
    o1.set_active(4, false);
    AdditiveTerm<double> o2 = AdditiveTerm<double>(o1);
    REQUIRE(o2.get_value(0) == d0);
    REQUIRE(o2.get_value(1) == d1);
    REQUIRE(o2.get_value(2) == d2);
    REQUIRE(o2.get_value(3) == d3);
    REQUIRE(o2.get_value(4) == d4);
    REQUIRE(o2.get_active(0) == false);
    REQUIRE(o2.get_active(1) == true);
    REQUIRE(o2.get_active(2) == false);
    REQUIRE(o2.get_active(3) == true);
    REQUIRE(o2.get_active(4) == false);

    // 1.1. Update a value in o2, confirm != o1
    o2.set_value(0, d4);
    REQUIRE(o1.get_value(0) == d0);
    REQUIRE(o2.get_value(0) == d4);
    
}

TEST_CASE("AdditiveTerm: get_elem, set_elem") {

    // 1. Set multiple elements value
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
    Element<double> d0 = Element<double>(false, 5.2);
    Element<double> d1 = Element<double>(true, 3.5);
    Element<double> d2 = Element<double>(false, 0);
    Element<double> d3 = Element<double>(true, -2.2);
    Element<double> d4 = Element<double>(false, -13);
    o1.set_elem(0, d0);
    o1.set_elem(1, d1);
    o1.set_elem(2, d2);
    o1.set_elem(3, d3);
    o1.set_elem(4, d4);
    // 1.1 Ensure all values match new values
    REQUIRE(o1.get_elem(0) == d0);
    REQUIRE(o1.get_elem(1) == d1);
    REQUIRE(o1.get_elem(2) == d2);
    REQUIRE(o1.get_elem(3) == d3);
    REQUIRE(o1.get_elem(4) == d4);
    // 1.2 Ensure that when we change the local Element it does impact the AdditiveTerm
    double temp = 6.0;
    d0.set_value(temp);
    REQUIRE(o1.get_elem(0) != d0);

    // 2. Ensure we can retrive the element and modify it if we wish
    o1.get_elem(0).set_value(temp);
    REQUIRE( o1.get_elem(0).get_value() == temp );

}

TEST_CASE("AdditiveTerm: get_count_active, get_active_positions") {

    // 1. Test increasing number of active variables
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
    // 1.1 Defaul of 0 active
    REQUIRE(o1.get_count_active() == 0);
    REQUIRE(o1.get_active_positions().size() == 0);
    Element<double> d0 = Element<double>(false, 5.2);
    Element<double> d1 = Element<double>(true, 3.5);
    Element<double> d2 = Element<double>(false, 0);
    Element<double> d3 = Element<double>(true, -2.2);
    Element<double> d4 = Element<double>(false, -13);
    o1.set_elem(0, d0);
    o1.set_elem(1, d1);
    // 1.2 After setting 1 active
    REQUIRE(o1.get_count_active() == 1);
    REQUIRE(o1.get_active_positions().size() == 1);
    REQUIRE(o1.get_active_positions()[0] == 1);
    o1.set_elem(2, d2);
    o1.set_elem(3, d3);
    // 1.3 After setting 2 active
    REQUIRE(o1.get_count_active() == 2);
    REQUIRE(o1.get_active_positions().size() == 2);
    REQUIRE(o1.get_active_positions()[0] == 1);
    REQUIRE(o1.get_active_positions()[1] == 3);
    o1.set_elem(4, d4);
    // 1.3 After setting 3 active
    o1.set_elem(0, d3);
    REQUIRE(o1.get_count_active() == 3);
    REQUIRE(o1.get_active_positions().size() == 3);
    REQUIRE(o1.get_active_positions()[0] == 0);
    REQUIRE(o1.get_active_positions()[1] == 1);
    REQUIRE(o1.get_active_positions()[2] == 3);
    // 1.4 After setting 4 active
    o1.set_elem(2, d3);
    REQUIRE(o1.get_count_active() == 4);
    REQUIRE(o1.get_active_positions().size() == 4);
    REQUIRE(o1.get_active_positions()[0] == 0);
    REQUIRE(o1.get_active_positions()[1] == 1);
    REQUIRE(o1.get_active_positions()[2] == 2);
    REQUIRE(o1.get_active_positions()[3] == 3);
    // 1.5 After setting 5 active
    o1.set_elem(4, d3);
    REQUIRE(o1.get_count_active() == 5);
    REQUIRE(o1.get_active_positions().size() == 5);
    REQUIRE(o1.get_active_positions()[0] == 0);
    REQUIRE(o1.get_active_positions()[1] == 1);
    REQUIRE(o1.get_active_positions()[2] == 2);
    REQUIRE(o1.get_active_positions()[3] == 3);
    REQUIRE(o1.get_active_positions()[4] == 4);

}

TEST_CASE("AdditiveTerm: == ") {

    // 1. Test object == self
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
    // 1.1 Check equality with empty elements
    REQUIRE(o1 == o1);
    Element<double> d0 = Element<double>(false, 5.2);
    Element<double> d1 = Element<double>(true, 3.5);
    Element<double> d2 = Element<double>(false, 0);
    Element<double> d3 = Element<double>(true, -2.2);
    Element<double> d4 = Element<double>(false, -13);
    o1.set_elem(0, d0);
    o1.set_elem(1, d1);
    o1.set_elem(2, d2);
    o1.set_elem(3, d3);
    o1.set_elem(4, d4);
    // 1.1 Check equality after assigning elements
    REQUIRE(o1 == o1);

    // 2. Empty object == self
    AdditiveTerm<double> o2 = AdditiveTerm<double>(0);
    REQUIRE(o2 == o2);

    // 3. Test two different objects with same data
    AdditiveTerm<double> o3 = AdditiveTerm<double>(5);
    o3.set_elem(0, d0);
    o3.set_elem(1, d1);
    o3.set_elem(2, d2);
    o3.set_elem(3, d3);
    o3.set_elem(4, d4);
    REQUIRE(o1 == o3);

    // 4. Change each element slightly 
    // 4.1 Elem 0
    double temp = 5.1;
    o3.set_value(0, temp);
    REQUIRE(!(o1 == o3));
    o3.set_elem(0, d0);
    REQUIRE(o1 == o3);
    // 4.2 Elem 1
    o3.set_value(1, temp);
    REQUIRE(!(o1 == o3));
    o3.set_elem(1, d1);
    REQUIRE(o1 == o3);
    // 4.3 Elem 2
    o3.set_value(2, temp);
    REQUIRE(!(o1 == o3));
    o3.set_elem(2, d2);
    REQUIRE(o1 == o3);
    // 4.4 Elem 3
    o3.set_value(3, temp);
    REQUIRE(!(o1 == o3));
    o3.set_elem(3, d3);
    REQUIRE(o1 == o3);
    // 4.5 Elem 4
    o3.set_value(4, temp);
    REQUIRE(!(o1 == o3));
    o3.set_elem(4, d4);
    REQUIRE(o1 == o3);

}

TEST_CASE("AdditiveTerm: << ") {

    // 1. Test object == self
    AdditiveTerm<double> o1 = AdditiveTerm<double>(5);
    // 1.1 Check equality with empty elements
    REQUIRE(o1 == o1);
    Element<double> d0 = Element<double>(true, 5.2);
    Element<double> d1 = Element<double>(false, 3.5);
    Element<double> d2 = Element<double>(true, 0);
    Element<double> d3 = Element<double>(true, -2.2);
    Element<double> d4 = Element<double>(true, -13);
    o1.set_elem(0, d0);
    o1.set_elem(1, d1);
    o1.set_elem(2, d2);
    o1.set_elem(3, d3);
    o1.set_elem(4, d4);
    // 1.1 Check equality after assigning elements
    stringstream ss1;
    ss1 << o1;
    REQUIRE(ss1.str() == "5.2*x0-2.2*x3-13");
    // 1.2 Inactivate constant term
    o1.set_active(4, false);
    stringstream ss2;
    ss2 << o1;
    REQUIRE(ss2.str() == "5.2*x0-2.2*x3");
    // 1.3 Inactivate first term
    o1.set_active(0, false);
    stringstream ss3;
    ss3 << o1;
    REQUIRE(ss3.str() == "-2.2*x3");
    // 1.4 Inactivate third term
    o1.set_active(3, false);
    stringstream ss4;
    ss4 << o1;
    REQUIRE(ss4.str() == "");
    // 1.4 Inactivate zero 2nd term
    o1.set_active(2, false);
    stringstream ss5;
    ss5 << o1;
    REQUIRE(ss5.str() == "");

}
