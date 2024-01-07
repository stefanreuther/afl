/**
  *  \file test/afl/base/deletertest.cpp
  *  \brief Test for afl::base::Deleter
  */

#include "afl/base/deleter.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    // Test class that just counts the number of live instances
    class Tester : public afl::base::Deletable, private afl::base::Uncopyable {
     public:
        static int s_count;
        static int s_deathTracker;

        Tester(int id)
            : m_id(id)
            {
                ++s_count;
            }

        ~Tester()
            {
                s_deathTracker = 10*s_deathTracker + m_id;
                --s_count;
            }

     private:
        int m_id;
    };
    int Tester::s_count = 0;
    int Tester::s_deathTracker = 0;
}

/** Test addNew, clear, unsafeAddNew. */
AFL_TEST("afl.base.Deleter:main", a)
{
    a.checkEqual("initial count", Tester::s_count, 0);

    {
        // Place one object in the Deleter.
        afl::base::Deleter del;
        a.checkEqual("step 1", Tester::s_count, 0);
        Tester::s_deathTracker = 0;

        Tester* p = new Tester(1);
        a.checkEqual("step 2", Tester::s_count, 1);

        Tester& t = del.addNew(p);
        a.checkEqual("step 3: count", Tester::s_count, 1);
        a.checkEqual("step 3: address", p, &t);
        a.checkEqual("step 3: deaths", Tester::s_deathTracker, 0);

        // Clear the deleter
        del.clear();
        a.checkEqual("step 4: count", Tester::s_count, 0);
        a.checkEqual("step 4: deaths", Tester::s_deathTracker, 1);

        // Add new objects. Use unsafeAddNew for a change.
        // (The difference between addNew and unsafeAddNew is that the latter
        // leaks the object if it fails; this is not an issue for the unit test
        // which fails anyway when unsafeAddNew fails.)
        Tester::s_deathTracker = 0;
        del.addNew(new Tester(2));
        del.unsafeAddNew(new Tester(5));
        del.addNew(new Tester(7));
        a.checkEqual("step 5: count", Tester::s_count, 3);

        del.clear();
        a.checkEqual("step 6: count", Tester::s_count, 0);
        a.checkEqual("step 6: deaths", Tester::s_deathTracker, 752);

        // Again; this time let the destructor clean up
        Tester::s_deathTracker = 0;
        del.addNew(new Tester(1));
        del.addNew(new Tester(2));
        del.addNew(new Tester(3));
    }
    a.checkEqual("final count", Tester::s_deathTracker, 321);
}

/** Test move. */
AFL_TEST("afl.base.Deleter:move", a)
{
    a.checkEqual("initial count", Tester::s_count, 0);
    Tester::s_deathTracker = 0;

    {
        afl::base::Deleter outer;
        outer.addNew(new Tester(1));
        outer.addNew(new Tester(2));
        a.checkEqual("step 1", Tester::s_count, 2);

        {
            afl::base::Deleter inner;
            inner.addNew(new Tester(3));
            inner.addNew(new Tester(4));
            a.checkEqual("step 2: count", Tester::s_count, 4);
            a.checkEqual("step 2: deaths", Tester::s_deathTracker, 0);

            // This is a no-op
            inner.moveTo(inner);
            a.checkEqual("step 3: count", Tester::s_count, 4);
            a.checkEqual("step 3: deaths", Tester::s_deathTracker, 0);

            // This saves the objects
            inner.moveTo(outer);
            a.checkEqual("step 4: count", Tester::s_count, 4);
            a.checkEqual("step 4: deaths", Tester::s_deathTracker, 0);

            // Add some others
            inner.addNew(new Tester(5));
            a.checkEqual("step 5: count", Tester::s_count, 5);
            a.checkEqual("step 5: deaths", Tester::s_deathTracker, 0);
        }

        // 5 has died now
        a.checkEqual("step 6: count", Tester::s_count, 4);
        a.checkEqual("step 6: deaths", Tester::s_deathTracker, 5);

        outer.addNew(new Tester(6));
    }

    a.checkEqual("final count", Tester::s_count, 0);
    a.checkEqual("final death", Tester::s_deathTracker, 564321);
}
