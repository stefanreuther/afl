/**
  *  \file u/t_data_hash.cpp
  *  \brief Test for afl::data::Hash
  */

#include "afl/data/hash.hpp"

#include "u/t_data.hpp"
#include "afl/data/integervalue.hpp"

/** Simple test. */
void
TestDataHash::testIt()
{
    // Make it
    afl::base::Ref<afl::data::Hash> p = afl::data::Hash::create();
    TS_ASSERT(&p.get() != 0);
    TS_ASSERT(p->getKeys().getNumNames() == 0);
    TS_ASSERT(p->getValues().size() == 0);

    // Test it. Lookup failure most not modify the data.
    TS_ASSERT(p->get("x") == 0);
    TS_ASSERT(p->get("y") == 0);
    TS_ASSERT(p->getKeys().getNumNames() == 0);
    TS_ASSERT(p->getValues().size() == 0);

    // Store null value. This will create a value.
    p->setNew("x", 0);
    TS_ASSERT(p->get("x") == 0);
    TS_ASSERT(p->get("y") == 0);
    TS_ASSERT(p->getKeys().getNumNames() == 1);
    // TS_ASSERT(p->getValues().size() == 1); Segment may have optimized.

    // Store non-null value.
    p->setNew("y", new afl::data::IntegerValue(42));
    TS_ASSERT(p->get("x") == 0);
    TS_ASSERT(p->get("y") != 0);
    TS_ASSERT(p->getKeys().getNumNames() == 2);
    TS_ASSERT(p->getValues().size() == 2);

    // It's case-sensitive
    TS_ASSERT(p->get("Y") == 0);

    // Overwrite
    p->setNew("y", new afl::data::IntegerValue(77));
    TS_ASSERT(p->get("x") == 0);
    TS_ASSERT(p->get("y") != 0);
    TS_ASSERT(p->getKeys().getNumNames() == 2);
    TS_ASSERT(p->getValues().size() == 2);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(p->get("y"))->getValue() == 77);

    // Overwrite with clone
    afl::data::IntegerValue iv(66);
    p->set("y", &iv);
    TS_ASSERT(p->get("y") != 0);
    TS_ASSERT(p->get("y") != &iv);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(p->get("y"))->getValue() == 66);

    // Index access
    afl::data::Hash::Index_t x;
    TS_ASSERT(p->getIndexByKey("y", x));
    TS_ASSERT(p->getValueByIndex(x) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 66);

    // Index modification
    p->setValueByIndexNew(x, new afl::data::IntegerValue(99));
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 99);

    p->setValueByIndex(x, &iv);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 66);

    // Nonexistant
    TS_ASSERT(!p->getIndexByKey("yyy", x));
}
