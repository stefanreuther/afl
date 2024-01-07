/**
  *  \file test/afl/data/segmenttest.cpp
  *  \brief Test for afl::data::Segment
  */

#include "afl/data/segment.hpp"

#include "afl/data/integerlist.hpp"
#include "afl/data/integervalue.hpp"
#include "afl/data/stringvalue.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.data.Segment:index-operations", a)
{
    afl::data::Segment seg;
    a.checkEqual("01. size", seg.size(), 0U);
    a.checkEqual("02. getNumUsedSlots", seg.getNumUsedSlots(), 0U);

    // First element
    seg.setNew(2, new afl::data::IntegerValue(99));
    a.check("11. size", seg.size() >= 3U);
    a.checkEqual("12. getNumUsedSlots", seg.getNumUsedSlots(), 3U);
    a.check("13. index", seg[0] == 0);
    a.check("14. index", seg[1] == 0);
    a.check("15. index", seg[2] != 0);
    a.check("16. index", seg[3] == 0);
    a.check("17. index", seg[4] == 0);

    // Swap down one place
    seg.swapElements(2, seg, 1);
    a.check("21. size", seg.size() >= 3U);
    a.checkEqual("22. getNumUsedSlots", seg.getNumUsedSlots(), 2U);
    a.check("23. index", seg[0] == 0);
    a.check("24. index", seg[1] != 0);
    a.check("25. index", seg[2] == 0);
    a.check("26. index", seg[3] == 0);
    a.check("27. index", seg[4] == 0);

    // Swap self
    seg.swapElements(1, seg, 1);
    a.check("31. size", seg.size() >= 3U);
    a.checkEqual("32. getNumUsedSlots", seg.getNumUsedSlots(), 2U);
    a.check("33. index", seg[0] == 0);
    a.check("34. index", seg[1] != 0);
    a.check("35. index", seg[2] == 0);
    a.check("36. index", seg[3] == 0);
    a.check("37. index", seg[4] == 0);

    // Duplicate
    seg.set(3, seg[1]);
    seg.set(4, seg[0]);
    a.checkEqual("41. getNumUsedSlots", seg.getNumUsedSlots(), 4U);
    a.check("42. index", seg[0] == 0);
    a.check("43. index", seg[1] != 0);
    a.check("44. index", seg[2] == 0);
    a.check("45. index", seg[3] != 0);
    a.check("46. index", seg[4] == 0);
    a.check("47. index", seg[1] != seg[3]);
    a.checkEqual("48. value", dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue(), 99);
    a.checkEqual("49. value", dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue(), 99);

    // Extract
    delete seg.extractElement(1);
    a.check("51. index", seg[0] == 0);
    a.check("52. index", seg[1] == 0);
    a.check("53. index", seg[2] == 0);
    a.check("54. index", seg[3] != 0);
    a.check("55. index", seg[4] == 0);
}

AFL_TEST("afl.data.Segment:stack-operations", a)
{
    afl::data::Segment seg;

    // First stack element
    seg.pushBackNew(new afl::data::IntegerValue(1));
    a.checkEqual("01. size", seg.size(), 1U);

    // Second stack element
    seg.pushBack(seg.top());
    a.checkEqual("11. size", seg.size(), 2U);
    a.check("12. top", seg.top() != seg.top(1));
    a.check("13. top", seg.top() == seg.top(0));

    // Third stack element
    seg.pushBackNew(new afl::data::IntegerValue(2));
    a.checkEqual("21. size", seg.size(), 3U);

    // Transfer two elements to other segment
    afl::data::Segment seg2;
    seg.transferLastTo(2, seg2);
    a.checkEqual("31. size", seg.size(), 1U);
    a.checkEqual("32. size", seg2.size(), 2U);

    a.checkEqual("33. value", dynamic_cast<afl::data::IntegerValue*>(seg.top())->getValue(), 1);

    a.checkEqual("34. value", dynamic_cast<afl::data::IntegerValue*>(seg2.top(0))->getValue(), 2);
    a.checkEqual("35. value", dynamic_cast<afl::data::IntegerValue*>(seg2.top(1))->getValue(), 1);

    // Extract
    delete seg.extractTop();
    a.checkEqual("41. size", seg.size(), 0U);
}

