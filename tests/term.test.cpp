#include "doctest.h"
#include <memetico/globals.h>
#include <forms/term.h>
#include <typeinfo>

TEST_CASE("Term<double>, get_elem, get_count(), set_elem(), ==") {

    size_t count = 5;

    Term<double>* t = new Term<double>(count);
    Element<double>* e1 = new Element<double>(true, 4.6);
    Element<double>* e2 = new Element<double>(false, 4.0);
    Element<double>* e3 = new Element<double>(true, 4.2);
    Element<double>* e4 = new Element<double>(false, 4.3);
    Element<double>* e5 = new Element<double>(true, 4.1);
    t->set_elem(0, e1);
    t->set_elem(1, e2);
    t->set_elem(2, e3);
    t->set_elem(3, e4);
    t->set_elem(4, e5);

    REQUIRE( t->get_count() == count);
    REQUIRE( t->get_elem(0) == e1 );
    REQUIRE( t->get_elem(1) == e2 );
    REQUIRE( t->get_elem(2) == e3 );
    REQUIRE( t->get_elem(3) == e4 );
    REQUIRE( t->get_elem(4) == e5 );

    delete t;

}