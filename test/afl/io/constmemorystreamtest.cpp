/**
  *  \file test/afl/io/constmemorystreamtest.cpp
  *  \brief Test for afl::io::ConstMemoryStream
  */

#include "afl/io/constmemorystream.hpp"

#include "afl/except/fileproblemexception.hpp"
#include "afl/test/testrunner.hpp"

AFL_TEST("afl.io.ConstMemoryStream", a)
{
    const uint8_t data[4] = { 1, 2, 3, 4 };
    afl::io::ConstMemoryStream mem(data);

    // Check basic properties
    a.checkEqual("01. getSize", mem.getSize(), 4U);
    a.checkEqual("02. getPos", mem.getPos(), 0U);
    a.check("03. hasCapabilities", mem.hasCapabilities(mem.CanRead | mem.CanSeek));
    a.check("04. hasCapabilities", !mem.hasCapabilities(mem.CanWrite));

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

    // Try fullRead at end
    mem.setPos(3);
    AFL_CHECK_THROWS(a("51. fullRead"), mem.fullRead(r), afl::except::FileProblemException);
}
