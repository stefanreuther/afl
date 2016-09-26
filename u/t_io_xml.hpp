/**
  *  \file u/t_io_xml.hpp
  *  \brief Test for afl::io::xml
  */
#ifndef AFL_U_T_IO_XML_HPP
#define AFL_U_T_IO_XML_HPP

#include <cxxtest/TestSuite.h>

class TestIoXmlBaseReader : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoXmlDefaultEntityHandler : public CxxTest::TestSuite {
 public:
    void testIt();
    void testDefault();
};

class TestIoXmlEntityHandler : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestIoXmlReader : public CxxTest::TestSuite {
 public:
    void testUtf8();
    void testLatin1();
    void testKOI8R();
    void testUtf16LE();
    void testUtf16BE();
    void testText();
    void testQuote();
};

#endif
