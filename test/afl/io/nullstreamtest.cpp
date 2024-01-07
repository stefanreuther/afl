/**
  *  \file test/afl/io/nullstreamtest.cpp
  *  \brief Test for afl::io::NullStream
  */

#include "afl/io/nullstream.hpp"

#include "afl/except/filetooshortexception.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.io.NullStream", a)
{
    afl::io::NullStream ns;
    a.checkEqual("01. getPos", ns.getPos(), 0U);
    a.checkEqual("02. getSize", ns.getSize(), 0U);

    // Write nothing. This will change the size!
    ns.setPos(99);
    a.checkEqual("11. getPos", ns.getPos(), 99U);
    a.checkEqual("12. getSize", ns.getSize(), 0U);

    size_t n = ns.write(afl::io::Stream::ConstBytes_t());
    a.checkEqual("13. write", n, 0U);
    a.checkEqual("14. getPos", ns.getPos(), 99U);
    a.checkEqual("15. getSize", ns.getSize(), 99U);

    // Write something.
    ns.setPos(200);
    a.checkEqual("21. getPos", ns.getPos(), 200U);
    a.checkEqual("22. getSize", ns.getSize(), 99U);

    uint8_t data[3] = { 0, 1, 2 };
    n = ns.write(afl::io::Stream::ConstBytes_t(data));
    a.checkEqual("23. write", n, 3U);
    a.checkEqual("24. getPos", ns.getPos(), 203U);
    a.checkEqual("25. getSize", ns.getSize(), 203U);

    // Write something at front
    ns.setPos(42);
    a.checkEqual("31. getPos", ns.getPos(), 42U);
    a.checkEqual("32. getSize", ns.getSize(), 203U);

    n = ns.write(afl::io::Stream::ConstBytes_t(data));
    a.checkEqual("33. write", n, 3U);
    a.checkEqual("34. getPos", ns.getPos(), 45U);
    a.checkEqual("35. getSize", ns.getSize(), 203U);

    // Read
    a.checkEqual("41. read", ns.read(afl::io::Stream::Bytes_t(data)), 0U);
    a.checkEqual("42. getPos", ns.getPos(), 45U);
    a.checkEqual("43. getSize", ns.getSize(), 203U);

    AFL_CHECK_THROWS(a("51. fullRead"), ns.fullRead(afl::io::Stream::Bytes_t(data)), afl::except::FileTooShortException);
}
