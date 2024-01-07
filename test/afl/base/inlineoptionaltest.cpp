/**
  *  \file test/afl/base/inlineoptionaltest.cpp
  *  \brief Test for afl::base::InlineOptional
  */

#include "afl/base/inlineoptional.hpp"
#include "afl/test/testrunner.hpp"

/** Test behaviour using an integer. */
AFL_TEST("afl.base.InlineOptional", as) {
    afl::base::InlineOptional<int,-1> a;
    afl::base::InlineOptional<int,-1> b(17);
    int tmp;

    as.check("01. isValid", !a.isValid());
    as.check("02. isValid", b.isValid());
    as.check("03. get",     !a.get(tmp));
    as.check("04. get",     b.get(tmp));
    as.checkEqual("05. value", tmp, 17);
    as.check("06. isSame",  !a.isSame(b));

    a = 33;
    as.check("11. isValid", a.isValid());
    as.check("12. get",     a.get(tmp));
    as.checkEqual("13. value", tmp, 33);

    a = b;
    as.check("21. isValid",  a.isValid());
    as.check("22. get",      a.get(tmp));
    as.checkEqual("23. value", tmp, 17);
    as.check("24. isSame"  , a.isSame(b));

    b = afl::base::InlineOptional<int,-1>();
    as.check("31. isValid", !b.isValid());
    as.check("32. get",     !b.get(tmp));

    afl::base::InlineOptional<long,0> s;
    as.check("41. isValid", !s.isValid());

    s = a;
    as.check("51. isValid", s.isValid());

    long val = 0;
    as.check("52. get", s.get(val));
    as.checkEqual("53. value", val, 17);

    s = b;
    as.check("61. isValid", !s.isValid());
    as.check("62. get",     !s.get(val));
    as.check("63. isSame",  b.isSame(s));

    afl::base::InlineOptional<int,-1> c = afl::base::Nothing;
    as.check("71. isValid", !c.isValid());

    a = 33;
    as.check("81. isValid", a.isValid());
    as.checkEqual("82. orElse", a.orElse(10), 33);

    a = afl::base::Nothing;
    as.check("91. isValid", !a.isValid());
    as.checkEqual("92. orElse", a.orElse(10), 10);
}
