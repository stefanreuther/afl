/**
  *  \file test/afl/data/hashtest.cpp
  *  \brief Test for afl::data::Hash
  */

#include "afl/data/hash.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/data/integervalue.hpp"

AFL_TEST("afl.data.Hash", a)
{
    // Make it
    afl::base::Ref<afl::data::Hash> p = afl::data::Hash::create();
    a.check("01. addr", &p.get() != 0);
    a.check("02. getKeys", p->getKeys().getNumNames() == 0);
    a.check("03. getValues", p->getValues().size() == 0);

    // Test it. Lookup failure most not modify the data.
    a.check("11. get", p->get("x") == 0);
    a.check("12. get", p->get("y") == 0);
    a.check("13. getNumNames", p->getKeys().getNumNames() == 0);
    a.check("14. size", p->getValues().size() == 0);

    // Store null value. This will create a value.
    p->setNew("x", 0);
    a.check("21. get", p->get("x") == 0);
    a.check("22. get", p->get("y") == 0);
    a.check("23. getNumNames", p->getKeys().getNumNames() == 1);
    // a.check("", p->getValues().size() == 1); Segment may have optimized.

    // Store non-null value.
    p->setNew("y", new afl::data::IntegerValue(42));
    a.check("31. get", p->get("x") == 0);
    a.check("32. get", p->get("y") != 0);
    a.check("33. getNumNames", p->getKeys().getNumNames() == 2);
    a.check("34. size", p->getValues().size() == 2);

    // It's case-sensitive
    a.check("35. get", p->get("Y") == 0);

    // Overwrite
    p->setNew("y", new afl::data::IntegerValue(77));
    a.check("41. get", p->get("x") == 0);
    a.check("42. get", p->get("y") != 0);
    a.check("43. getNumNames", p->getKeys().getNumNames() == 2);
    a.check("44. size", p->getValues().size() == 2);
    a.check("45. getValue", dynamic_cast<afl::data::IntegerValue*>(p->get("y"))->getValue() == 77);

    // Overwrite with clone
    afl::data::IntegerValue iv(66);
    p->set("y", &iv);
    a.check("51. get", p->get("y") != 0);
    a.check("52. get", p->get("y") != &iv);
    a.check("53. getValue", dynamic_cast<afl::data::IntegerValue*>(p->get("y"))->getValue() == 66);

    // Index access
    afl::data::Hash::Index_t x;
    a.check("61. getIndexByKey", p->getIndexByKey("y", x));
    a.check("62. getValueByIndex", p->getValueByIndex(x) != 0);
    a.check("63. getValue", dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 66);

    // Index modification
    p->setValueByIndexNew(x, new afl::data::IntegerValue(99));
    a.check("71. getValue", dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 99);

    p->setValueByIndex(x, &iv);
    a.check("81. getValue", dynamic_cast<afl::data::IntegerValue*>(p->getValueByIndex(x))->getValue() == 66);

    // Nonexistant
    a.check("91. getIndexByKey", !p->getIndexByKey("yyy", x));
}
