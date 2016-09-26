/**
  *  \file u/t_data_segmentview.cpp
  *  \brief Test for afl::data::SegmentView
  */

#include "afl/data/segmentview.hpp"

#include "u/t_data.hpp"
#include "afl/data/segment.hpp"
#include "afl/except/invaliddataexception.hpp"


/** Test the untyped SegmentView::eat(). */
void
TestDataSegmentView::testEat()
{
    afl::data::Segment seg;
    seg.pushBack(12);
    seg.pushBack("foo");
    seg.pushBackNew(0);
    seg.pushBack(42);
    seg.pushBack("hello");
    seg.pushBack("bye");

    // First constructor
    {
        afl::data::SegmentView v(seg);
        TS_ASSERT_EQUALS(v.size(), 6U);
        TS_ASSERT(v.eat());     // 12
        TS_ASSERT(v.eat());     // "foo"
        TS_ASSERT(!v.eat());    // 0
        TS_ASSERT(v.eat());     // 42
        TS_ASSERT_EQUALS(v.size(), 2U);
        TS_ASSERT(v.eat());     // "hello"
        TS_ASSERT(v.eat());     // "bye"
        TS_ASSERT_EQUALS(v.size(), 0U);
        TS_ASSERT(!v.eat());    // off-end
        TS_ASSERT_EQUALS(v.size(), 0U);
    }

    // Second constructor, non-overflow
    {
        afl::data::SegmentView v(seg, 1, 2);
        TS_ASSERT_EQUALS(v.size(), 2U);
        TS_ASSERT(v.eat());     // "foo"
        TS_ASSERT(!v.eat());    // 0
    }

    // Second constructor, overflow case #1
    {
        afl::data::SegmentView v(seg, 3, 5);
        TS_ASSERT_EQUALS(v.size(), 3U);
        TS_ASSERT(v.eat());     // 42
        TS_ASSERT(v.eat());     // "hello"
        TS_ASSERT(v.eat());     // "bye"
        TS_ASSERT(!v.eat());
    }

    // Second constructor, overflow case #2
    {
        afl::data::SegmentView v(seg, 1000, 5);
        TS_ASSERT_EQUALS(v.size(), 0U);
        TS_ASSERT(!v.eat());
    }
}

/** Test typed extractors. */
void
TestDataSegmentView::testTypes()
{
    afl::data::Segment seg;
    seg.pushBack(12);
    seg.pushBack("foo");
    seg.pushBackNew(0);
    seg.pushBack(42);
    seg.pushBack("hello");
    seg.pushBack(99);

    int32_t iv;
    String_t sv;
    afl::data::SegmentView v(seg);

    // Regular cases
    TS_ASSERT(v.eat(iv));
    TS_ASSERT_EQUALS(iv, 12);
    TS_ASSERT(v.eat(sv));
    TS_ASSERT_EQUALS(sv, "foo");

    // Fetching null
    iv = 17;
    TS_ASSERT(!v.eat(iv));
    TS_ASSERT_EQUALS(iv, 17);

    // Regular case
    TS_ASSERT(v.eat(iv));
    TS_ASSERT_EQUALS(iv, 42);
    TS_ASSERT_EQUALS(v.size(), 2U);

    // Fetching integer when there is a string throws
    TS_ASSERT_THROWS(v.eat(iv), afl::except::InvalidDataException);

    // Fetching string when there is an integer converts
    TS_ASSERT(v.eat(sv));
    TS_ASSERT_EQUALS(sv, "99");
}
