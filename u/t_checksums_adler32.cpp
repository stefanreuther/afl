/**
  *  \file u/t_checksums_adler32.cpp
  *  \brief Test for afl::checksums::Adler32
  */

#include "afl/checksums/adler32.hpp"

#include "u/t_checksums.hpp"

/** Simple test. */
void
TestChecksumsAdler32::testIt()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::Adler32 t;

    // Check empty
    TS_ASSERT_EQUALS(t.add(Memory_t(), 0), 0U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 1), 1U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999), 9999U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999999), 9999999U);

    // "Wikipedia" (example from precisely there)
    uint8_t wikipedia[9] = { 'W', 'i', 'k', 'i', 'p', 'e', 'd', 'i', 'a' };
    TS_ASSERT_EQUALS(t.add(Memory_t(wikipedia), 1), 0x11E60398U);

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

        TS_ASSERT_EQUALS(t.add(wholeBlock, 1), t.add(secondHalf, t.add(firstHalf, 1)));
    }
}
