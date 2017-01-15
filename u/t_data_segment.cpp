/**
  *  \file u/t_data_segment.cpp
  *  \brief Test for afl::data::Segment
  */

#include "afl/data/segment.hpp"

#include "u/t_data.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/data/integerlist.hpp"

/** Test indexed operations. */
void
TestDataSegment::testIndex()
{
    afl::data::Segment seg;
    TS_ASSERT_EQUALS(seg.size(), 0U);
    TS_ASSERT_EQUALS(seg.getNumUsedSlots(), 0U);

    // First element
    seg.setNew(2, new afl::data::IntegerValue(99));
    TS_ASSERT(seg.size() >= 3U);
    TS_ASSERT_EQUALS(seg.getNumUsedSlots(), 3U);
    TS_ASSERT(seg[0] == 0);
    TS_ASSERT(seg[1] == 0);
    TS_ASSERT(seg[2] != 0);
    TS_ASSERT(seg[3] == 0);
    TS_ASSERT(seg[4] == 0);

    // Swap down one place
    seg.swapElements(2, seg, 1);
    TS_ASSERT(seg.size() >= 3U);
    TS_ASSERT_EQUALS(seg.getNumUsedSlots(), 2U);
    TS_ASSERT(seg[0] == 0);
    TS_ASSERT(seg[1] != 0);
    TS_ASSERT(seg[2] == 0);
    TS_ASSERT(seg[3] == 0);
    TS_ASSERT(seg[4] == 0);

    // Swap self
    seg.swapElements(1, seg, 1);
    TS_ASSERT(seg.size() >= 3U);
    TS_ASSERT_EQUALS(seg.getNumUsedSlots(), 2U);
    TS_ASSERT(seg[0] == 0);
    TS_ASSERT(seg[1] != 0);
    TS_ASSERT(seg[2] == 0);
    TS_ASSERT(seg[3] == 0);
    TS_ASSERT(seg[4] == 0);

    // Duplicate
    seg.set(3, seg[1]);
    seg.set(4, seg[0]);
    TS_ASSERT_EQUALS(seg.getNumUsedSlots(), 4U);
    TS_ASSERT(seg[0] == 0);
    TS_ASSERT(seg[1] != 0);
    TS_ASSERT(seg[2] == 0);
    TS_ASSERT(seg[3] != 0);
    TS_ASSERT(seg[4] == 0);
    TS_ASSERT(seg[1] != seg[3]);
    TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue(), 99);
    TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue(), 99);

    // Extract
    delete seg.extractElement(1);
    TS_ASSERT(seg[0] == 0);
    TS_ASSERT(seg[1] == 0);
    TS_ASSERT(seg[2] == 0);
    TS_ASSERT(seg[3] != 0);
    TS_ASSERT(seg[4] == 0);
}

/** Test stack operations. */
void
TestDataSegment::testStack()
{
    afl::data::Segment seg;

    // First stack element
    seg.pushBackNew(new afl::data::IntegerValue(1));
    TS_ASSERT_EQUALS(seg.size(), 1U);

    // Second stack element
    seg.pushBack(seg.top());
    TS_ASSERT_EQUALS(seg.size(), 2U);
    TS_ASSERT(seg.top() != seg.top(1));
    TS_ASSERT(seg.top() == seg.top(0));

    // Third stack element
    seg.pushBackNew(new afl::data::IntegerValue(2));
    TS_ASSERT_EQUALS(seg.size(), 3U);

    // Transfer two elements to other segment
    afl::data::Segment seg2;
    seg.transferLastTo(2, seg2);
    TS_ASSERT_EQUALS(seg.size(), 1U);
    TS_ASSERT_EQUALS(seg2.size(), 2U);

    TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(seg.top())->getValue(), 1);

    TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(seg2.top(0))->getValue(), 2);
    TS_ASSERT_EQUALS(dynamic_cast<afl::data::IntegerValue*>(seg2.top(1))->getValue(), 1);

    // Extract
    delete seg.extractTop();
    TS_ASSERT_EQUALS(seg.size(), 0U);
}

/** Test push operations. */
void
TestDataSegment::testPush()
{
    // Initialize
    afl::data::Segment seg;
    seg.pushBackString("hi");
    seg.pushBackInteger(17);

    // Check
    TS_ASSERT_EQUALS(seg.size(), 2U);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[1]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue() == 17);

    // Append some integers
    afl::data::IntegerList_t ints;
    ints.push_back(9);
    ints.push_back(8);
    ints.push_back(7);
    seg.pushBackElements(ints);
    TS_ASSERT_EQUALS(seg.size(), 5U);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 9);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[3]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue() == 8);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 7);

    // Append list to itself
    seg.pushBackElements(seg);
    TS_ASSERT_EQUALS(seg.size(), 10U);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 7);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[9]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[9])->getValue() == 7);
}

/** Test insert operation. */
void
TestDataSegment::testInsert()
{
    // Initialize
    afl::data::Segment seg;
    seg.pushBackString("hi");
    seg.pushBackInteger(17);

    // Insert: "hi",3,17
    seg.insertNew(1, new afl::data::IntegerValue(3));
    TS_ASSERT_EQUALS(seg.size(), 3U);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[1]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue() == 3);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 17);

    // Insert again: "hi",null,3,17
    seg.insertNew(1, 0);
    TS_ASSERT_EQUALS(seg.size(), 4U);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    TS_ASSERT(seg[1] == 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 3);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[3]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue() == 17);

    // Beginning: 42,"hi",null,3,17
    seg.insertNew(0, new afl::data::IntegerValue(42));
    TS_ASSERT_EQUALS(seg.size(), 5U);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[0]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[0])->getValue() == 42);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[1]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::StringValue*>(seg[1])->getValue() == "hi");

    // End: 42,"hi",null,3,17,-2
    seg.insertNew(5, new afl::data::IntegerValue(-2));
    TS_ASSERT_EQUALS(seg.size(), 6U);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 17);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[5]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[5])->getValue() == -2);

    // Beyond end
    seg.insertNew(40, new afl::data::IntegerValue(543));
    TS_ASSERT_EQUALS(seg.size(), 41U);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[40]) != 0);
    TS_ASSERT(dynamic_cast<afl::data::IntegerValue*>(seg[40])->getValue() == 543);
    TS_ASSERT(seg[37] == 0);
    TS_ASSERT(seg[38] == 0);
    TS_ASSERT(seg[39] == 0);
    TS_ASSERT(seg[41] == 0);
    TS_ASSERT(seg[42] == 0);
}
