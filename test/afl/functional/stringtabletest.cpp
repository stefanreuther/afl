/**
  *  \file test/afl/functional/stringtabletest.cpp
  *  \brief Test for afl::functional::StringTable
  */

#include "afl/functional/stringtable.hpp"

#include <vector>
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.functional.StringTable:static-const-charp-array", a)
{
    static const char*const TABLE[] = { "eins", "zwei", "drei" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(TABLE));
    a.checkEqual("index 0", t(0), "eins");
    a.checkEqual("index 1", t(1), "zwei");
    a.checkEqual("index 2", t(2), "drei");

    a.checkEqual("index -1", t(-1), "");
    a.checkEqual("index 3", t(3), "");
    a.checkEqual("index 4", t(4), "");
    a.checkEqual("index 5", t(5), "");

    int32_t i;
    a.check("getFirstKey", t.getFirstKey(i));
    a.checkEqual("getFirstKey value", i, 0);
    a.check("getNextKey 1", t.getNextKey(i));
    a.checkEqual("getNextKey 1 value", i, 1);
    a.check("getNextKey 2", t.getNextKey(i));
    a.checkEqual("getNextKey 2 value", i, 2);
    a.check("getNextKey 3", !t.getNextKey(i));
}

/** Test a mutable array of const char* as table. */
AFL_TEST("afl.functional.StringTable:static-mutable-charp-array", a)
{
    static const char* MUTABLE[] = { "one", "two" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(MUTABLE));
    a.checkEqual("index 0", t(0), "one");
    a.checkEqual("index 1", t(1), "two");
    a.checkEqual("index 2", t(2), "");
}

/** Test an array of String_t as table. */
AFL_TEST("afl.functional.StringTable:string-array", a)
{
    String_t ARRAY[] = { "hi", "ho" };
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(ARRAY));
    a.checkEqual("index 0", t(0), "hi");
    a.checkEqual("index 1", t(1), "ho");
    a.checkEqual("index 2", t(2), "");

    int32_t i;
    a.check("getFirstKey", t.getFirstKey(i));
    a.checkEqual("getFirstKey value", i, 0);
    a.check("getNextKey 1", t.getNextKey(i));
    a.checkEqual("getNextKey 1 value", i, 1);
    a.check("getNextKey 2", !t.getNextKey(i));
}

/** Test a vector of String_t as table. */
AFL_TEST("afl.functional.StringTable:string-vector", a)
{
    std::vector<String_t> vec;
    vec.push_back("one");
    vec.push_back("two");
    vec.push_back("three");

    const afl::functional::StringTable_t& t(afl::functional::createStringTable(vec));
    a.checkEqual("index 0", t(0), "one");
    a.checkEqual("index 1", t(1), "two");
    a.checkEqual("index 2", t(2), "three");
    a.checkEqual("index 3", t(3), "");
}

/** Test empty string table. */
AFL_TEST("afl.functional.StringTable:empty", a)
{
    const afl::functional::StringTable_t& t(afl::functional::createStringTable(afl::base::Memory<const char* const>()));
    a.checkEqual("index 0", t(0), "");

    int32_t i;
    a.check("getFirstKey", !t.getFirstKey(i));
}
