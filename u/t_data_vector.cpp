/**
  *  \file u/t_data_vector.cpp
  *  \brief Test for afl::data::Vector
  */

#include "afl/data/vector.hpp"

#include "u/t_data.hpp"
#include "afl/data/integervalue.hpp"

/** Simple test. */
void
TestDataVector::testIt()
{
    // Make it
    afl::base::Ref<afl::data::Vector> p = afl::data::Vector::create();
    TS_ASSERT(&p.get() != 0);
    TS_ASSERT_EQUALS(p->size(), 0U);

    // Set some value
    TS_ASSERT(p->get(2) == 0);
    p->setNew(2, new afl::data::IntegerValue(99));
    TS_ASSERT_EQUALS(p->size(), 3U);
    TS_ASSERT(p->get(0) == 0);
    TS_ASSERT(p->get(1) == 0);
    TS_ASSERT(p->get(2) != 0);

    // Push back (clones)
    p->pushBack(p->get(2));
    TS_ASSERT_EQUALS(p->size(), 4U);
    TS_ASSERT(p->get(0) == 0);
    TS_ASSERT(p->get(1) == 0);
    TS_ASSERT(p->get(2) != 0);
    TS_ASSERT(p->get(3) != 0);
    TS_ASSERT(p->get(2) != p->get(3));
}