AFL_TEST("afl.data.Segment:push", a)
{
    // Initialize
    afl::data::Segment seg;
    seg.pushBackString("hi");
    seg.pushBackInteger(17);

    // Check
    a.checkEqual("01. size", seg.size(), 2U);
    a.check("02. type",  dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    a.check("03. value", dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    a.check("04. type",  dynamic_cast<afl::data::IntegerValue*>(seg[1]) != 0);
    a.check("05. value", dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue() == 17);

    // Append some integers
    afl::data::IntegerList_t ints;
    ints.push_back(9);
    ints.push_back(8);
    ints.push_back(7);
    seg.pushBackElements(ints);
    a.checkEqual("11. size", seg.size(), 5U);
    a.check("12. type", dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    a.check("13. value", dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 9);
    a.check("14. type", dynamic_cast<afl::data::IntegerValue*>(seg[3]) != 0);
    a.check("15. value", dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue() == 8);
    a.check("16. type", dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    a.check("17. value", dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 7);

    // Append list to itself
    seg.pushBackElements(seg);
    a.checkEqual("21. size", seg.size(), 10U);
    a.check("22. type",  dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    a.check("23. value", dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 7);
    a.check("24. type",  dynamic_cast<afl::data::IntegerValue*>(seg[9]) != 0);
    a.check("25. value", dynamic_cast<afl::data::IntegerValue*>(seg[9])->getValue() == 7);
}

/** Test insert operation. */
AFL_TEST("afl.data.Segment:insert", a)
{
    // Initialize
    afl::data::Segment seg;
    seg.pushBackString("hi");
    seg.pushBackInteger(17);

    // Insert: "hi",3,17
    seg.insertNew(1, new afl::data::IntegerValue(3));
    a.checkEqual("01. size", seg.size(), 3U);
    a.check("02. type", dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    a.check("03. value", dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    a.check("04. type", dynamic_cast<afl::data::IntegerValue*>(seg[1]) != 0);
    a.check("05. value", dynamic_cast<afl::data::IntegerValue*>(seg[1])->getValue() == 3);
    a.check("06. type", dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    a.check("07. value", dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 17);

    // Insert again: "hi",null,3,17
    seg.insertNew(1, 0);
    a.checkEqual("11. size", seg.size(), 4U);
    a.check("12. type", dynamic_cast<afl::data::StringValue*>(seg[0]) != 0);
    a.check("13. value", dynamic_cast<afl::data::StringValue*>(seg[0])->getValue() == "hi");
    a.check("14. value", seg[1] == 0);
    a.check("15. type", dynamic_cast<afl::data::IntegerValue*>(seg[2]) != 0);
    a.check("16. value", dynamic_cast<afl::data::IntegerValue*>(seg[2])->getValue() == 3);
    a.check("17. type", dynamic_cast<afl::data::IntegerValue*>(seg[3]) != 0);
    a.check("18. value", dynamic_cast<afl::data::IntegerValue*>(seg[3])->getValue() == 17);

    // Beginning: 42,"hi",null,3,17
    seg.insertNew(0, new afl::data::IntegerValue(42));
    a.checkEqual("21. size", seg.size(), 5U);
    a.check("22. type", dynamic_cast<afl::data::IntegerValue*>(seg[0]) != 0);
    a.check("23. value", dynamic_cast<afl::data::IntegerValue*>(seg[0])->getValue() == 42);
    a.check("24. type", dynamic_cast<afl::data::StringValue*>(seg[1]) != 0);
    a.check("25. value", dynamic_cast<afl::data::StringValue*>(seg[1])->getValue() == "hi");

    // End: 42,"hi",null,3,17,-2
    seg.insertNew(5, new afl::data::IntegerValue(-2));
    a.checkEqual("31. size", seg.size(), 6U);
    a.check("32. type", dynamic_cast<afl::data::IntegerValue*>(seg[4]) != 0);
    a.check("35. value", dynamic_cast<afl::data::IntegerValue*>(seg[4])->getValue() == 17);
    a.check("34. type", dynamic_cast<afl::data::IntegerValue*>(seg[5]) != 0);
    a.check("36. value", dynamic_cast<afl::data::IntegerValue*>(seg[5])->getValue() == -2);

    // Beyond end
    seg.insertNew(40, new afl::data::IntegerValue(543));
    a.checkEqual("41. size", seg.size(), 41U);
    a.check("42. type", dynamic_cast<afl::data::IntegerValue*>(seg[40]) != 0);
    a.check("43. value", dynamic_cast<afl::data::IntegerValue*>(seg[40])->getValue() == 543);
    a.check("44. value", seg[37] == 0);
    a.check("45. value", seg[38] == 0);
    a.check("46. value", seg[39] == 0);
    a.check("47. value", seg[41] == 0);
    a.check("48. value", seg[42] == 0);
}
