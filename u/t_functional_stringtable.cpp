/**
  *  \file u/t_functional_stringtable.cpp
  *  \brief Test for afl::functional::StringTable_t
  */

#include "afl/functional/stringtable.hpp"

#include <vector>
#include "u/t_functional.hpp"

/** Test a constant array of const char* as table. */
void
TestFunctionalStringTable::testStaticConst()
{
    static const char*const TABLE[] = { "eins", "zwei", "drei" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(TABLE));
    TS_ASSERT_EQUALS(t(0), "eins");
    TS_ASSERT_EQUALS(t(1), "zwei");
    TS_ASSERT_EQUALS(t(2), "drei");

    TS_ASSERT_EQUALS(t(-1), "");
    TS_ASSERT_EQUALS(t(3), "");
    TS_ASSERT_EQUALS(t(4), "");
    TS_ASSERT_EQUALS(t(5), "");

    int32_t i;
    TS_ASSERT(t.getFirstKey(i));
    TS_ASSERT_EQUALS(i, 0);
    TS_ASSERT(t.getNextKey(i));
    TS_ASSERT_EQUALS(i, 1);
    TS_ASSERT(t.getNextKey(i));
    TS_ASSERT_EQUALS(i, 2);
    TS_ASSERT(!t.getNextKey(i));
}

/** Test a mutable array of const char* as table. */
void
TestFunctionalStringTable::testStaticMutable()
{
    static const char* MUTABLE[] = { "one", "two" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(MUTABLE));
    TS_ASSERT_EQUALS(t(0), "one");
    TS_ASSERT_EQUALS(t(1), "two");
    TS_ASSERT_EQUALS(t(2), "");
}

/** Test an array of String_t as table. */
void
TestFunctionalStringTable::testStringArray()
{
    String_t ARRAY[] = { "hi", "ho" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(ARRAY));
    TS_ASSERT_EQUALS(t(0), "hi");
    TS_ASSERT_EQUALS(t(1), "ho");
    TS_ASSERT_EQUALS(t(2), "");

    int32_t i;
    TS_ASSERT(t.getFirstKey(i));
    TS_ASSERT_EQUALS(i, 0);
    TS_ASSERT(t.getNextKey(i));
    TS_ASSERT_EQUALS(i, 1);
    TS_ASSERT(!t.getNextKey(i));
}

/** Test a vector of String_t as table. */
void
TestFunctionalStringTable::testStringVector()
{
    std::vector<String_t> vec;
    vec.push_back("one");
    vec.push_back("two");
    vec.push_back("three");

    const afl::functional::StringTable_t& t(afl::functional::createStringTable(vec));
    TS_ASSERT_EQUALS(t(0), "one");
    TS_ASSERT_EQUALS(t(1), "two");
    TS_ASSERT_EQUALS(t(2), "three");
    TS_ASSERT_EQUALS(t(3), "");
}

/** Test empty string table. */
void
TestFunctionalStringTable::testEmpty()
{
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(afl::base::Memory<const char* const>()));
    TS_ASSERT_EQUALS(t(0), "");

    int32_t i;
    TS_ASSERT(!t.getFirstKey(i));
}
