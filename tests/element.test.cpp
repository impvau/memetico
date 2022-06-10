#include "doctest.h"
#include <memetico/model_base/element.h>

TEST_CASE("Element: Construct<double>, set_active, set_value, get_active, get_value") {

    Element<double>* e = new Element<double>(true, 4.6);
    REQUIRE(e->get_active() == true);
    REQUIRE(e->get_value() == 4.6);
    delete e;

    e = new Element(false, 2.3);
    REQUIRE(e->get_active() == false);
    REQUIRE(e->get_value() == 2.3);
    delete e;

    e = new Element(false, -2.2);
    REQUIRE(e->get_active() == false);
    REQUIRE(e->get_value() == -2.2);
    delete e;

}

TEST_CASE("Element: Construct<int>, set_active, set_value, get_active, get_value") {

    Element<double>* e = new Element<double>(true, 3);
    REQUIRE(e->get_active() == true);
    REQUIRE(e->get_value() == 3);
    delete e;

    e = new Element<double>(false, 2);
    REQUIRE(e->get_active() == false);
    REQUIRE(e->get_value() == 2);
    delete e;

    e = new Element<double>(false, -1);
    REQUIRE(e->get_active() == false);
    REQUIRE(e->get_value() == -1);
    delete e;

}