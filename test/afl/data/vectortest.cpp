/**
  *  \file test/afl/data/vectortest.cpp
  *  \brief Test for afl::data::Vector
  */

#include "afl/data/vector.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/data/integervalue.hpp"

AFL_TEST("afl.data.Vector", a)
{
    // Make it
    afl::base::Ref<afl::data::Vector> p = afl::data::Vector::create();
    a.check("01. addr", &p.get() != 0);
    a.checkEqual("02. size", p->size(), 0U);

    // Set some value
    a.check("11. value", p->get(2) == 0);
    p->setNew(2, new afl::data::IntegerValue(99));
    a.checkEqual("12. size", p->size(), 3U);
    a.check("13. value", p->get(0) == 0);
    a.check("14. value", p->get(1) == 0);
    a.check("15. value", p->get(2) != 0);

    // Push back (clones)
    p->pushBack(p->get(2));
    a.checkEqual("21. size", p->size(), 4U);
    a.check("22. size", p->get(0) == 0);
    a.check("23. size", p->get(1) == 0);
    a.check("24. size", p->get(2) != 0);
    a.check("25. size", p->get(3) != 0);
    a.check("26. size", p->get(2) != p->get(3));
}
