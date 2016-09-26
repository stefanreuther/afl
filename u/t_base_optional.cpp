/**
  *  \file u/t_base_optional.cpp
  *  \brief Test for afl::base::Optional
  */

#include "afl/base/optional.hpp"
#include "u/t_base.hpp"
#include "afl/string/string.hpp"
#include "afl/base/inlineoptional.hpp"

namespace {
    int g_instances = 0;

    class TestClass {
     public:
        TestClass()
            { ++g_instances; }
        TestClass(const TestClass&)
            { ++g_instances; }
        ~TestClass()
            { --g_instances; }
    };
}

/** Test behaviour using an integer (POD). */
void
TestBaseOptional::testInt()
{
    afl::base::Optional<int> a;
    afl::base::Optional<int> b(17);

    TS_ASSERT(!a.isValid());
    TS_ASSERT(b.isValid());
    TS_ASSERT(a.get() == 0);
    TS_ASSERT(b.get() != 0);
    TS_ASSERT_EQUALS(*b.get(), 17);
    TS_ASSERT(!a.isSame(b));

    a = 33;
    TS_ASSERT(a.isValid());
    TS_ASSERT_EQUALS(*a.get(), 33);

    a = b;
    TS_ASSERT(a.isValid());
    TS_ASSERT_EQUALS(*a.get(), 17);
    TS_ASSERT_DIFFERS(a.get(), b.get());
    TS_ASSERT(a.isSame(b));

    b = afl::base::Optional<int>();
    TS_ASSERT(!b.isValid());
    TS_ASSERT(b.get() == 0);

    afl::base::Optional<long> s;
    TS_ASSERT(!s.isValid());

    s = a;
    TS_ASSERT(s.isValid());
    TS_ASSERT_EQUALS(*s.get(), 17);

    long val = 0;
    TS_ASSERT(s.get(val));
    TS_ASSERT_EQUALS(val, 17);

    s = b;
    TS_ASSERT(!s.isValid());
    TS_ASSERT(!s.get(val));

    afl::base::Optional<int> c = afl::base::Nothing;
    TS_ASSERT(!c.isValid());

    a = 33;
    TS_ASSERT(a.isValid());
    TS_ASSERT_EQUALS(a.orElse(10), 33);
    a = afl::base::Nothing;
    TS_ASSERT(!a.isValid());
    TS_ASSERT_EQUALS(a.orElse(10), 10);
}

/** Test behaviour using a string (class, with little observable behaviour). */
void
TestBaseOptional::testString()
{
    afl::base::Optional<String_t> a;
    afl::base::Optional<String_t> b("hi");

    TS_ASSERT(!a.isValid());
    TS_ASSERT(b.isValid());
    TS_ASSERT(a.get() == 0);
    TS_ASSERT(b.get() != 0);
    TS_ASSERT_EQUALS(*b.get(), "hi");
    TS_ASSERT(!a.isSame(b));

    b = String_t("hu");
    TS_ASSERT_EQUALS(*b.get(), "hu");

    b = String_t("he");
    TS_ASSERT_EQUALS(*b.get(), "he");
    TS_ASSERT(b.isSame(b));
    TS_ASSERT_EQUALS(b.orElse("ah"), "he");

    a = b;
    TS_ASSERT(a.isSame(b));

    b.clear();
    TS_ASSERT(b.get() == 0);
    TS_ASSERT(b.isSame(b));
    TS_ASSERT_EQUALS(b.orElse("ah"), "ah");
}

/** Test behaviour using a class with observable behaviour.
    This verifies that ctor/dtor are called correctly. */
void
TestBaseOptional::testClass()
{
    TS_ASSERT_EQUALS(g_instances, 0);

    afl::base::Optional<TestClass> a;
    afl::base::Optional<TestClass> b = TestClass();

    TS_ASSERT(!a.isValid());
    TS_ASSERT(b.isValid());
    TS_ASSERT(a.get() == 0);
    TS_ASSERT(b.get() != 0);

    TS_ASSERT_EQUALS(g_instances, 1);

    a = b;
    TS_ASSERT_EQUALS(g_instances, 2);
    TS_ASSERT(a.get() != 0);
    TS_ASSERT(b.get() != 0);

    b = TestClass();
    TS_ASSERT_EQUALS(g_instances, 2);
    TS_ASSERT(a.get() != 0);
    TS_ASSERT(b.get() != 0);

    b = afl::base::Optional<TestClass>();
    TS_ASSERT_EQUALS(g_instances, 1);
    TS_ASSERT(a.get() != 0);
    TS_ASSERT(b.get() == 0);

    {
        afl::base::Optional<TestClass> c = a;
        TS_ASSERT_EQUALS(g_instances, 2);
    }

    TS_ASSERT_EQUALS(g_instances, 1);
}

/** Test interaction with InlineOptional. */
void
TestBaseOptional::testInline()
{
    afl::base::Optional<int> o;
    afl::base::InlineOptional<int,-1> io;
    afl::base::InlineOptional<char,-1,int> cio;
    int n;

    TS_ASSERT(!o.isValid());
    TS_ASSERT(!io.isValid());
    TS_ASSERT(!cio.isValid());

    o = io;
    TS_ASSERT(!o.isValid());

    o = cio;
    TS_ASSERT(!o.isValid());

    io = 10;
    TS_ASSERT(io.isValid());
    TS_ASSERT(io.get(n));
    TS_ASSERT_EQUALS(n, 10);

    o = io;
    TS_ASSERT(o.isValid());
    TS_ASSERT(o.get(n));
    TS_ASSERT_EQUALS(n, 10);
    TS_ASSERT_EQUALS(*o.get(), 10);

    cio = 9;
    TS_ASSERT(cio.isValid());
    TS_ASSERT(cio.get(n));
    TS_ASSERT_EQUALS(n, 9);

    o = cio;
    TS_ASSERT(o.isValid());
    TS_ASSERT(o.get(n));
    TS_ASSERT_EQUALS(n, 9);
    TS_ASSERT_EQUALS(*o.get(), 9);
}
