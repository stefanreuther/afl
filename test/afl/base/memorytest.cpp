/**
  *  \file test/afl/base/memorytest.cpp
  *  \brief Test for afl::base::Memory
  */

#include "afl/base/memory.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    template<typename T>
    struct Tester {
        static void testIt(const afl::test::Assert& a) {
            // Construct a blank one
            afl::base::Memory<T> blank;
            a.check("01. empty", blank.empty());
            a.checkEqual("02. size", blank.size(), 0U);

            // Construct one with two elements
            T array[2];
            afl::base::Memory<T> two(array);
            a.check("11. empty", !two.empty());
            a.checkEqual("12. size", two.size(), 2U);
            a.checkEqual("13. unsafeData", two.unsafeData(), &array[0]);
            two.fill(T());

            // Construct from single object
            T obj;
            afl::base::Memory<T> one(afl::base::Memory<T>::fromSingleObject(obj));
            a.check("21. empty", !one.empty());
            a.checkEqual("22. size", one.size(), 1U);
            a.checkEqual("23. unsafeData", one.unsafeData(), &obj);

            // Basic checks
            a.check("31. equalContent", !two.equalContent(blank));
            a.check("32. compareContent", two.compareContent(blank) > 0);
            a.check("33. compareContent", blank.compareContent(two) < 0);
            a.check("34. equalContent", two.equalContent(two));
            a.checkEqual("35. compareContent", two.compareContent(two), 0);
            a.check("36. copyFrom/empty", two.copyFrom(blank).empty());
            a.checkEqual("37. find", blank.find(T()), 0U);
            a.check("38. atEnd", blank.atEnd(0) == 0);
            a.check("39. atEnd", two.atEnd(0) == &array[1]);
            a.check("3A. atEnd", two.atEnd(1) == &array[0]);
            a.check("3B. atEnd", two.atEnd(2) == 0);
            a.check("3C. at", two.at(0) == &array[0]);
            a.check("3D. at", two.at(1) == &array[1]);
            a.check("3E. at", two.at(2) == 0);

            two.removeEnd(1);
            a.checkEqual("41. size", two.size(), 1U);
            a.check("42. atEnd", two.atEnd(0) == &array[0]);
            a.check("43. atEnd", two.atEnd(1) == 0);
            a.check("44. atEnd", two.atEnd(2) == 0);
            a.check("45. at", two.at(0) == &array[0]);
            a.check("46. at", two.at(1) == 0);
            a.check("47. at", two.at(2) == 0);
        }
    };

    bool checkEmpty(afl::base::Memory<int> p)
    {
        return p.empty();
    }
}

/*
 *  Test successful instantiation for a number of types.
 */
AFL_TEST("afl.base.Memory:instantiate:char", a) {
    Tester<char>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:uchar", a) {
    Tester<unsigned char>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:schar", a) {
    Tester<signed char>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:bool", a) {
    Tester<bool>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:wchar", a) {
    Tester<wchar_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:float", a) {
    Tester<float>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:double", a) {
    Tester<double>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:i16", a) {
    Tester<int16_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:i32", a) {
    Tester<int32_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:i64", a) {
    Tester<int64_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:u16", a) {
    Tester<uint16_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:u32", a) {
    Tester<uint32_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:u64", a) {
    Tester<uint64_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:u8", a) {
    Tester<uint8_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:i8", a) {
    Tester<int8_t>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:string", a) {
    Tester<std::string>::testIt(a);
}
AFL_TEST("afl.base.Memory:instantiate:voidp", a) {
    Tester<void*>::testIt(a);
}

namespace {
    struct Base { };
    struct Derived : Base { };
}

/** Test behaviour with mixed types. */
AFL_TEST_NOARG("afl.base.Memory:mixed") {
    afl::base::Memory<char> pc;
    afl::base::Memory<const char> pcc;

    pcc = pc;

    // Should fail to compile:
    // pc = pcc;

    // These should compile without warnings:
    pc.toBytes();
    pcc.toBytes();

    afl::base::Memory<Base> pb;
    afl::base::Memory<Derived> pd;

    // Should fail to compile:
    // pb = pd;
    // pd = pb;
}

