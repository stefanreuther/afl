/**
  *  \file test/afl/checksums/crc32test.cpp
  *  \brief Test for afl::checksums::CRC32
  */

#include "afl/checksums/crc32.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.checksums.CRC32", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::CRC32 t(0xEDB88320);

    // Check empty
    a.checkEqual("empty 1", t.add(Memory_t(), 0), 0U);
    a.checkEqual("empty 2", t.add(Memory_t(), 9999), 9999U);
    a.checkEqual("empty 3", t.add(Memory_t(), 9999999), 9999999U);

    // "hi" (example verified against gzip)
    uint8_t hi[2] = { 'h', 'i' };
    a.checkEqual("testcase", t.add(Memory_t(hi), 0), 0xD8932AACU);

    // 256 bytes
    uint8_t bytes[256];
    for (size_t i = 0; i < sizeof(bytes); ++i) {
        bytes[i] = uint8_t(i);
    }
    a.checkEqual("bytes", t.add(Memory_t(bytes), 0), 0x29058C73U);

    // Verify associativity
    for (size_t i = 0; i <= sizeof(bytes); ++i) {
        Memory_t wholeBlock(bytes);

        Memory_t secondHalf(bytes);
        Memory_t firstHalf(secondHalf.split(i));

        a.checkEqual("partial", t.add(wholeBlock, 0), t.add(secondHalf, t.add(firstHalf, 0)));
    }

    // Inquiry
    a.checkEqual("bits", t.bits(), 32U);
}

/** Test using the interface. */
AFL_TEST("afl.checksums.CRC32:interface", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::CRC32(0xEDB88320));

    // Check empty
    a.checkEqual("empty", t->add(Memory_t(), 9999), 9999U);

    // "hi" (example verified against gzip)
    uint8_t hi[2] = { 'h', 'i' };
    a.checkEqual("bytes", t->add(hi, 0), 0xD8932AACU);

    // Inquiry
    a.checkEqual("bits", t->bits(), 32U);
}
