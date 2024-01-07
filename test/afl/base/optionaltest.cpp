/**
  *  \file test/afl/base/optionaltest.cpp
  *  \brief Test for afl::base::Optional
  */

#include "afl/base/optional.hpp"
#include "afl/test/testrunner.hpp"

#include "afl/base/inlineoptional.hpp"
#include "afl/string/string.hpp"

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
AFL_TEST("afl.base.Optional:int", as)
{
    afl::base::Optional<int> a;
    afl::base::Optional<int> b(17);

    as.check("01. isValid", !a.isValid());
    as.check("02. isValid", b.isValid());
    as.check("03. get", a.get() == 0);
    as.check("04. get", b.get() != 0);
    as.checkEqual("05. get", *b.get(), 17);
    as.check("06. isSame", !a.isSame(b));

    a = 33;
    as.check("11. isValid", a.isValid());
    as.checkEqual("12. get", *a.get(), 33);

    a = b;
    as.check("21. isValid", a.isValid());
    as.checkEqual("22. isValid", *a.get(), 17);
    as.checkDifferent("23. get", a.get(), b.get());
    as.check("24. isSame", a.isSame(b));

    b = afl::base::Optional<int>();
    as.check("31. isValid", !b.isValid());
    as.checkNull("32. get", b.get());

    afl::base::Optional<long> s;
    as.check("41. isValid", !s.isValid());

    s = a;
    as.check("51. isValid", s.isValid());
    as.checkEqual("52. get", *s.get(), 17);

    long val = 0;
    as.check("61. get", s.get(val));
    as.checkEqual("62. value", val, 17);

    s = b;
    as.check("71. isValid", !s.isValid());
    as.check("72. get", !s.get(val));

    afl::base::Optional<int> c = afl::base::Nothing;
    as.check("81. isValid", !c.isValid());

    a = 33;
    as.check("91. isValid", a.isValid());
    as.checkEqual("92. orElse", a.orElse(10), 33);
    a = afl::base::Nothing;
    as.check("93. isValid", !a.isValid());
    as.checkEqual("94. orElse", a.orElse(10), 10);
}

/** Test behaviour using a string (class, with little observable behaviour). */
AFL_TEST("afl.base.Optional:string", as) {
    afl::base::Optional<String_t> a;
    afl::base::Optional<String_t> b("hi");

    as.check("11. isValid", !a.isValid());
    as.check("12. isValid", b.isValid());
    as.check("13. get", a.get() == 0);
    as.check("14. get", b.get() != 0);
    as.checkEqual("15. get", *b.get(), "hi");
    as.check("16. isSame", !a.isSame(b));

    b = String_t("hu");
    as.checkEqual("21. get", *b.get(), "hu");

    b = String_t("he");
    as.checkEqual("31. get", *b.get(), "he");
    as.check("32. isSame", b.isSame(b));
    as.checkEqual("33. orElse", b.orElse("ah"), "he");

    a = b;
    as.check("41. isSame", a.isSame(b));

    b.clear();
    as.check("51. get", b.get() == 0);
    as.check("52. isSame", b.isSame(b));
    as.checkEqual("53. orElse", b.orElse("ah"), "ah");
}

/** Test behaviour using a class with observable behaviour.
    This verifies that ctor/dtor are called correctly. */
AFL_TEST("afl.base.Optional:class", as)
{
    as.checkEqual("01. instances", g_instances, 0);

    afl::base::Optional<TestClass> a;
    afl::base::Optional<TestClass> b = TestClass();

    as.check("11. isValid", !a.isValid());
    as.check("12. isValid", b.isValid());
    as.check("13. get", a.get() == 0);
    as.check("14. get", b.get() != 0);

    as.checkEqual("15. instances", g_instances, 1);

    a = b;
    as.checkEqual("21. instances", g_instances, 2);
    as.checkNonNull("22. get", a.get());
    as.checkNonNull("23. get", b.get());

    b = TestClass();
    as.checkEqual("31. instances", g_instances, 2);
    as.checkNonNull("32. get", a.get());
    as.checkNonNull("33. get", b.get());

    b = afl::base::Optional<TestClass>();
    as.checkEqual("41. instances", g_instances, 1);
    as.checkNonNull("42. get", a.get());
    as.checkNull("43. get", b.get());

    {
        afl::base::Optional<TestClass> c = a;
        as.checkEqual("51. instances", g_instances, 2);
    }

    as.checkEqual("61. instances", g_instances, 1);
}

/** Test interaction with InlineOptional. */
AFL_TEST("afl.base.Optional:inline", a) {
    afl::base::Optional<int> o;
    afl::base::InlineOptional<int,-1> io;
    afl::base::InlineOptional<char,-1,int> cio;
    int n;

    a.check("01. isValid", !o.isValid());
    a.check("02. isValid", !io.isValid());
    a.check("03. isValid", !cio.isValid());

    o = io;
    a.check("11. isValid", !o.isValid());

    o = cio;
    a.check("21. isValid", !o.isValid());

    io = 10;
    a.check("31. isValid", io.isValid());
    a.check("32. get", io.get(n));
    a.checkEqual("33. value", n, 10);

    o = io;
    a.check("41. isValid", o.isValid());
    a.check("42. get", o.get(n));
    a.checkEqual("43. value", n, 10);
    a.checkEqual("44. get", *o.get(), 10);

    cio = 9;
    a.check("51. isValid", cio.isValid());
    a.check("52. get", cio.get(n));
    a.checkEqual("53. value", n, 9);

    o = cio;
    a.check("61. isValid", o.isValid());
    a.check("62. get", o.get(n));
    a.checkEqual("63. value", n, 9);
    a.checkEqual("64. get", *o.get(), 9);
}
