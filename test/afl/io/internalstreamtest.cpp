/**
  *  \file test/afl/io/internalstreamtest.cpp
  *  \brief Test for afl::io::InternalStream
  */

#include "afl/io/internalstream.hpp"

#include <stdexcept>
#include "afl/except/fileproblemexception.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::ConstBytes_t;

/** Simple test. */
AFL_TEST("afl.io.InternalStream", a)
{
    afl::io::InternalStream mem;

    // Check basic properties
    a.checkEqual("01. getSize", mem.getSize(), 0U);
    a.checkEqual("02. getPos", mem.getPos(), 0U);
    a.check("03. hasCapabilities", mem.hasCapabilities(mem.CanRead | mem.CanWrite | mem.CanSeek));
    a.checkEqual("04. getContent", mem.getContent().size(), 0U);

    // Cannot read yet
    uint8_t r[3];
    size_t n = mem.read(r);
    a.checkEqual("11. read", n, 0U);

    // After end
    mem.setPos(99);
    a.checkEqual("21. getPos", mem.getPos(), 99U);
    n = mem.read(r);
    a.checkEqual("22. read", n, 0U);

    // Write at beginning
    const uint8_t w[2] = { 88, 77 };
    mem.setPos(1);
    n = mem.write(w);
    a.checkEqual("31. write", n, 2U);
    a.checkEqual("32. getPos", mem.getPos(), 3U);
    a.checkEqual("33. getSize", mem.getSize(), 3U);

    // Test content access
    afl::io::Stream::ConstBytes_t content(mem.getContent());
    a.checkEqual("41. size", content.size(), 3U);
    a.checkEqual("42. content", *content.eat(), 0U);
    a.checkEqual("43. content", *content.eat(), 88U);
    a.checkEqual("44. content", *content.eat(), 77U);
    a.checkNull("45. content", content.eat());

    // Read again; must use just-set pointer
    mem.setPos(0);
    n = mem.read(r);
    a.checkEqual("51. read", n, 3U);
    a.checkEqual("52. getPos", mem.getPos(), 3U);
    a.checkEqual("53. data", r[0], 0U);
    a.checkEqual("54. data", r[1], 88U);
    a.checkEqual("55. data", r[2], 77U);

    // Try fullRead at end
    mem.setPos(2);
    AFL_CHECK_THROWS(a("61. fullRead"), mem.fullRead(r), afl::except::FileProblemException);
    a.checkEqual("62. getPos", mem.getPos(), 3U);
}

/** Test overflowing size. */
AFL_TEST("afl.io.InternalStream:size-overflow", a)
{
    afl::io::InternalStream mem;
    mem.setPos(size_t(-1));
    a.checkEqual("getPos", mem.getPos(), size_t(-1));
    a.checkEqual("getSize", mem.getSize(), 0U);

    // Attempting to write 5 bytes will overflow a size_t.
    // (Or it will exhaust memory and throw a bad_alloc.)
    uint8_t data[5] = { 1,2,3,4,5 };
    AFL_CHECK_THROWS(a("write"), mem.write(data), std::exception);
}

/** Test zero-byte boundary case. */
AFL_TEST("afl.io.InternalStream:write:zero-bytes", a)
{
    afl::io::InternalStream mem;
    a.checkEqual("", mem.write(afl::io::InternalStream::ConstBytes_t()), 0U);
}

/** Test name. */
AFL_TEST("afl.io.InternalStream:name", a)
{
    afl::io::InternalStream testee;
    a.checkEqual("getName initial", testee.getName(), "<internal>");

    testee.setName("hi");
    a.checkEqual("getName updated", testee.getName(), "hi");
}

/** Test setWritePermission(). */
AFL_TEST("afl.io.InternalStream:setWritePermission", a)
{
    afl::io::InternalStream testee;

    // Write
    static const uint8_t data[] = {1,2,3};
    testee.write(data);
    a.checkEqual("01. getPos", testee.getPos(), 3U);
    a.checkEqual("02. getSize", testee.getSize(), 3U);
    a.check("03. getContent", testee.getContent().equalContent(data));

    // Write-protect
    testee.setWritePermission(false);

    // Verify
    a.checkEqual("11. getCapabilities", testee.getCapabilities(), afl::io::Stream::CanRead | afl::io::Stream::CanSeek);
    AFL_CHECK_THROWS(a("12. write"), testee.write(data), afl::except::FileProblemException);
    a.checkEqual("13. getPos", testee.getPos(), 3U);
    a.checkEqual("14. getSize", testee.getSize(), 3U);

    // Read
    testee.setPos(0);
    uint8_t data2[sizeof(data)];
    a.checkEqual("21. read", testee.read(data2), sizeof(data2));
    a.checkEqualContent("22. data", ConstBytes_t(data), ConstBytes_t(data2));
}
