/**
  *  \file test/afl/base/growablememorytest.cpp
  *  \brief Test for afl::base::GrowableMemory
  */

#include "afl/base/growablememory.hpp"
#include "afl/test/testrunner.hpp"

namespace {
    template<typename T>
    struct Tester {
        static void testIt(const afl::test::Assert& a, T specimen) {
            // Construct a blank one
            afl::base::GrowableMemory<T> g;
            a.check("initial: empty", g.empty());
            a.checkEqual("initial: size", g.size(), 0U);

            // Append two elements
            g.appendN(specimen, 2);
            a.checkEqual("add: size", g.size(), 2U);

            // Basic checks
            afl::base::GrowableMemory<T> blank;
            a.check("equalContent: not blank", !g.equalContent(blank));
            a.check("compareContent: after blank", g.compareContent(blank) > 0);
            a.check("compareContent: blank before", blank.compareContent(g) < 0);
            a.check("copyFrom: empty", g.copyFrom(blank).empty());

            // Resizing
            g.resize(10);
            a.checkEqual("resize: size 10", g.size(), 10U);
            g.ensureSize(20);
            a.checkEqual("ensureSize: size 20", g.size(), 20U);
            g.ensureSize(5);
            a.checkEqual("ensureSize: size 5", g.size(), 20U);
            g.trim(8);
            a.checkEqual("trim: size 8", g.size(), 8U);
            g.trim(10);
            a.checkEqual("trim: size 10", g.size(), 8U);
            g.resize(5);
            a.checkEqual("resize: size 5", g.size(), 5U);

            g.clear();
            a.checkEqual("clear: size", g.size(), 0U);

            // End access
            g.resize(10);
            a.check("at 9", g.at(9) == g.at(0) + 9);
            a.check("atEnd 9", g.atEnd(0) == g.atEnd(9) + 9);
            a.check("atEnd 0", g.atEnd(0) == g.at(9));
            a.check("atEnd 10", g.atEnd(10) == 0);
            a.check("at 10", g.at(10) == 0);

            g.removeEnd(3);
            a.check("removeEnd(3): at 6", g.at(6) == g.at(0) + 6);
            a.check("removeEnd(3): atEnd 6", g.atEnd(0) == g.atEnd(6) + 6);
            a.check("removeEnd(3): atEnd 0", g.atEnd(0) == g.at(6));
            a.check("removeEnd(3): atEnd 7", g.atEnd(7) == 0);
            a.check("removeEnd(3): at 7", g.at(7) == 0);

            g.removeEnd(10);
            a.checkEqual("removeEnd: size", g.size(), 0U);
            a.check("removeEnd(10): at 0", g.at(0) == 0);
            a.check("removeEnd(10): atEnd 0", g.atEnd(0) == 0);
        }
    };
}

/*
 *  Test various instantiations
 */

AFL_TEST("afl.base.GrowableMemory:instantiate:char", a) {
    Tester<char>::testIt(a, 'a');
}

AFL_TEST("afl.base.GrowableMemory:instantiate:uchar", a) {
    Tester<unsigned char>::testIt(a, 9);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:schar", a) {
    Tester<signed char>::testIt(a, 9);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:bool", a) {
    Tester<bool>::testIt(a, true);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:wchar", a) {
    Tester<wchar_t>::testIt(a, L'a');
}

AFL_TEST("afl.base.GrowableMemory:instantiate:float", a) {
    Tester<float>::testIt(a, 2.0);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:double", a) {
    Tester<double>::testIt(a, 3.0);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:i16", a) {
    Tester<int16_t>::testIt(a, 32000);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:i32", a) {
    Tester<int32_t>::testIt(a, 400000);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:i64", a) {
    Tester<int64_t>::testIt(a, 99999999999LL);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:u16", a) {
    Tester<uint16_t>::testIt(a, 10000);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:u32", a) {
    Tester<uint32_t>::testIt(a, 1000000);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:u64", a) {
    Tester<uint64_t>::testIt(a, 99999999999LL);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:u8", a) {
    Tester<uint8_t>::testIt(a, 100);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:i8", a) {
    Tester<int8_t>::testIt(a, -20);
}

AFL_TEST("afl.base.GrowableMemory:instantiate:string", a) {
    Tester<std::string>::testIt(a, "hi");
}

AFL_TEST("afl.base.GrowableMemory:instantiate:void", a) {
    int i;
    Tester<void*>::testIt(a, &i);
}


namespace {
    struct Base { };
    struct Derived : Base { };
}

/** Test behaviour with mixed types. */
AFL_TEST_NOARG("afl.base.GrowableMemory:mixed-types") {
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
AFL_TEST("afl.base.GrowableMemory:conversion", a) {
    afl::base::GrowableMemory<int> imem;
    imem.appendN(1, 3);
    a.checkEqual("size after append", imem.size(), 3U);

    afl::base::Memory<uint8_t> ibytes(imem.toBytes());
    a.checkEqual("size after conversion", ibytes.size(), 3 * sizeof(int));
}

/** Test accessing vector content. */
AFL_TEST("afl.base.GrowableMemory:vector", a) {
    afl::base::GrowableMemory<int> vec;
    a.checkEqual("initial size", vec.size(), 0U);
    vec.append(1);
    vec.append(2);
    vec.append(3);

    afl::base::Memory<int> mem(vec);
    a.checkEqual("mem size", mem.size(), 3U);
    a.checkEqual("mem eat 1", *mem.eat(), 1);
    a.checkEqual("mem eat 2", *mem.eat(), 2);
    a.checkEqual("mem eat 3", *mem.eat(), 3);
    a.checkEqual("mem eat last", mem.eat(), static_cast<int*>(0));

    mem = vec;
    *mem.eat() = 4;
    a.checkEqual("vec at 0", *vec.at(0), 4);
    a.checkEqual("vec at 1", *vec.at(1), 2);
    a.checkEqual("vec at 2", *vec.at(2), 3);
    a.checkEqual("mem size after", mem.size(), 2U);  // one already eaten
}

/** Test duplicating the content by self-append. */
AFL_TEST("afl.base.GrowableMemory:self-append", a) {
    static const int stuff[] = { 1, 2, 3, 4, 5 };
    afl::base::GrowableMemory<int> vec;
    vec.append(stuff);
    a.checkEqual("size 1", vec.size(), 5U);

    // Replicate 1024 times
    for (int i = 0; i < 10; ++i) {
        vec.append(vec);
    }
    a.checkEqual("size 2", vec.size(), 5120U);

    // Verify content
    for (size_t i = 0; i < 5120; ++i) {
        a.check("element present", vec.at(i));
        a.checkEqual("content", *vec.at(i), stuff[i % 5]);
    }
}

/** Test appendN(). */
AFL_TEST("afl.base.GrowableMemory:appendN", a) {
    afl::base::GrowableMemory<char> vec;
    vec.appendN('a', 3).fill('b');
    a.checkEqual("size 1", vec.appendN('x', 2).size(), 2U);
    a.checkEqual("size 2", vec.appendN('y', 0).size(), 0U);
    vec.appendN('c', 5).fill('d');
    a.checkEqual("size 3", vec.size(), 10U);
    a.check("content", vec.equalContent(afl::base::Memory<const char>::unsafeCreate("bbbxxddddd", 10)));
}
