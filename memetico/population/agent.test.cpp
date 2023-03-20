
#include "doctest.h"
#include <memetico/helpers/rng.h>
#include <memetico/population/agent.h>
#include <memetico/models/cont_frac.h>
#include <memetico/models/regression.h>
#include <sstream>
#include <stdexcept>

typedef ContinuedFraction<double> AgentModel;

inline AgentModel frac_1() {

    // f(x) = x1 + 2x3 + 3x5 - 20 
    size_t params = 6;
    size_t depth = 0;
    ContinuedFraction<double> o  = ContinuedFraction<double>(depth);
    Regression<double> m1 = Regression<double>(params);
    o.set_global_active(0, true);
    o.set_global_active(1, false);
    o.set_global_active(2, true);
    o.set_global_active(3, false);
    o.set_global_active(4, true);
    o.set_global_active(5, true);
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
    o.set_terms(0,m1);
    return o;
}

inline AgentModel frac_2() {

    // f(x) = -3x2 + 4x4 + 3x5 - 3
    size_t params = 6;
    size_t depth = 0;
    ContinuedFraction<double> o  = ContinuedFraction<double>(depth);
    Regression<double> m2 = Regression<double>(params);
    o.set_global_active(0, false);
    o.set_global_active(1, true);
    o.set_global_active(2, false);
    o.set_global_active(3, true);
    o.set_global_active(4, true);
    o.set_global_active(5, true);
    double d0 = 0;
    double d1 = -3;
    double d2 = 0;
    double d3 = 4;
    double d4 = 3;
    double d5 = -3;
    m2.set_active(0, false);
    m2.set_active(1, true);
    m2.set_active(2, false);
    m2.set_active(3, true);
    m2.set_active(4, true);
    m2.set_active(5, true);
    m2.set_value(0, d0);
    m2.set_value(1, d1);
    m2.set_value(2, d2);
    m2.set_value(3, d3);
    m2.set_value(4, d4);
    m2.set_value(5, d5);
    o.set_terms(0, m2);
    return o;
}

