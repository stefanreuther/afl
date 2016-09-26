/**
  *  \file u/t_charset.hpp
  *  \brief Test for afl::charset
  */
#ifndef AFL_U_T_CHARSET_HPP
#define AFL_U_T_CHARSET_HPP

#include <cxxtest/TestSuite.h>

class TestCharsetBase64 : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetCharset : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetCharsetFactory : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetCodepage : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetCodepageCharset : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetDefaultCharsetFactory : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetQuotedPrintable : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetUnicode : public CxxTest::TestSuite {
 public:
    void testErrorChar();
    void testCase();
};

class TestCharsetUrlEncoding : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetUtf8 : public CxxTest::TestSuite {
 public:
    void testEncDec();
    void testDecEnc();
    void testFuzz();
    void testLength();
    void testSubstr();
    void testPos();
};

class TestCharsetUtf8Charset : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestCharsetUtf8Reader : public CxxTest::TestSuite {
 public:
    void testDecoder();
    void testSequence();
};

#endif
