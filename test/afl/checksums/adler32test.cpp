/**
  *  \file test/afl/checksums/adler32test.cpp
  *  \brief Test for afl::checksums::Adler32
  */

#include "afl/checksums/adler32.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.checksums.Adler32", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::Adler32 t;

    // Check empty
    a.checkEqual("empty 0",       t.add(Memory_t(), 0), 0U);
    a.checkEqual("empty 1",       t.add(Memory_t(), 1), 1U);
    a.checkEqual("empty 9999",    t.add(Memory_t(), 9999), 9999U);
    a.checkEqual("empty 9999999", t.add(Memory_t(), 9999999), 9999999U);

    // "Wikipedia" (example from precisely there)
    static const uint8_t wikipedia[9] = { 'W', 'i', 'k', 'i', 'p', 'e', 'd', 'i', 'a' };
    a.checkEqual("testcase", t.add(Memory_t(wikipedia), 1), 0x11E60398U);

    // Verify associativity. Since we know that the implementation has a
    // special case at 5500, test a larger block.
    uint8_t large[7000];
    for (size_t i = 0; i < sizeof(large); ++i) {
        large[i] = i & 255;
    }
    for (size_t i = 0; i <= sizeof(large); ++i) {
        Memory_t wholeBlock(large);

        Memory_t secondHalf(large);
        Memory_t firstHalf(secondHalf.split(i));

        a.checkEqual("split", t.add(wholeBlock, 1), t.add(secondHalf, t.add(firstHalf, 1)));
    }
}

/** Test using the interface. */
AFL_TEST("afl.checksums.Adler32:interface", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::Adler32());

    // Check empty
    a.checkEqual("empty", t->add(Memory_t(), 9999), 9999U);

    // "Wikipedia"
    static const uint8_t wikipedia[9] = { 'W', 'i', 'k', 'i', 'p', 'e', 'd', 'i', 'a' };
    a.checkEqual("testcase", t->add(wikipedia, 1), 0x11E60398U);

    // Inquiry
    a.checkEqual("bits", t->bits(), 32U);
}
