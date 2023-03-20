#include "doctest.h"
#include <memetico/model_base/element.h>
#include <sstream>

TEST_CASE("Element: Element<double>(), get_active, get_value") {

    // 1. Active float element
    Element<double> o1 = Element<double>(true, 4.6);
    REQUIRE(o1.get_active() == true);
    REQUIRE(o1.get_value() == 4.6);

    // 2. Inactive float element
    Element<double> o2 = Element<double>(false, 2.3);
    REQUIRE(o2.get_active() == false);
    REQUIRE(o2.get_value() == 2.3);

    // 3. Negative float element
    Element<double> o3 = Element<double>(false, -2.2);
    REQUIRE(o3.get_active() == false);
    REQUIRE(o3.get_value() == -2.2);

    // 4. Empty element
    Element<double> o4 = Element<double>();
    REQUIRE(o4.get_active() == false);
    REQUIRE(o4.get_value() == 0);

}

TEST_CASE("Element: set_value, set_active") {

    // 1. Active float element
    Element<double> o1 = Element<double>(true, 4.6);
    // 1.1 Set active
    o1.set_active(false);
    REQUIRE(o1.get_active() == false);
    // 1.2 Set value
    double newVal = 7.9;
    o1.set_value(newVal);
    REQUIRE(o1.get_value() == 7.9);

}

TEST_CASE("Element: Element<double>( &Element<double> )") {

    // 1. Active float element
    Element<double> o1 = Element<double>(true, 4.6);
    Element<double> oCopy = Element<double>(o1);
    // 1.1 Copy has same value
    REQUIRE(oCopy.get_active() == true);
    REQUIRE(oCopy.get_value() == 4.6);
    // 1.2 Assigning a new value changes the copy, not the original
    double newVal = 5.8;
    oCopy.set_value(newVal);
    REQUIRE(o1.get_value() == 4.6);
    REQUIRE(oCopy.get_value() == 5.8);
    
}

TEST_CASE("Element: Element<int>(), set_active, set_value, get_active, get_value") {

    // 1. Active int element
    Element<int> o1 = Element<int>(true, 3);
    REQUIRE(o1.get_active() == true);
    REQUIRE(o1.get_value() == 3);

    // 2. Inactive int element
    Element<int> o2 = Element<int>(false, 2);
    REQUIRE(o2.get_active() == false);
    REQUIRE(o2.get_value() == 2);

    // 3. Negative int element
    Element<int> o3 = Element<int>(false, -1);
    REQUIRE(o3.get_active() == false);
    REQUIRE(o3.get_value() == -1);

}

TEST_CASE("Element: ==, !=") {

    Element<double> o1 = Element<double>(true, 4.6);
    Element<double> o2 = Element(false, 2.3);

    // 1. 1st object == itself
    REQUIRE(o1 == o1);

    // 2. 2nd object == itself
    REQUIRE(o2 == o2);

    // 3. 1st != 2nd
    REQUIRE(!(o1 == o2));

    // 4. 2nd != 1st
    REQUIRE(!(o2 == o1));

    // 5. 1st != 1st
    REQUIRE(!(o1 != o1));

    // 6. 2nd != 2nd
    REQUIRE(!(o2 != o2));

    // 7. 1st != 2nd
    REQUIRE(o1 != o2);

    // 8. 2nd != 1st
    REQUIRE(o2 != o1);

}

TEST_CASE("Element: <<") {

    Element<double> o1 = Element<double>(true, 4.6);
    Element<double> o2 = Element(false, 2.3);
    Element<double> o3 = Element(true, -2.2);  

    // Active float
    stringstream ss1; 
    ss1 << o1;
    REQUIRE(ss1.str() == "4.6");

    // Inactive float
    stringstream ss2; 
    ss2 << o2;
    REQUIRE(ss2.str() == "");

    // Negative float
    stringstream ss3; 
    ss3 << o3;
    REQUIRE(ss3.str() == "-2.2");

}