/**
  *  \file test/afl/io/bufferedstreamtest.cpp
  *  \brief Test for afl::io::BufferedStream
  */

#include "afl/io/bufferedstream.hpp"

#include "afl/io/constmemorystream.hpp"
#include "afl/io/memorystream.hpp"
#include "afl/string/string.hpp"
#include "afl/test/testrunner.hpp"

using afl::base::ConstBytes_t;

/** Test readByte, peekByte. */
AFL_TEST("afl.io.BufferedStream:bytewise-access", a)
{
    afl::io::ConstMemoryStream cms(afl::string::toBytes("123\012abc"));
    afl::io::BufferedStream bs(cms);

    // 1
    const uint8_t* p;
    p = bs.peekByte();
    a.checkNonNull("01. peekByte", p);
    a.checkEqual("02. peekByte value", *p, '1');

    p = bs.readByte();
    a.checkNonNull("03. readByte", p);
    a.checkEqual("04. readByte value", *p, '1');

    // 2
    p = bs.peekByte();
    a.checkNonNull("11. peekByte", p);
    a.checkEqual("12. peekByte value", *p, '2');

    p = bs.readByte();
    a.checkNonNull("13. readByte", p);
    a.checkEqual("14. readByte value", *p, '2');

    // 3
    p = bs.peekByte();
    a.checkNonNull("21. peekByte", p);
    a.checkEqual("22. peekByte value", *p, '3');

    p = bs.readByte();
    a.checkNonNull("23. readByte", p);
    a.checkEqual("24. readByte value", *p, '3');

    // newline
    p = bs.peekByte();
    a.checkNonNull("31. peekByte", p);
    a.checkEqual("32. peekByte value", *p, 10);

    p = bs.readByte();
    a.checkNonNull("33. readByte", p);
    a.checkEqual("34. readByte value", *p, 10);

    // a
    p = bs.peekByte();
    a.checkNonNull("41. peekByte", p);
    a.checkEqual("42. peekByte value", *p, 'a');

    p = bs.readByte();
    a.checkNonNull("43. readByte", p);
    a.checkEqual("44. readByte value", *p, 'a');

    // b
    p = bs.peekByte();
    a.checkNonNull("51. peekByte", p);
    a.checkEqual("52. peekByte value", *p, 'b');

    p = bs.readByte();
    a.checkNonNull("53. readByte", p);
    a.checkEqual("54. readByte value", *p, 'b');

    // c
    p = bs.peekByte();
    a.checkNonNull("61. peekByte", p);
    a.checkEqual("62. peekByte value", *p, 'c');

    p = bs.readByte();
    a.checkNonNull("63. readByte", p);
    a.checkEqual("64. readByte value", *p, 'c');

    // EOF
    a.checkNull("71. end", bs.peekByte());
    a.checkNull("72. end", bs.readByte());
    a.checkNull("73. end", bs.peekByte());
    a.checkNull("74. end", bs.readByte());
    a.checkNull("75. end", bs.peekByte());
    a.checkNull("76. end", bs.readByte());
}

/** Test mixed (read/write) I/O. */
AFL_TEST("afl.io.BufferedStream:mixed-io", a)
{
    uint8_t mem[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                      'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                      's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
    afl::io::MemoryStream ms(mem);
    afl::io::BufferedStream bs(ms);
    a.checkEqual("01. getPos", bs.getPos(), 0U);
    a.checkEqual("02. getSize", bs.getSize(), 26U);

    const uint8_t* pc;
    pc = bs.readByte();
    a.checkNonNull("21. readByte", pc);
    a.checkEqual("22. value", *pc, 'a');
    pc = bs.readByte();
    a.checkNonNull("23. readByte", pc);
    a.checkEqual("24. value", *pc, 'b');
    a.checkEqual("25. getPos", bs.getPos(), 2U);

    static const uint8_t CD[] = { 'C', 'D' };
    a.checkEqual("31. write", bs.write(CD), 2U);
    a.checkEqual("32. getPos", bs.getPos(), 4U);

    pc = bs.readByte();
    a.checkNonNull("41. readByte", pc);
    a.checkEqual("42. value", *pc, 'e');
    pc = bs.readByte();
    a.checkNonNull("43. readByte", pc);
    a.checkEqual("44. value", *pc, 'f');

    bs.setPos(3);
    a.checkEqual("51. getPos", bs.getPos(), 3U);
    pc = bs.readByte();
    a.checkNonNull("52. readByte", pc);
    a.checkEqual("53. value", *pc, 'D');

    uint8_t tmp[5];
    a.checkEqual("61. read", bs.read(tmp), 5U);
    a.checkEqualContent("62. content", ConstBytes_t(tmp), ConstBytes_t(afl::string::toBytes("efghi")));

    bs.setPos(24);
    a.checkEqual("71. read", bs.read(tmp), 2U);
    a.checkEqualContent("72. content", ConstBytes_t(tmp), ConstBytes_t(afl::string::toBytes("yzghi")));

    bs.flush();
    a.checkEqualContent("99. mem", ConstBytes_t(mem).trim(7), ConstBytes_t(afl::string::toBytes("abCDefg")));
}