/** Test conversions. */
AFL_TEST("afl.base.Memory:conversion", a) {
    int is[3];
    afl::base::Memory<int> imem(is);
    a.checkEqual("initial size", imem.size(), 3U);

    afl::base::Memory<uint8_t> ibytes(imem.toBytes());
    a.checkEqual("byte size 1", ibytes.size(), sizeof(is));
    a.checkEqual("byte size 2", ibytes.size(), 3 * sizeof(int));

    typedef uint8_t IntBytes[sizeof(int)];
    IntBytes* p = ibytes.eatN<sizeof(int)>();
    a.checkNonNull("eat result", p);
    a.checkEqual("eat remainder", ibytes.size(), 2 * sizeof(int));
}

/** Test conversions involving std::vector. */
AFL_TEST_NOARG("afl.base.Memory:conversion-from-vector")
{
    std::vector<int> vec;
    const std::vector<int> cvec;

    {
        afl::base::Memory<int> mem(vec);
    }
    // Should fail to compile (gives at least a warning):
    // {
    //     afl::base::Memory<int> mem(cvec);
    // }
    {
        afl::base::Memory<const int> mem(cvec);
    }
    {
        afl::base::Memory<const int> mem(cvec);
    }
}

/** Test accessing vector content. */
AFL_TEST("afl.base.Memory:vector", a)
{
    std::vector<int> vec;
    afl::base::Memory<int> mem(vec);
    a.checkEqual("01. size", mem.size(), 0U);

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    mem = vec;
    a.checkEqual("11. size", mem.size(), 3U);
    a.checkEqual("12. eat", *mem.eat(), 1);
    a.checkEqual("13. eat", *mem.eat(), 2);
    a.checkEqual("14. eat", *mem.eat(), 3);
    a.checkEqual("15. eat", mem.eat(), static_cast<int*>(0));

    mem = vec;
    *mem.eat() = 4;
    a.checkEqual("21. vec[]", vec[0], 4);
    a.checkEqual("22. vec[]", vec[1], 2);
    a.checkEqual("23. vec[]", vec[2], 3);
    a.checkEqual("24. size", mem.size(), 2U);  // one already eaten
}

/** Test split. */
AFL_TEST("afl.base.Memory:split", a) {
    int array[100];
    afl::base::Memory<int> mem(array);
    afl::base::Memory<int> mem2(mem.split(20));

    a.checkEqual("01. size", mem.size(), 80U);
    a.checkEqual("02. size", mem2.size(), 20U);

    int m1 = -20;
    afl::base::Memory<int> mem3(mem.splitUpdate(m1));
    a.checkEqual("11. value", m1, -20);
    a.checkEqual("12. size", mem.size(), 80U);
    a.checkEqual("13. size", mem3.size(), 0U);

    long long m2 = 10;
    afl::base::Memory<int> mem4(mem.splitUpdate(m2));
    a.checkEqual("21. value", (uint64_t)m2, (uint64_t)0LL);    // Format() does not handle 'long long'
    a.checkEqual("22. size", mem.size(), 70U);
    a.checkEqual("23. size", mem4.size(), 10U);

    uint8_t m3 = 90;
    afl::base::Memory<int> mem5(mem.splitUpdate(m3));
    a.checkEqual("31. value", m3, 20U);
    a.checkEqual("32. size", mem.size(), 0U);
    a.checkEqual("33. size", mem5.size(), 70U);
}

/** Test fromObject. */
AFL_TEST("afl.base.Memory:fromObject", a) {
    int i;
    afl::base::Bytes_t bytes = afl::base::fromObject(i);
    a.checkEqual("", bytes.size(), sizeof(i));
    a.checkEqual("", static_cast<void*>(bytes.unsafeData()), static_cast<void*>(&i));
}
AFL_TEST("afl.base.Memory:fromObject:const", a) {
    const long cl = 99;
    afl::base::ConstBytes_t constBytes = afl::base::fromObject(cl);
    a.checkEqual("", constBytes.size(), sizeof(cl));
    a.checkEqual("", static_cast<const void*>(constBytes.unsafeData()), static_cast<const void*>(&cl));
}

