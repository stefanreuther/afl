/**
  *  \file u/t_base_inlineoptional.cpp
  */

#include "afl/base/inlineoptional.hpp"

#include "u/t_base.hpp"

/** Test behaviour using an integer. */
void
TestBaseInlineOptional::testIt()
{
    afl::base::InlineOptional<int,-1> a;
    afl::base::InlineOptional<int,-1> b(17);
    int tmp;

    TS_ASSERT(!a.isValid());
    TS_ASSERT(b.isValid());
    TS_ASSERT(!a.get(tmp));
    TS_ASSERT(b.get(tmp));
    TS_ASSERT_EQUALS(tmp, 17);
    TS_ASSERT(!a.isSame(b));

    a = 33;
    TS_ASSERT(a.isValid());
    TS_ASSERT(a.get(tmp));
    TS_ASSERT_EQUALS(tmp, 33);

    a = b;
    TS_ASSERT(a.isValid());
    TS_ASSERT(a.get(tmp));
    TS_ASSERT_EQUALS(tmp, 17);
    TS_ASSERT(a.isSame(b));

    b = afl::base::InlineOptional<int,-1>();
    TS_ASSERT(!b.isValid());
    TS_ASSERT(!b.get(tmp));

    afl::base::InlineOptional<long,0> s;
    TS_ASSERT(!s.isValid());

    s = a;
    TS_ASSERT(s.isValid());

    long val = 0;
    TS_ASSERT(s.get(val));
    TS_ASSERT_EQUALS(val, 17);

    s = b;
    TS_ASSERT(!s.isValid());
    TS_ASSERT(!s.get(val));
    TS_ASSERT(b.isSame(s));

    afl::base::InlineOptional<int,-1> c = afl::base::Nothing;
    TS_ASSERT(!c.isValid());

    a = 33;
    TS_ASSERT(a.isValid());
    TS_ASSERT_EQUALS(a.orElse(10), 33);
    a = afl::base::Nothing;
    TS_ASSERT(!a.isValid());
    TS_ASSERT_EQUALS(a.orElse(10), 10);
}
