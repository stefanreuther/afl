/**
  *  \file u/t_checksums_bytesum.cpp
  *  \brief Test for afl::checksums::ByteSum
  */

#include <memory>
#include "afl/checksums/bytesum.hpp"

#include "u/t_checksums.hpp"

/** Simple test. */
void
TestChecksumsByteSum::testIt()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::ByteSum t;

    TS_ASSERT_EQUALS(t.add(Memory_t(), 0), 0U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999), 9999U);
    TS_ASSERT_EQUALS(t.add(Memory_t(), 9999999), 9999999U);

    uint8_t one[1] = { 17 };
    TS_ASSERT_EQUALS(t.add(Memory_t(one), 0), 17U);
    TS_ASSERT_EQUALS(t.add(Memory_t(one), 250), 267U);

    uint8_t three[3] = { 128, 128, 255 };
    TS_ASSERT_EQUALS(t.add(Memory_t(three), 0), 511U);
    TS_ASSERT_EQUALS(t.add(Memory_t(three), 511), 1022U);
}

/** Test using the interface. */
void
TestChecksumsByteSum::testInterface()
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::ByteSum());

    // Check empty
    TS_ASSERT_EQUALS(t->add(Memory_t(), 9999), 9999U);

    // "Wikipedia"
    static const uint8_t three[3] = { 128, 128, 255 };
    TS_ASSERT_EQUALS(t->add(three, 1), 512U);

    // Inquiry
    TS_ASSERT_EQUALS(t->bits(), 32U);
}
