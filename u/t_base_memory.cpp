/**
  *  \file u/t_base_memory.cpp
  *  \brief Test for afl::base::Memory
  */

#include <string>
#include "afl/base/memory.hpp"

#include "t_base.hpp"

namespace {
    template<typename T>
    struct Tester {
        static void testIt() {
            // Construct a blank one
            afl::base::Memory<T> blank;
            TS_ASSERT(blank.empty());
            TS_ASSERT_EQUALS(blank.size(), 0U);

            // Construct one with two elements
            T array[2];
            afl::base::Memory<T> two(array);
            TS_ASSERT(!two.empty());
            TS_ASSERT_EQUALS(two.size(), 2U);

            // Basic checks
            TS_ASSERT(!two.equalContent(blank));
            TS_ASSERT(two.compareContent(blank) > 0);
            TS_ASSERT(blank.compareContent(two) < 0);
            TS_ASSERT(two.copyFrom(blank).empty());
            TS_ASSERT_EQUALS(blank.find(T()), 0U);
            TS_ASSERT(blank.atEnd(0) == 0);
            TS_ASSERT(two.atEnd(0) == &array[1]);
            TS_ASSERT(two.atEnd(1) == &array[0]);
            TS_ASSERT(two.atEnd(2) == 0);
            TS_ASSERT(two.at(0) == &array[0]);
            TS_ASSERT(two.at(1) == &array[1]);
            TS_ASSERT(two.at(2) == 0);

            two.removeEnd(1);
            TS_ASSERT_EQUALS(two.size(), 1U);
            TS_ASSERT(two.atEnd(0) == &array[0]);
            TS_ASSERT(two.atEnd(1) == 0);
            TS_ASSERT(two.atEnd(2) == 0);
            TS_ASSERT(two.at(0) == &array[0]);
            TS_ASSERT(two.at(1) == 0);
            TS_ASSERT(two.at(2) == 0);
        }
    };

    bool checkEmpty(afl::base::Memory<int> p)
    {
        return p.empty();
    }
}

/** Test successful instantiation for a number of types. */
void
TestBaseMemory::testInstantiation()
{
    Tester<char>::testIt();
    Tester<unsigned char>::testIt();
    Tester<signed char>::testIt();
    Tester<bool>::testIt();
    Tester<wchar_t>::testIt();
    Tester<float>::testIt();
    Tester<double>::testIt();
    Tester<int16_t>::testIt();
    Tester<int32_t>::testIt();
    Tester<int64_t>::testIt();
    Tester<uint16_t>::testIt();
    Tester<uint32_t>::testIt();
    Tester<uint64_t>::testIt();

    Tester<uint8_t>::testIt();
    Tester<int8_t>::testIt();

    Tester<std::string>::testIt();
    Tester<void*>::testIt();
}

namespace {
    struct Base { };
    struct Derived : Base { };
}