/** Test find. */
AFL_TEST("afl.base.Memory:find:char", asserter) {
    // char
    char as[] = { 3, 1, 4, 1, 5, 9, 2 };
    afl::base::Memory<const char> a(as);
    asserter.checkEqual("size", a.size(), 7U);
    asserter.checkEqual("find 0", a.find(0), 7U);
    asserter.checkEqual("find 1", a.find(1), 1U);
    asserter.checkEqual("find 2", a.find(2), 6U);
    asserter.checkEqual("find 3", a.find(3), 0U);
    asserter.checkEqual("find 4", a.find(4), 2U);
    asserter.checkEqual("find 5", a.find(5), 4U);
    asserter.checkEqual("find 6", a.find(6), 7U);
    asserter.checkEqual("find 7", a.find(7), 7U);
    asserter.checkEqual("find 8", a.find(8), 7U);
    asserter.checkEqual("find 9", a.find(9), 5U);

    asserter.checkEqual("findFirstOf self",    a.findFirstOf(a), 0U);
    asserter.checkEqual("findFirstOf empty",   a.findFirstOf(afl::base::Memory<const char>()), 7U);
    asserter.checkEqual("findFirstOf range 3", a.findFirstOf(a.subrange(3)), 1U);
    asserter.checkEqual("findFirstOf range 4", a.findFirstOf(a.subrange(4)), 4U);
}

AFL_TEST("afl.base.Memory:find:int", asserter) {
    // pod
    int as[] = { 3, 1, 4, 1, 5, 9, 2 };
    afl::base::Memory<const int> a(as);
    asserter.checkEqual("size", a.size(), 7U);
    asserter.checkEqual("find 0", a.find(0), 7U);
    asserter.checkEqual("find 1", a.find(1), 1U);
    asserter.checkEqual("find 2", a.find(2), 6U);
    asserter.checkEqual("find 3", a.find(3), 0U);
    asserter.checkEqual("find 4", a.find(4), 2U);
    asserter.checkEqual("find 5", a.find(5), 4U);
    asserter.checkEqual("find 6", a.find(6), 7U);
    asserter.checkEqual("find 7", a.find(7), 7U);
    asserter.checkEqual("find 8", a.find(8), 7U);
    asserter.checkEqual("find 9", a.find(9), 5U);

    asserter.checkEqual("findFirstOf self", a.findFirstOf(a), 0U);
    asserter.checkEqual("findFirstOf empty", a.findFirstOf(afl::base::Memory<int>()), 7U);
    asserter.checkEqual("findFirstOf range 3", a.findFirstOf(a.subrange(3)), 1U);
    asserter.checkEqual("findFirstOf range 4", a.findFirstOf(a.subrange(4)), 4U);
}

AFL_TEST("afl.base.Memory:find:string", asserter) {
    // object
    std::string as[] = { "hi", "ho", "ha", "", "hu" };
    afl::base::Memory<std::string> a(as);
    asserter.checkEqual("size", a.size(), 5U);
    asserter.checkEqual("find hi", a.find("hi"), 0U);
    asserter.checkEqual("find ''", a.find(std::string()), 3U);

    asserter.checkEqual("findFirstOf self", a.findFirstOf(a), 0U);
    asserter.checkEqual("findFirstOf empty", a.findFirstOf(afl::base::Memory<std::string>()), 5U);
    asserter.checkEqual("findFirstOf range 3", a.findFirstOf(a.subrange(3)), 3U);
}

/** Test conversion from Nothing. */
AFL_TEST("afl.base.Memory:empty", a) {
    // Conversion through constructor
    int foo[2];
    a.check("implicit empty(Nothing)", checkEmpty(afl::base::Nothing));
    a.check("implicit empty(array)",   !checkEmpty(foo));

    long bar[3];
    afl::base::Memory<long> ml;
    ml = afl::base::Nothing;
    a.check("explicit empty(Nothing)", ml.empty());
    ml = bar;
    a.check("explicit empty(array)", !ml.empty());
}

