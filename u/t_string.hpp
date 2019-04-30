/**
  *  \file u/t_string.hpp
  *  \brief Test for afl::string
  */
#ifndef AFL_U_T_STRING_HPP
#define AFL_U_T_STRING_HPP

#include <cxxtest/TestSuite.h>

class TestStringChar : public CxxTest::TestSuite {
 public:
    void testToLower();
    void testToUpper();
    void testIsLower();
    void testIsUpper();
    void testIsSpace();
    void testIsDigit();
    void testIsAlphanumeric();
};

class TestStringFormat : public CxxTest::TestSuite {
 public:
    void testConv();
    void testTypes();
    void testInt();
    void testUInt();
    void testFP();
    void testIntStr();
    void testReorder();
    void testStream();
    void testSpecials();
    void testPointer();
};

class TestStringFormatState : public CxxTest::TestSuite {
 public:
    void testAccessors();
    void testNumberConditions();
};

class TestStringFormatTraits : public CxxTest::TestSuite {
 public:
    void testTypes();
};

class TestStringFormatUnion : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringHex : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringLanguageCode : public CxxTest::TestSuite {
 public:
    void testBasic();
    void testGeneralize();
    void testIsSameOrMoreGeneral();
};

class TestStringMessages : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringNullTranslator : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringParse : public CxxTest::TestSuite {
 public:
    void testStrToInt();
    void testStrToLL();
    void testStrToFloat();
};

class TestStringParseMemory : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringPosixFileNames : public CxxTest::TestSuite {
 public:
    void testIsAbsolute();
    void testIsPathSep();
    void testMakePath();
    void testGetCanonical();
    void testGetFileName();
    void testGetDirName();
};

class TestStringProxyTranslator : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringString : public CxxTest::TestSuite {
 public:
    void testMemory();
    void testCompare();
    void testTrim();
    void testParse();
    void testCase();
    void testWord();
};

class TestStringTranslator : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestStringWin32FileNames : public CxxTest::TestSuite {
 public:
    void testFlipSlashes();
    void testIsAbsolute();
    void testIsPathSeparator();
    void testMakePath();
    void testGetCanonicalPathName();
    void testGetFileName();
    void testGetDirectoryName();
};

#endif
