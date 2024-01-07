/**
  *  \file test/afl/checksums/crc16test.cpp
  *  \brief Test for afl::checksums::CRC16
  */

#include "afl/checksums/crc16.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.checksums.CRC16", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::CRC16 t(0x8408);

    // Check empty
    a.checkEqual("empty 1", t.add(Memory_t(), 0), 0);
    a.checkEqual("empty 2", t.add(Memory_t(), 9999), 9999);

    // "hi" (example verified against PHost)
    uint8_t hi[2] = { 'h', 'i' };
    a.checkEqual("testcase", t.add(Memory_t(hi), 0), 0x5552);

    // 256 bytes
    uint8_t bytes[256];
    for (size_t i = 0; i < sizeof(bytes); ++i) {
        bytes[i] = uint8_t(i);
    }
    a.checkEqual("bytes", t.add(Memory_t(bytes), 0), 0xD841);

    // Verify associativity
    for (size_t i = 0; i <= sizeof(bytes); ++i) {
        Memory_t wholeBlock(bytes);

        Memory_t secondHalf(bytes);
        Memory_t firstHalf(secondHalf.split(i));

        a.checkEqual("partial", t.add(wholeBlock, 0), t.add(secondHalf, t.add(firstHalf, 0)));
    }

    // Test inquiry
    a.checkEqual("bits", t.bits(), 16U);
}

AFL_TEST("afl.checksums.CRC16:getDefaultInstance", a)
{
    uint8_t hi[2] = { 'h', 'i' };
    a.checkEqual("", afl::checksums::CRC16::getDefaultInstance().add(hi, 0), 0x5552);
}

/** Test using the interface. */
AFL_TEST("afl.checksums.CRC16:interface", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::CRC16(0x8408));

    // Check empty
    a.checkEqual("empty", t->add(Memory_t(), 9999), 9999U);

    // "hi"
    uint8_t hi[2] = { 'h', 'i' };
    a.checkEqual("bytes", t->add(hi, 0), 0x5552U);

    // Inquiry
    a.checkEqual("bits", t->bits(), 16U);
}
