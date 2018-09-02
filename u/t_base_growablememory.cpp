/**
  *  \file u/t_base_growablememory.cpp
  *  \brief Test for afl::base::GrowableMemory
  */

#include <string>
#include "afl/base/growablememory.hpp"

#include "t_base.hpp"

namespace {
    template<typename T>
    struct Tester {
        static void testIt(T specimen) {
            // Construct a blank one
            afl::base::GrowableMemory<T> g;
            TS_ASSERT(g.empty());
            TS_ASSERT_EQUALS(g.size(), 0U);

            // Append two elements
            g.appendN(specimen, 2);
            TS_ASSERT_EQUALS(g.size(), 2U);

            // Basic checks
            afl::base::GrowableMemory<T> blank;
            TS_ASSERT(!g.equalContent(blank));
            TS_ASSERT(g.compareContent(blank) > 0);
            TS_ASSERT(blank.compareContent(g) < 0);
            TS_ASSERT(g.copyFrom(blank).empty());

            // Resizing
            g.resize(10);
            TS_ASSERT_EQUALS(g.size(), 10U);
            g.ensureSize(20);
            TS_ASSERT_EQUALS(g.size(), 20U);
            g.ensureSize(5);
            TS_ASSERT_EQUALS(g.size(), 20U);
            g.trim(8);
            TS_ASSERT_EQUALS(g.size(), 8U);
            g.trim(10);
            TS_ASSERT_EQUALS(g.size(), 8U);
            g.resize(5);
            TS_ASSERT_EQUALS(g.size(), 5U);

            g.clear();
            TS_ASSERT_EQUALS(g.size(), 0U);

            // End access
            g.resize(10);
            TS_ASSERT(g.at(9) == g.at(0) + 9);
            TS_ASSERT(g.atEnd(0) == g.atEnd(9) + 9);
            TS_ASSERT(g.atEnd(0) == g.at(9));
            TS_ASSERT(g.atEnd(10) == 0);
            TS_ASSERT(g.at(10) == 0);

            g.removeEnd(3);
            TS_ASSERT(g.at(6) == g.at(0) + 6);
            TS_ASSERT(g.atEnd(0) == g.atEnd(6) + 6);
            TS_ASSERT(g.atEnd(0) == g.at(6));
            TS_ASSERT(g.atEnd(7) == 0);
            TS_ASSERT(g.at(7) == 0);

            g.removeEnd(10);
            TS_ASSERT_EQUALS(g.size(), 0U);
            TS_ASSERT(g.at(0) == 0);
            TS_ASSERT(g.atEnd(0) == 0);
        }
    };
}

/** Test various instantiations. */
void
TestBaseGrowableMemory::testInstantiation()
{
    Tester<char>::testIt('a');
    Tester<unsigned char>::testIt(9);
    Tester<signed char>::testIt(9);
    Tester<bool>::testIt(true);
    Tester<wchar_t>::testIt(L'a');
    Tester<float>::testIt(2.0);
    Tester<double>::testIt(3.0);
    Tester<int16_t>::testIt(32000);
    Tester<int32_t>::testIt(400000);
    Tester<int64_t>::testIt(99999999999LL);
    Tester<uint16_t>::testIt(10000);
    Tester<uint32_t>::testIt(1000000);
    Tester<uint64_t>::testIt(99999999999LL);

    Tester<uint8_t>::testIt(100);
    Tester<int8_t>::testIt(-20);

    Tester<std::string>::testIt("hi");

    int i;
    Tester<void*>::testIt(&i);
}

namespace {
    struct Base { };
    struct Derived : Base { };
}

/** Test behaviour with mixed types. */
void
TestBaseGrowableMemory::testMix()
{
    afl::base::GrowableMemory<char> pc;
    afl::base::GrowableMemory<const char> pcc;

    afl::base::Memory<char> mpc;
    afl::base::Memory<const char> mpcc;

    mpc = pc;
    mpcc = pc;
    mpcc = pcc;

    // Should fail to compile:
    // mpc = pcc;

    // These should compile without warnings:
    pc.toBytes();
    pcc.toBytes();

    afl::base::GrowableMemory<Base> pb;
    afl::base::GrowableMemory<Derived> pd;

    afl::base::Memory<Base> mpb;
    afl::base::Memory<Derived> mpd;

    mpb = pb;
    mpd = pd;

    // Should fail to compile:
    // mpb = pd;
    // mpd = pb;
}

/** Test conversions. */
void
TestBaseGrowableMemory::testConversion()
{
    afl::base::GrowableMemory<int> imem;
    imem.appendN(1, 3);
    TS_ASSERT_EQUALS(imem.size(), 3U);

    afl::base::Memory<uint8_t> ibytes(imem.toBytes());
    TS_ASSERT_EQUALS(ibytes.size(), 3 * sizeof(int));
}

/** Test accessing vector content. */
void
TestBaseGrowableMemory::testVectorContent()
{
    afl::base::GrowableMemory<int> vec;
    TS_ASSERT_EQUALS(vec.size(), 0U);
    vec.append(1);
    vec.append(2);
    vec.append(3);

    afl::base::Memory<int> mem(vec);
    TS_ASSERT_EQUALS(mem.size(), 3U);
    TS_ASSERT_EQUALS(*mem.eat(), 1);
    TS_ASSERT_EQUALS(*mem.eat(), 2);
    TS_ASSERT_EQUALS(*mem.eat(), 3);
    TS_ASSERT_EQUALS(mem.eat(), static_cast<int*>(0));

    mem = vec;
    *mem.eat() = 4;
    TS_ASSERT_EQUALS(*vec.at(0), 4);
    TS_ASSERT_EQUALS(*vec.at(1), 2);
    TS_ASSERT_EQUALS(*vec.at(2), 3);
    TS_ASSERT_EQUALS(mem.size(), 2U);  // one already eaten
}

/** Test duplicating the content by self-append. */
void
TestBaseGrowableMemory::testDuplicate()
{
    static const int stuff[] = { 1, 2, 3, 4, 5 };
    afl::base::GrowableMemory<int> vec;
    vec.append(stuff);
    TS_ASSERT_EQUALS(vec.size(), 5U);

    // Replicate 1024 times
    for (int i = 0; i < 10; ++i) {
        vec.append(vec);
    }
    TS_ASSERT_EQUALS(vec.size(), 5120U);

    // Verify content
    for (int i = 0; i < 5120; ++i) {
        TS_ASSERT(vec.at(i));
        TS_ASSERT_EQUALS(*vec.at(i), stuff[i % 5]);
    }
}

/** Test appendN(). */
void
TestBaseGrowableMemory::testAppend()
{
    afl::base::GrowableMemory<char> vec;
    vec.appendN('a', 3).fill('b');
    TS_ASSERT_EQUALS(vec.appendN('x', 2).size(), 2U);
    TS_ASSERT_EQUALS(vec.appendN('y', 0).size(), 0U);
    vec.appendN('c', 5).fill('d');
    TS_ASSERT_EQUALS(vec.size(), 10U);
    TS_ASSERT(vec.equalContent(afl::base::Memory<const char>::unsafeCreate("bbbxxddddd", 10)));
}