/** Test fullRead(). */
AFL_TEST("afl.base.Memory:fullRead", a) {
    static const uint8_t in[] = {1,2,3,4,5};
    afl::base::Memory<const uint8_t> testee(in);

    // Read first part
    uint8_t out[2];
    bool ok = testee.fullRead(out);
    a.check("01. ok", ok);
    a.checkEqual("02. out", out[0], 1);
    a.checkEqual("03. out", out[1], 2);
    a.checkEqual("04. size", testee.size(), 3U);

    // Read second part
    ok = testee.fullRead(out);
    a.check("11. ok", ok);
    a.checkEqual("12. out", out[0], 3);
    a.checkEqual("13. out", out[1], 4);
    a.checkEqual("14. size", testee.size(), 1U);

    // No complete part remains; reading returns an error and keeps output unchanged
    ok = testee.fullRead(out);
    a.check("21. ok", !ok);
    a.checkEqual("22. out", out[0], 3);
    a.checkEqual("23. out", out[1], 4);
    a.checkEqual("24. size", testee.size(), 0U);

    // Still no result
    ok = testee.fullRead(out);
    a.check("31. ok", !ok);
}

/** Test findNot(). */
AFL_TEST("afl.base.Memory:findNot", as) {
    // Normal
    static const uint8_t a[] = {1,1,2,3};
    afl::base::Memory<const uint8_t> ta(a);
    as.checkEqual("01. findNot", ta.findNot(1), 2U);
    as.checkEqual("02. findNot", ta.findNot(3), 0U);

    // Empty
    afl::base::Memory<const uint8_t> tb;
    as.checkEqual("11. findNot", tb.findNot(1), 0U);
    as.checkEqual("12. findNot", tb.findNot(3), 0U);

    // Mismatch
    static const uint8_t c[] = {1,1,1,1,1,1};
    afl::base::Memory<const uint8_t> tc(c);
    as.checkEqual("21. findNot", tc.findNot(1), 6U);
    as.checkEqual("22. findNot", tc.findNot(3), 0U);
}

/** Test copy(). */
AFL_TEST("afl.base.Memory:copy", a) {
    uint8_t bytes[] = {1,2,3,4,5,6,7};
    afl::base::Memory<uint8_t> t1(bytes);

    std::string strings[] = {"1","2","3","4","5","6","7"};
    afl::base::Memory<std::string> t2(strings);

    uint32_t words[] = {1,2,3,4,5,6,7};
    afl::base::Memory<uint32_t> t3(words);

    // Copy 1,2 to 6,7
    //   1,2,3,4,5,1,2
    t1.subrange(5,2).copyFrom(t1);
    t2.subrange(5,2).copyFrom(t2);
    t3.subrange(5,2).copyFrom(t3);

    // Copy 1,2,3,4 to 3,4,5,6
    //   1,2,1,2,3,4,2
    t1.subrange(2,4).copyFrom(t1);
    t2.subrange(2,4).copyFrom(t2);
    t3.subrange(2,4).copyFrom(t3);

    // Copy 4,5(now 2,3) to 1,2
    //   2,3,1,2,3,4,2
    t1.subrange(0,2).copyFrom(t1.subrange(3,2));
    t2.subrange(0,2).copyFrom(t2.subrange(3,2));
    t3.subrange(0,2).copyFrom(t3.subrange(3,2));

    // Verify
    a.checkEqual("bytes[0]", bytes[0], 2);
    a.checkEqual("bytes[1]", bytes[1], 3);
    a.checkEqual("bytes[2]", bytes[2], 1);
    a.checkEqual("bytes[3]", bytes[3], 2);
    a.checkEqual("bytes[4]", bytes[4], 3);
    a.checkEqual("bytes[5]", bytes[5], 4);
    a.checkEqual("bytes[6]", bytes[6], 2);

    a.checkEqual("strings[0]", strings[0], "2");
    a.checkEqual("strings[1]", strings[1], "3");
    a.checkEqual("strings[2]", strings[2], "1");
    a.checkEqual("strings[3]", strings[3], "2");
    a.checkEqual("strings[4]", strings[4], "3");
    a.checkEqual("strings[5]", strings[5], "4");
    a.checkEqual("strings[6]", strings[6], "2");

    a.checkEqual("words[0]", words[0], 2U);
    a.checkEqual("words[1]", words[1], 3U);
    a.checkEqual("words[2]", words[2], 1U);
    a.checkEqual("words[3]", words[3], 2U);
    a.checkEqual("words[4]", words[4], 3U);
    a.checkEqual("words[5]", words[5], 4U);
    a.checkEqual("words[6]", words[6], 2U);
}