/** Test behaviour with mixed types. */
void
TestBaseMemory::testMix()
{
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
void
TestBaseMemory::testConversion()
{
    int is[3];
    afl::base::Memory<int> imem(is);
    TS_ASSERT_EQUALS(imem.size(), 3U);

    afl::base::Memory<uint8_t> ibytes(imem.toBytes());
    TS_ASSERT_EQUALS(ibytes.size(), sizeof(is));
    TS_ASSERT_EQUALS(ibytes.size(), 3 * sizeof(int));

    typedef uint8_t IntBytes[sizeof(int)];
    IntBytes* p = ibytes.eatN<sizeof(int)>();
    TS_ASSERT(p != 0);
    TS_ASSERT_EQUALS(ibytes.size(), 2 * sizeof(int));
}

/** Test conversions involving std::vector. */
void
TestBaseMemory::testVectorConversion()
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
void
TestBaseMemory::testVectorContent()
{
    std::vector<int> vec;
    afl::base::Memory<int> mem(vec);
    TS_ASSERT_EQUALS(mem.size(), 0U);

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    mem = vec;
    TS_ASSERT_EQUALS(mem.size(), 3U);
    TS_ASSERT_EQUALS(*mem.eat(), 1);
    TS_ASSERT_EQUALS(*mem.eat(), 2);
    TS_ASSERT_EQUALS(*mem.eat(), 3);
    TS_ASSERT_EQUALS(mem.eat(), static_cast<int*>(0));

    mem = vec;
    *mem.eat() = 4;
    TS_ASSERT_EQUALS(vec[0], 4);
    TS_ASSERT_EQUALS(vec[1], 2);
    TS_ASSERT_EQUALS(vec[2], 3);
    TS_ASSERT_EQUALS(mem.size(), 2U);  // one already eaten
}

/** Test split. */
void
TestBaseMemory::testSplit()
{
    int array[100];
    afl::base::Memory<int> mem(array);
    afl::base::Memory<int> mem2(mem.split(20));

    TS_ASSERT_EQUALS(mem.size(), 80U);
    TS_ASSERT_EQUALS(mem2.size(), 20U);

    int m1 = -20;
    afl::base::Memory<int> mem3(mem.splitUpdate(m1));
    TS_ASSERT_EQUALS(m1, -20);
    TS_ASSERT_EQUALS(mem.size(), 80U);
    TS_ASSERT_EQUALS(mem3.size(), 0U);

    long long m2 = 10;
    afl::base::Memory<int> mem4(mem.splitUpdate(m2));
    TS_ASSERT_EQUALS(m2, 0LL);
    TS_ASSERT_EQUALS(mem.size(), 70U);
    TS_ASSERT_EQUALS(mem4.size(), 10U);

    uint8_t m3 = 90;
    afl::base::Memory<int> mem5(mem.splitUpdate(m3));
    TS_ASSERT_EQUALS(m3, 20U);
    TS_ASSERT_EQUALS(mem.size(), 0U);
    TS_ASSERT_EQUALS(mem5.size(), 70U);
}

/** Test fromObject. */
void
TestBaseMemory::testObject()
{
    int i;
    afl::base::Bytes_t bytes = afl::base::fromObject(i);
    TS_ASSERT_EQUALS(bytes.size(), sizeof(i));
    TS_ASSERT_EQUALS(static_cast<void*>(bytes.unsafeData()), static_cast<void*>(&i));

    const long cl = 99;
    afl::base::ConstBytes_t constBytes = afl::base::fromObject(cl);
    TS_ASSERT_EQUALS(constBytes.size(), sizeof(cl));
    TS_ASSERT_EQUALS(static_cast<const void*>(constBytes.unsafeData()), static_cast<const void*>(&cl));
}

/** Test find. */
void
TestBaseMemory::testFind()
{
    {
        // char
        char as[] = { 3, 1, 4, 1, 5, 9, 2 };
        afl::base::Memory<const char> a(as);
        TS_ASSERT_EQUALS(a.size(), 7U);
        TS_ASSERT_EQUALS(a.find(0), 7U);
        TS_ASSERT_EQUALS(a.find(1), 1U);
        TS_ASSERT_EQUALS(a.find(2), 6U);
        TS_ASSERT_EQUALS(a.find(3), 0U);
        TS_ASSERT_EQUALS(a.find(4), 2U);
        TS_ASSERT_EQUALS(a.find(5), 4U);
        TS_ASSERT_EQUALS(a.find(6), 7U);
        TS_ASSERT_EQUALS(a.find(7), 7U);
        TS_ASSERT_EQUALS(a.find(8), 7U);
        TS_ASSERT_EQUALS(a.find(9), 5U);

        TS_ASSERT_EQUALS(a.findFirstOf(a), 0U);
        TS_ASSERT_EQUALS(a.findFirstOf(afl::base::Memory<const char>()), 7U);
        TS_ASSERT_EQUALS(a.findFirstOf(a.subrange(3)), 1U);
        TS_ASSERT_EQUALS(a.findFirstOf(a.subrange(4)), 4U);
    }

    {
        // pod
        int as[] = { 3, 1, 4, 1, 5, 9, 2 };
        afl::base::Memory<const int> a(as);
        TS_ASSERT_EQUALS(a.size(), 7U);
        TS_ASSERT_EQUALS(a.find(0), 7U);
        TS_ASSERT_EQUALS(a.find(1), 1U);
        TS_ASSERT_EQUALS(a.find(2), 6U);
        TS_ASSERT_EQUALS(a.find(3), 0U);
        TS_ASSERT_EQUALS(a.find(4), 2U);
        TS_ASSERT_EQUALS(a.find(5), 4U);
        TS_ASSERT_EQUALS(a.find(6), 7U);
        TS_ASSERT_EQUALS(a.find(7), 7U);
        TS_ASSERT_EQUALS(a.find(8), 7U);
        TS_ASSERT_EQUALS(a.find(9), 5U);

        TS_ASSERT_EQUALS(a.findFirstOf(a), 0U);
        TS_ASSERT_EQUALS(a.findFirstOf(afl::base::Memory<int>()), 7U);
        TS_ASSERT_EQUALS(a.findFirstOf(a.subrange(3)), 1U);
        TS_ASSERT_EQUALS(a.findFirstOf(a.subrange(4)), 4U);
    }

    {
        // object
        std::string as[] = { "hi", "ho", "ha", "", "hu" };
        afl::base::Memory<std::string> a(as);
        TS_ASSERT_EQUALS(a.size(), 5U);
        TS_ASSERT_EQUALS(a.find("hi"), 0U);
        TS_ASSERT_EQUALS(a.find(std::string()), 3U);

        TS_ASSERT_EQUALS(a.findFirstOf(a), 0U);
        TS_ASSERT_EQUALS(a.findFirstOf(afl::base::Memory<std::string>()), 5U);
        TS_ASSERT_EQUALS(a.findFirstOf(a.subrange(3)), 3U);
    }
}

/** Test conversion from Nothing. */
void
TestBaseMemory::testNothing()
{
    // Conversion through constructor
    int foo[2];
    TS_ASSERT(checkEmpty(afl::base::Nothing));
    TS_ASSERT(!checkEmpty(foo));

    long bar[3];
    afl::base::Memory<long> ml;
    ml = afl::base::Nothing;
    TS_ASSERT(ml.empty());
    ml = bar;
    TS_ASSERT(!ml.empty());
}

/** Test fullRead(). */
void
TestBaseMemory::testFullRead()
{
    static const uint8_t in[] = {1,2,3,4,5};
    afl::base::Memory<const uint8_t> testee(in);

    // Read first part
    uint8_t out[2];
    bool ok = testee.fullRead(out);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(out[0], 1);
    TS_ASSERT_EQUALS(out[1], 2);
    TS_ASSERT_EQUALS(testee.size(), 3U);

    // Read second part
    ok = testee.fullRead(out);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(out[0], 3);
    TS_ASSERT_EQUALS(out[1], 4);
    TS_ASSERT_EQUALS(testee.size(), 1U);

    // No complete part remains; reading returns an error and keeps output unchanged
    ok = testee.fullRead(out);
    TS_ASSERT(!ok);
    TS_ASSERT_EQUALS(out[0], 3);
    TS_ASSERT_EQUALS(out[1], 4);
    TS_ASSERT_EQUALS(testee.size(), 0U);

    // Still no result
    ok = testee.fullRead(out);
    TS_ASSERT(!ok);
}

/** Test findNot(). */
void
TestBaseMemory::testFindNot()
{
    // Normal
    static const uint8_t a[] = {1,1,2,3};
    afl::base::Memory<const uint8_t> ta(a);
    TS_ASSERT_EQUALS(ta.findNot(1), 2U);
    TS_ASSERT_EQUALS(ta.findNot(3), 0U);

    // Empty
    afl::base::Memory<const uint8_t> tb;
    TS_ASSERT_EQUALS(tb.findNot(1), 0U);
    TS_ASSERT_EQUALS(tb.findNot(3), 0U);

    // Mismatch
    static const uint8_t c[] = {1,1,1,1,1,1};
    afl::base::Memory<const uint8_t> tc(c);
    TS_ASSERT_EQUALS(tc.findNot(1), 6U);
    TS_ASSERT_EQUALS(tc.findNot(3), 0U);
}

/** Test copy(). */
void
TestBaseMemory::testCopy()
{
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
    TS_ASSERT_EQUALS(bytes[0], 2);
    TS_ASSERT_EQUALS(bytes[1], 3);
    TS_ASSERT_EQUALS(bytes[2], 1);
    TS_ASSERT_EQUALS(bytes[3], 2);
    TS_ASSERT_EQUALS(bytes[4], 3);
    TS_ASSERT_EQUALS(bytes[5], 4);
    TS_ASSERT_EQUALS(bytes[6], 2);

    TS_ASSERT_EQUALS(strings[0], "2");
    TS_ASSERT_EQUALS(strings[1], "3");
    TS_ASSERT_EQUALS(strings[2], "1");
    TS_ASSERT_EQUALS(strings[3], "2");
    TS_ASSERT_EQUALS(strings[4], "3");
    TS_ASSERT_EQUALS(strings[5], "4");
    TS_ASSERT_EQUALS(strings[6], "2");

    TS_ASSERT_EQUALS(words[0], 2U);
    TS_ASSERT_EQUALS(words[1], 3U);
    TS_ASSERT_EQUALS(words[2], 1U);
    TS_ASSERT_EQUALS(words[3], 2U);
    TS_ASSERT_EQUALS(words[4], 3U);
    TS_ASSERT_EQUALS(words[5], 4U);
    TS_ASSERT_EQUALS(words[6], 2U);
}

