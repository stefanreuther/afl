/**
  *  \file u/t_checksums_crc32.cpp
  *  \brief Test for afl::checksums::CRC32
  */

#include <memory>
#include "afl/checksums/crc32.hpp"

#include "u/t_checksums.hpp"

/** Simple test. */
void
TestChecksumsCRC32::testIt()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::CRC32 t(0xEDB88320);

    // Check empty
    TS_ASSERT_EQUALS(t.add(Memory_t(), 0), 0U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999), 9999U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999999), 9999999U);

    // "hi" (example verified against gzip)
    uint8_t hi[2] = { 'h', 'i' };
    TS_ASSERT_EQUALS(t.add(Memory_t(hi), 0), 0xD8932AACU);

    // 256 bytes
    uint8_t bytes[256];
    for (size_t i = 0; i < sizeof(bytes); ++i) {
        bytes[i] = uint8_t(i);
    }
    TS_ASSERT_EQUALS(t.add(Memory_t(bytes), 0), 0x29058C73U);

    // Verify associativity
    for (size_t i = 0; i <= sizeof(bytes); ++i) {
        Memory_t wholeBlock(bytes);

        Memory_t secondHalf(bytes);
        Memory_t firstHalf(secondHalf.split(i));

        TS_ASSERT_EQUALS(t.add(wholeBlock, 0), t.add(secondHalf, t.add(firstHalf, 0)));
    }

    // Inquiry
    TS_ASSERT_EQUALS(t.bits(), 32U);
}

/** Test using the interface. */
void
TestChecksumsCRC32::testInterface()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::CRC32(0xEDB88320));

    // Check empty
    TS_ASSERT_EQUALS(t->add(Memory_t(), 9999), 9999U);

    // "hi" (example verified against gzip)
    uint8_t hi[2] = { 'h', 'i' };
    TS_ASSERT_EQUALS(t->add(hi, 0), 0xD8932AACU);

    // Inquiry
    TS_ASSERT_EQUALS(t->bits(), 32U);
}