TEST_CASE("Agent: Agent<T>, get_depth, get_number, get_children, get_parent, is_leaf") {

    // Test
    // 1. Construct without MAX_DEPTH set 
    // 2. Construct without DEGREE set
    // 3. Construct MAX_DEPTH 0, degree 1
    // 4. Construct MAX_DEPTH 0, degree 3
    // 5. Construct MAX_DEPTH 0, degree 6
    // 6. Construct MAX_DEPTH 1, degree 1
    // 7. Construct MAX_DEPTH 1, degree 3
    // 8. Construct MAX_DEPTH 1, degree 6
    // 9. Construct MAX_DEPTH 2, degree 3
    // To do later
    // 10. Construct MAX_DEPTH 3, degree 3
    // 11. Construct MAX_DEPTH 4, degree 3

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    
    // 1. Not possible size_t varaible
    // 2. Construct without DEGREE set
    Agent<AgentModel>::DEGREE = 0;
    try {
        Agent<AgentModel> a = Agent<AgentModel>();
    } catch (const exception& ex) {
        stringstream ss; 
        ss << ex.what();
        REQUIRE(ss.str() == "Agent::DEGREE is not set before constructing agent" );
    }

    // 3. Construct MAX_DEPTH 0, degree 1
    Agent<AgentModel>::DEGREE = 1;
    Agent<AgentModel>::MAX_DEPTH = 0;
    Agent<AgentModel> a1 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a1.get_children().size() == 0);
    REQUIRE( a1.get_parent() == nullptr);
    REQUIRE( a1.get_depth() == 0 );
    REQUIRE( a1.get_number() == 0 );
    REQUIRE( a1.is_leaf() );
    // 3.1 Ensure pocket and current are set
    REQUIRE( a1.get_members().size() == 2 );
    REQUIRE( a1.get_current().get_count_active() > 0 );
    REQUIRE( a1.get_pocket().get_count_active() > 0 );
    // 3.2 Ensure that pocket is unique from current
    REQUIRE( !(a1.get_current() == a1.get_pocket()) );   

    // 4. Construct MAX_DEPTH 0, degree 3 - should not change anything as no children
    Agent<AgentModel>::DEGREE = 3;
    Agent<AgentModel>::MAX_DEPTH = 0;
    Agent<AgentModel> a2 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a2.get_children().size() == 0);
    REQUIRE( a2.get_parent() == nullptr);
    REQUIRE( a2.get_depth() == 0 );
    REQUIRE( a2.get_number() == 0 );
    REQUIRE( a2.is_leaf() );
    REQUIRE( a2.get_members().size() == 2 );
    REQUIRE( a2.get_current().get_count_active() > 0 );
    REQUIRE( a2.get_pocket().get_count_active() > 0 );
    REQUIRE( !(a2.get_current() == a2.get_pocket()) );

    // 5. Construct MAX_DEPTH 0, degree 6 - should not change anything as no children
    Agent<AgentModel>::DEGREE = 6;
    Agent<AgentModel>::MAX_DEPTH = 0;
    Agent<AgentModel> a3 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a3.get_children().size() == 0);
    REQUIRE( a3.get_parent() == nullptr);
    REQUIRE( a3.get_depth() == 0 );
    REQUIRE( a3.get_number() == 0 );
    REQUIRE( a3.is_leaf() );
    REQUIRE( a3.get_members().size() == 2 );
    REQUIRE( a3.get_current().get_count_active() > 0 );
    REQUIRE( a3.get_pocket().get_count_active() > 0 );
    REQUIRE( !(a3.get_current() == a3.get_pocket()) );
    
    // 6. Construct MAX_DEPTH 1, degree 1
    Agent<AgentModel>::DEGREE = 1;
    Agent<AgentModel>::MAX_DEPTH = 1;
    //Agent<AgentModel> a = Agent<AgentModel>(0, 0, 0);
    Agent<AgentModel> a4 = Agent<AgentModel>(0, 0, 0);
    // 6.1 Expect 1 child, still no parent
    REQUIRE( a4.get_children().size() == 1);
    REQUIRE( a4.get_parent() == nullptr);
    REQUIRE( a4.get_depth() == 0 );
    REQUIRE( a4.get_number() == 0 );
    REQUIRE( !a4.is_leaf() );
    // 6.2 Verify child
    REQUIRE( a4.get_children()[0]->get_children().size() == 0);
    REQUIRE( a4.get_children()[0]->get_parent() == &a4);
    REQUIRE( a4.get_children()[0]->get_depth() == 1 );
    REQUIRE( a4.get_children()[0]->get_number() == 1 );
    REQUIRE( a4.get_children()[0]->is_leaf() );
    REQUIRE( a4.get_children()[0]->get_members().size() == 2 );
    REQUIRE( a4.get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a4.get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a4.get_children()[0]->get_current() == a4.get_children()[0]->get_pocket()) );
    // 6.3 Verify child parent
    REQUIRE( a4.get_children()[0]->get_parent()->get_children().size() == 1);
    REQUIRE( a4.get_children()[0]->get_parent()->get_parent() == nullptr);
    REQUIRE( a4.get_children()[0]->get_parent()->get_depth() == 0 );
    REQUIRE( a4.get_children()[0]->get_parent()->get_number() == 0 );
    REQUIRE( !a4.get_children()[0]->get_parent()->is_leaf() );

    // 7. Construct MAX_DEPTH 1, degree 3
    Agent<AgentModel>::DEGREE = 3;
    Agent<AgentModel>::MAX_DEPTH = 1;
    Agent<AgentModel> a5 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a5.get_children().size() == 3);
    REQUIRE( a5.get_parent() == nullptr);
    REQUIRE( a5.get_depth() == 0 );
    REQUIRE( a5.get_number() == 0 );
    REQUIRE( !a5.is_leaf() );
    // 7.1 Verify children
    REQUIRE( a5.get_children()[0]->get_children().size() == 0);
    REQUIRE( a5.get_children()[0]->get_parent() == &a5);
    REQUIRE( a5.get_children()[0]->get_depth() == 1 );
    REQUIRE( a5.get_children()[0]->get_number() == 1 );
    REQUIRE( a5.get_children()[0]->is_leaf() );
    REQUIRE( a5.get_children()[0]->get_members().size() == 2 );
    REQUIRE( a5.get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a5.get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a5.get_children()[0]->get_current() == a5.get_children()[0]->get_pocket()) );
    REQUIRE( a5.get_children()[1]->get_children().size() == 0);
    REQUIRE( a5.get_children()[1]->get_parent() == &a5);
    REQUIRE( a5.get_children()[1]->get_depth() == 1 );
    REQUIRE( a5.get_children()[1]->get_number() == 2 );
    REQUIRE( a5.get_children()[1]->is_leaf() );
    REQUIRE( a5.get_children()[1]->get_members().size() == 2 );
    REQUIRE( a5.get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a5.get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a5.get_children()[1]->get_current() == a5.get_children()[1]->get_pocket()) );
    REQUIRE( a5.get_children()[2]->get_children().size() == 0);
    REQUIRE( a5.get_children()[2]->get_parent() == &a5);
    REQUIRE( a5.get_children()[2]->get_depth() == 1 );
    REQUIRE( a5.get_children()[2]->get_number() == 3 );
    REQUIRE( a5.get_children()[2]->is_leaf() );
    REQUIRE( a5.get_children()[2]->get_members().size() == 2 );
    REQUIRE( a5.get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a5.get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a5.get_children()[2]->get_current() == a5.get_children()[2]->get_pocket()) );

    // 8. Construct MAX_DEPTH 1, degree 6
    Agent<AgentModel>::DEGREE = 6;
    Agent<AgentModel>::MAX_DEPTH = 1;
    Agent<AgentModel> a6 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a6.get_children().size() == 6);
    REQUIRE( a6.get_parent() == nullptr);
    REQUIRE( a6.get_depth() == 0 );
    REQUIRE( a6.get_number() == 0 );
    REQUIRE( !a6.is_leaf() );
    // 8.1 Verify children
    REQUIRE( a6.get_children()[0]->get_children().size() == 0);
    REQUIRE( a6.get_children()[0]->get_parent() == &a6);
    REQUIRE( a6.get_children()[0]->get_depth() == 1 );
    REQUIRE( a6.get_children()[0]->get_number() == 1 );
    REQUIRE( a6.get_children()[0]->is_leaf() );
    REQUIRE( a6.get_children()[0]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[0]->get_current() == a6.get_children()[0]->get_pocket()) );
    REQUIRE( a6.get_children()[1]->get_children().size() == 0);
    REQUIRE( a6.get_children()[1]->get_parent() == &a6);
    REQUIRE( a6.get_children()[1]->get_depth() == 1 );
    REQUIRE( a6.get_children()[1]->get_number() == 2 );
    REQUIRE( a6.get_children()[1]->is_leaf() );
    REQUIRE( a6.get_children()[1]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[1]->get_current() == a6.get_children()[1]->get_pocket()) );
    REQUIRE( a6.get_children()[2]->get_children().size() == 0);
    REQUIRE( a6.get_children()[2]->get_parent() == &a6);
    REQUIRE( a6.get_children()[2]->get_depth() == 1 );
    REQUIRE( a6.get_children()[2]->get_number() == 3 );
    REQUIRE( a6.get_children()[2]->is_leaf() );
    REQUIRE( a6.get_children()[2]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[2]->get_current() == a6.get_children()[2]->get_pocket()) );
    REQUIRE( a6.get_children()[3]->get_children().size() == 0);
    REQUIRE( a6.get_children()[3]->get_parent() == &a6);
    REQUIRE( a6.get_children()[3]->get_depth() == 1 );
    REQUIRE( a6.get_children()[3]->get_number() == 4 );
    REQUIRE( a6.get_children()[3]->is_leaf() );
    REQUIRE( a6.get_children()[3]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[3]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[3]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[3]->get_current() == a6.get_children()[3]->get_pocket()) );
    REQUIRE( a6.get_children()[4]->get_children().size() == 0);
    REQUIRE( a6.get_children()[4]->get_parent() == &a6);
    REQUIRE( a6.get_children()[4]->get_depth() == 1 );
    REQUIRE( a6.get_children()[4]->get_number() == 5 );
    REQUIRE( a6.get_children()[4]->is_leaf() );
    REQUIRE( a6.get_children()[4]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[4]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[4]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[4]->get_current() == a6.get_children()[4]->get_pocket()) );
    REQUIRE( a6.get_children()[5]->get_children().size() == 0);
    REQUIRE( a6.get_children()[5]->get_parent() == &a6);
    REQUIRE( a6.get_children()[5]->get_depth() == 1 );
    REQUIRE( a6.get_children()[5]->get_number() == 6 );
    REQUIRE( a6.get_children()[5]->is_leaf() );
    REQUIRE( a6.get_children()[5]->get_members().size() == 2 );
    REQUIRE( a6.get_children()[5]->get_current().get_count_active() > 0 );
    REQUIRE( a6.get_children()[5]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a6.get_children()[5]->get_current() == a6.get_children()[5]->get_pocket()) );

    // 9. Construct MAX_DEPTH 2, degree 3
    Agent<AgentModel>::DEGREE = 3;
    Agent<AgentModel>::MAX_DEPTH = 2;
    Agent<AgentModel> a7 = Agent<AgentModel>(0, 0, 0);
    REQUIRE( a7.get_children().size() == 3);
    REQUIRE( a7.get_parent() == nullptr);
    REQUIRE( a7.get_depth() == 0 );
    REQUIRE( a7.get_number() == 0 );
    REQUIRE( !a7.is_leaf() );
    // 8.1 Verify childre depth 1
    REQUIRE( a7.get_children()[0]->get_children().size() == 3);
    REQUIRE( a7.get_children()[0]->get_parent() == &a7);
    REQUIRE( a7.get_children()[0]->get_depth() == 1 );
    REQUIRE( a7.get_children()[0]->get_number() == 1 );
    REQUIRE( !a7.get_children()[0]->is_leaf() );
    REQUIRE( a7.get_children()[0]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( a7.get_children()[0]->get_parent()[0].get_children().size() == 3);
    REQUIRE( a7.get_children()[0]->get_parent()[0].get_parent() == nullptr);
    REQUIRE( a7.get_children()[0]->get_parent()[0].get_depth() == 0 );
    REQUIRE( a7.get_children()[0]->get_parent()[0].get_number() == 0 );
    REQUIRE( !a7.get_children()[0]->get_parent()[0].is_leaf() );
    REQUIRE( !(a7.get_children()[0]->get_current() == a7.get_children()[0]->get_pocket()) );
    REQUIRE( a7.get_children()[1]->get_children().size() == 3);
    REQUIRE( a7.get_children()[1]->get_parent() == &a7);
    REQUIRE( a7.get_children()[1]->get_depth() == 1 );
    REQUIRE( a7.get_children()[1]->get_number() == 2 );
    REQUIRE( !a7.get_children()[1]->is_leaf() );
    REQUIRE( a7.get_children()[1]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( a7.get_children()[1]->get_parent()[0].get_children().size() == 3);
    REQUIRE( a7.get_children()[1]->get_parent()[0].get_parent() == nullptr);
    REQUIRE( a7.get_children()[1]->get_parent()[0].get_depth() == 0 );
    REQUIRE( a7.get_children()[1]->get_parent()[0].get_number() == 0 );
    REQUIRE( !a7.get_children()[1]->get_parent()[0].is_leaf() );
    REQUIRE( !(a7.get_children()[1]->get_current() == a7.get_children()[1]->get_pocket()) );
    REQUIRE( a7.get_children()[2]->get_children().size() == 3);
    REQUIRE( a7.get_children()[2]->get_parent() == &a7);
    REQUIRE( a7.get_children()[2]->get_depth() == 1 );
    REQUIRE( a7.get_children()[2]->get_number() == 3 );
    REQUIRE( !a7.get_children()[2]->is_leaf() );
    REQUIRE( a7.get_children()[2]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( a7.get_children()[2]->get_parent()[0].get_children().size() == 3);
    REQUIRE( a7.get_children()[2]->get_parent()[0].get_parent() == nullptr);
    REQUIRE( a7.get_children()[2]->get_parent()[0].get_depth() == 0 );
    REQUIRE( a7.get_children()[2]->get_parent()[0].get_number() == 0 );
    REQUIRE( !a7.get_children()[2]->get_parent()[0].is_leaf() );
    // Verify depth 2, child 0
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_children().size() == 0);
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_parent() == a7.get_children()[0]);
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_depth() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_number() == 4 );
    REQUIRE( a7.get_children()[0]->get_children()[0]->is_leaf() );
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[0]->get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[0]->get_children()[0]->get_current() == a7.get_children()[0]->get_children()[0]->get_pocket()) );
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_children().size() == 0);
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_parent() == a7.get_children()[0]);
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_depth() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_number() == 5 );
    REQUIRE( a7.get_children()[0]->get_children()[1]->is_leaf() );
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[0]->get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[0]->get_children()[1]->get_current() == a7.get_children()[0]->get_children()[1]->get_pocket()) );
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_children().size() == 0);
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_parent() == a7.get_children()[0]);
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_depth() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_number() == 6 );
    REQUIRE( a7.get_children()[0]->get_children()[2]->is_leaf() );
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[0]->get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[0]->get_children()[2]->get_current() == a7.get_children()[0]->get_children()[2]->get_pocket()) );
    // Verify depth 2, child 1
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_children().size() == 0);
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_parent() == a7.get_children()[1]);
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_depth() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_number() == 7 );
    REQUIRE( a7.get_children()[1]->get_children()[0]->is_leaf() );
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[1]->get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[1]->get_children()[0]->get_current() == a7.get_children()[1]->get_children()[0]->get_pocket()) );
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_children().size() == 0);
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_parent() == a7.get_children()[1]);
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_depth() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_number() == 8 );
    REQUIRE( a7.get_children()[1]->get_children()[1]->is_leaf() );
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[1]->get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[1]->get_children()[1]->get_current() == a7.get_children()[1]->get_children()[1]->get_pocket()) );
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_children().size() == 0);
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_parent() == a7.get_children()[1]);
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_depth() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_number() == 9 );
    REQUIRE( a7.get_children()[1]->get_children()[2]->is_leaf() );
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[1]->get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[1]->get_children()[2]->get_current() == a7.get_children()[1]->get_children()[2]->get_pocket()) );
    // Verify depth 2, child 2
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_children().size() == 0);
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_parent() == a7.get_children()[2]);
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_depth() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_number() == 10 );
    REQUIRE( a7.get_children()[2]->get_children()[0]->is_leaf() );
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[2]->get_children()[0]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[2]->get_children()[0]->get_current() == a7.get_children()[2]->get_children()[0]->get_pocket()) );
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_children().size() == 0);
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_parent() == a7.get_children()[2]);
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_depth() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_number() == 11 );
    REQUIRE( a7.get_children()[2]->get_children()[1]->is_leaf() );
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[2]->get_children()[1]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[2]->get_children()[1]->get_current() == a7.get_children()[2]->get_children()[1]->get_pocket()) );
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_children().size() == 0);
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_parent() == a7.get_children()[2]);
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_depth() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_number() == 12 );
    REQUIRE( a7.get_children()[2]->get_children()[2]->is_leaf() );
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_members().size() == 2 );
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_current().get_count_active() > 0 );
    REQUIRE( a7.get_children()[2]->get_children()[2]->get_pocket().get_count_active() > 0 );
    REQUIRE( !(a7.get_children()[2]->get_children()[2]->get_current() == a7.get_children()[2]->get_children()[2]->get_pocket()) );
}

