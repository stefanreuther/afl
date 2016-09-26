/**
  *  \file u/t_checksums_crc16.cpp
  *  \brief Test for afl::checksums::CRC16
  */

#include "afl/checksums/crc16.hpp"

#include "u/t_checksums.hpp"

/** Simple test. */
void
TestChecksumsCRC16::testIt()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::CRC16 t(0x8408);

    // Check empty
    TS_ASSERT_EQUALS(t.add(Memory_t(), 0), 0);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999), 9999);

    // "hi" (example verified against PHost)
    uint8_t hi[2] = { 'h', 'i' };
    TS_ASSERT_EQUALS(t.add(Memory_t(hi), 0), 0x5552);

    // 256 bytes
    uint8_t bytes[256];
    for (size_t i = 0; i < sizeof(bytes); ++i) {
        bytes[i] = uint8_t(i);
    }
    TS_ASSERT_EQUALS(t.add(Memory_t(bytes), 0), 0xD841);

    // Verify associativity
    for (size_t i = 0; i <= sizeof(bytes); ++i) {
        Memory_t wholeBlock(bytes);

        Memory_t secondHalf(bytes);
        Memory_t firstHalf(secondHalf.split(i));

        TS_ASSERT_EQUALS(t.add(wholeBlock, 0), t.add(secondHalf, t.add(firstHalf, 0)));
    }
}
