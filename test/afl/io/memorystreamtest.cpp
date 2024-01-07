/**
  *  \file test/afl/io/memorystreamtest.cpp
  *  \brief Test for afl::io::MemoryStream
  */

#include "afl/io/memorystream.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.io.MemoryStream", a)
{
    uint8_t data[4] = { 1, 2, 3, 4 };
    afl::io::MemoryStream mem(data);

    // Check basic properties
    a.checkEqual("01. getSize", mem.getSize(), 4U);
    a.checkEqual("02. getPos", mem.getPos(), 0U);
    a.check("03. hasCapabilities", mem.hasCapabilities(mem.CanRead | mem.CanWrite | mem.CanSeek));

    // Can read
    uint8_t r[3];
    size_t n = mem.read(r);
    a.checkEqual("11. read", n, 3U);
    a.checkEqual("12. getPos", mem.getPos(), 3U);
    a.checkEqual("13. data", r[0], 1U);
    a.checkEqual("14. data", r[1], 2U);
    a.checkEqual("15. data", r[2], 3U);

    n = mem.read(r);
    a.checkEqual("21. read", n, 1U);
    a.checkEqual("22. getPos", mem.getPos(), 4U);
    a.checkEqual("23. data", r[0], 4U);

    n = mem.read(r);
    a.checkEqual("31. read", n, 0U);
    a.checkEqual("32. getPos", mem.getPos(), 4U);

    // After end
    mem.setPos(99);
    a.checkEqual("41. getPos", mem.getPos(), 99U);
    n = mem.read(r);
    a.checkEqual("42. read", n, 0U);

    // Write after end
    n = mem.write(r);
    a.checkEqual("43. write", n, 0U);

    // Write at beginning
    const uint8_t w[2] = { 88, 77 };
    mem.setPos(1);
    n = mem.write(w);
    a.checkEqual("51. write", n, 2U);
    a.checkEqual("52. getPos", mem.getPos(), 3U);
    a.checkEqual("53. data", data[0], 1U);
    a.checkEqual("54. data", data[1], 88U);
    a.checkEqual("55. data", data[2], 77U);
    a.checkEqual("56. data", data[3], 4U);

    // Read again; must use just-set pointer
    n = mem.read(r);
    a.checkEqual("61. read", n, 1U);
    a.checkEqual("62. getPos", mem.getPos(), 4U);

    // Read again; must use just-set data
    mem.setPos(1);
    n = mem.read(r);
    a.checkEqual("71. read", n, 3U);
    a.checkEqual("72. data", r[0], 88U);
    a.checkEqual("73. data", r[1], 77U);
    a.checkEqual("74. data", r[2], 4U);

    // Try fullRead at end
    mem.setPos(3);
    AFL_CHECK_THROWS(a("81. fullRead"), mem.fullRead(r), afl::except::FileProblemException);
    a.checkEqual("82. getPos", mem.getPos(), 4U);
    AFL_CHECK_THROWS(a("83. fullWrite"), mem.fullWrite(w), afl::except::FileProblemException);
}