TEST_CASE("Agent: set_pocket, set_current, get_pocket, get_current, get_memebers, exchange") {

    // Tests
    // 1. Set know pocket, confirm we can get pocket
    // 2. Set know current, confirm we can get current
    // 3. Make sure returned memebers match
    // 4. Trigger the exchange process
    // 5. Renew the 
    
    Agent<AgentModel>::DEGREE = 1;
    Agent<AgentModel>::MAX_DEPTH = 1;
    Agent<AgentModel> a = Agent<AgentModel>(0, 0, 0);
    // 1. Set know pocket, confirm we can get pocket
    AgentModel pock = frac_1();
    AgentModel curr = frac_2();
    REQUIRE( !(pock == curr) );
    REQUIRE( !(a.get_pocket() == pock) );
    a.set_pocket(pock);
    REQUIRE( (a.get_pocket() == pock));
    // 1.1 Check changing the external object does not impact the Agents
    pock.set_depth(7);
    REQUIRE( !(a.get_pocket() == pock));

    // 2. Set know current, confirm we can get current
    REQUIRE( !(a.get_current() == curr) );
    a.set_current(curr);
    REQUIRE( (a.get_current() == curr));
    // 2.1 Check changing the external object does not impact the Agents
    curr.set_depth(7);
    REQUIRE( !(a.get_current() == curr));

    // 3. Make sure returned memebers match
    pock = frac_1();
    curr = frac_2();    
    a.set_pocket(pock);
    a.set_current(curr);
    REQUIRE( (a.get_members()[0] == pock) );
    REQUIRE( (a.get_members()[1] == curr) );
    REQUIRE( (a.get_members()[Agent<AgentModel>::POCKET] == pock) );
    REQUIRE( (a.get_members()[Agent<AgentModel>::CURRENT]== curr) );
    
    // 4. Trigger the exchange process
    a.exchange();
    REQUIRE( (a.get_members()[Agent<AgentModel>::POCKET] == curr) );
    REQUIRE( (a.get_members()[Agent<AgentModel>::CURRENT]== pock) );
    a.exchange();
    REQUIRE( (a.get_members()[Agent<AgentModel>::POCKET] == pock) );
    REQUIRE( (a.get_members()[Agent<AgentModel>::CURRENT]== curr) );

}

