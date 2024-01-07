/**
  *  \file test/afl/data/segmentviewtest.cpp
  *  \brief Test for afl::data::SegmentView
  */

#include "afl/data/segmentview.hpp"

#include "afl/data/segment.hpp"
#include "afl/except/invaliddataexception.hpp"
#include "afl/test/testrunner.hpp"

/** Test the untyped SegmentView::eat(). */
AFL_TEST("afl.data.SegmentView:eat", as)
{
    afl::data::Segment seg;
    seg.pushBackInteger(12);
    seg.pushBackString("foo");
    seg.pushBackNew(0);
    seg.pushBackInteger(42);
    seg.pushBackString("hello");
    seg.pushBackString("bye");

    // First constructor
    {
        afl::test::Assert a(as("from-segment"));
        afl::data::SegmentView v(seg);
        a.checkEqual("size 1", v.size(), 6U);
        a.check("eat 1", v.eat());     // 12
        a.check("eat 2", v.eat());     // "foo"
        a.check("eat 3", !v.eat());    // 0
        a.check("eat 4", v.eat());     // 42
        a.checkEqual("size 2", v.size(), 2U);
        a.check("eat 5", v.eat());     // "hello"
        a.check("eat 6", v.eat());     // "bye"
        a.checkEqual("size 3", v.size(), 0U);
        a.check("eat 7", !v.eat());    // off-end
        a.checkEqual("size 4", v.size(), 0U);
    }

    // Second constructor, non-overflow
    {
        afl::test::Assert a(as("from-range:valid"));
        afl::data::SegmentView v(seg, 1, 2);
        a.checkEqual("size", v.size(), 2U);
        a.check("eat 1", v.eat());     // "foo"
        a.check("eat 2", !v.eat());    // 0
    }

    // Second constructor, overflow case #1
    {
        afl::test::Assert a(as("from-range:excess-size"));
        afl::data::SegmentView v(seg, 3, 5);
        a.checkEqual("size", v.size(), 3U);
        a.check("eat 1", v.eat());     // 42
        a.check("eat 2", v.eat());     // "hello"
        a.check("eat 3", v.eat());     // "bye"
        a.check("eat 4", !v.eat());
    }

    // Second constructor, overflow case #2
    {
        afl::test::Assert a(as("from-range:excess-index"));
        afl::data::SegmentView v(seg, 1000, 5);
        a.checkEqual("size", v.size(), 0U);
        a.check("eat", !v.eat());
    }
}

AFL_TEST("afl.data.SegmentView:typed", a)
{
    afl::data::Segment seg;
    seg.pushBackInteger(12);
    seg.pushBackString("foo");
    seg.pushBackNew(0);
    seg.pushBackInteger(42);
    seg.pushBackString("hello");
    seg.pushBackInteger(99);

    int32_t iv;
    String_t sv;
    afl::data::SegmentView v(seg);

    // Regular cases
    a.check("eat int", v.eat(iv));
    a.checkEqual("eat int value", iv, 12);
    a.check("eat str", v.eat(sv));
    a.checkEqual("eat str value", sv, "foo");

    // Fetching null
    iv = 17;
    a.check("eat int null", !v.eat(iv));
    a.checkEqual("eat int null value unchanged", iv, 17);

    // Regular case
    a.check("eat int 2", v.eat(iv));
    a.checkEqual("eat int value 2", iv, 42);
    a.checkEqual("remaining size", v.size(), 2U);

    // Fetching integer when there is a string throws
    AFL_CHECK_THROWS(a("eat int wrong type"), v.eat(iv), afl::except::InvalidDataException);

    // Fetching string when there is an integer converts
    a.check("eat str 2", v.eat(sv));
    a.checkEqual("eat str value 2", sv, "99");
}
