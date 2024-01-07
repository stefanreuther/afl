/**
  *  \file test/afl/checksums/bytesumtest.cpp
  *  \brief Test for afl::checksums::ByteSum
  */

#include "afl/checksums/bytesum.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.checksums.ByteSum", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;

    afl::checksums::ByteSum t;

    a.checkEqual("empty 1", t.add(Memory_t(), 0), 0U);
    a.checkEqual("empty 2", t.add(Memory_t(), 9999), 9999U);
    a.checkEqual("empty 3", t.add(Memory_t(), 9999999), 9999999U);

    uint8_t one[1] = { 17 };
    a.checkEqual("one 1", t.add(Memory_t(one), 0), 17U);
    a.checkEqual("one 2", t.add(Memory_t(one), 250), 267U);

    uint8_t three[3] = { 128, 128, 255 };
    a.checkEqual("three 1", t.add(Memory_t(three), 0), 511U);
    a.checkEqual("three 2", t.add(Memory_t(three), 511), 1022U);
}

AFL_TEST("afl.checksums.ByteSum:interface", a)
{
    typedef afl::checksums::Checksum::Memory_t Memory_t;
    std::auto_ptr<afl::checksums::Checksum> t(new afl::checksums::ByteSum());

    // Check empty
    a.checkEqual("empty", t->add(Memory_t(), 9999), 9999U);

    // "Wikipedia"
    static const uint8_t three[3] = { 128, 128, 255 };
    a.checkEqual("three", t->add(three, 1), 512U);

    // Inquiry
    a.checkEqual("bits", t->bits(), 32U);
}