TEST_CASE("Agent: bubble") {

    RandInt ri = RandInt(42);
    RandReal rr = RandReal(42);
    RandInt::RANDINT = &ri;
    RandReal::RANDREAL = &rr;

    // Tests
    // 1. Parent with single child, confirm pocks swap when fitness is smaller in child
    // 2. Case 1, but both best child pocket and current < parent pocket (i.e. child current goes to pocket, parent pocket goes to child current)
    // 3. Parent with multi child, confirm best child pocket with parent pocket

    Agent<AgentModel>::DEGREE = 1;
    Agent<AgentModel>::MAX_DEPTH = 1;
    
    // 1. Parent with single child, confirm pocks swap when fitness is smaller in child
    Agent<AgentModel> a = Agent<AgentModel>(0, 0, 0);
    a.get_pocket().set_fitness(10);
    a.get_children()[0]->get_pocket().set_fitness(9);
    a.get_children()[0]->get_current().set_fitness(11);     // Child current will not exchange with new child pocket
    AgentModel pock0 = a.get_pocket();
    AgentModel curr0 = a.get_current();
    AgentModel pock1 = a.get_children()[0]->get_pocket();
    AgentModel curr1 = a.get_children()[0]->get_current();
    a.bubble();
    // 1.1 Confirm pockets have changed
    REQUIRE(a.get_pocket() == pock1);
    REQUIRE(a.get_children()[0]->get_pocket() == pock0);
    // 1.2 Confirm currents remain unchanged
    REQUIRE(a.get_current() == curr0);
    REQUIRE(a.get_children()[0]->get_current() == curr1);
    
    // 2. Case 1, but both best child pocket and current < parent pocket (i.e. child current goes to pocket, parent pocket goes to child current)
    Agent<AgentModel> a2 = Agent<AgentModel>(0, 0, 0);
    a2.get_pocket().set_fitness(10);
    a2.get_children()[0]->get_pocket().set_fitness(8);
    a2.get_children()[0]->get_current().set_fitness(9);     // Child current will not exchange with new child pocket
    pock0 = a2.get_pocket();
    curr0 = a2.get_current();
    pock1 = a2.get_children()[0]->get_pocket();
    curr1 = a2.get_children()[0]->get_current();
    a2.bubble();
    // 1.1 Confirm pockets have changed
    REQUIRE(a2.get_pocket() == pock1);
    REQUIRE(a2.get_children()[0]->get_pocket() == curr1);
    // 1.2 Confirm currents remain unchanged
    REQUIRE(a2.get_current() == curr0);
    REQUIRE(a2.get_children()[0]->get_current() == pock0);
    
    // 3. Parent with multi child, confirm best child pocket with parent pocket
    Agent<AgentModel>::DEGREE = 3;
    Agent<AgentModel>::MAX_DEPTH = 1;
    Agent<AgentModel> a3 = Agent<AgentModel>(0, 0, 0);
    // Set parent fitness
    a3.get_pocket().set_fitness(10);
    a3.get_pocket().set_fitness(15);
    // Set all child fitnesses, we expect middle child 1 to be most fit
    a3.get_children()[0]->get_pocket().set_fitness(6);
    a3.get_children()[0]->get_current().set_fitness(17);
    a3.get_children()[1]->get_pocket().set_fitness(4);
    a3.get_children()[1]->get_current().set_fitness(15);
    a3.get_children()[2]->get_pocket().set_fitness(5);
    a3.get_children()[2]->get_current().set_fitness(16);
    // Save all solns
    pock0 = a3.get_pocket();
    curr0 = a3.get_current();
    pock1 = a3.get_children()[0]->get_pocket();
    curr1 = a3.get_children()[0]->get_current();
    AgentModel pock2 = a3.get_children()[1]->get_pocket();
    AgentModel curr2 = a3.get_children()[1]->get_current();
    AgentModel pock3 = a3.get_children()[2]->get_pocket();
    AgentModel curr3 = a3.get_children()[2]->get_current();
    // 
    a3.bubble();
    REQUIRE(a3.get_pocket() == pock2);   // Pocket swaped with middle child
    REQUIRE(a3.get_current() == curr0);
    REQUIRE(a3.get_children()[0]->get_pocket() == pock1);
    REQUIRE(a3.get_children()[0]->get_current() == curr1);
    REQUIRE(a3.get_children()[1]->get_pocket() == pock0);    // Pocket swaped with parent
    REQUIRE(a3.get_children()[1]->get_current() == curr2);
    REQUIRE(a3.get_children()[2]->get_pocket() == pock3);
    REQUIRE(a3.get_children()[2]->get_current() == curr3);

}