/**
  *  \file u/t_sys.hpp
  *  \brief Test for afl::sys
  */
#ifndef AFL_U_T_SYS_HPP
#define AFL_U_T_SYS_HPP

#include <cxxtest/TestSuite.h>

class TestSysAtomicInteger : public CxxTest::TestSuite {
 public:
    void testBasics();
    void testParallelIncrement();
    void testParallelReplace();
};

class TestSysCommandLineParser : public CxxTest::TestSuite {
 public:
    void testIt();
    void testRequired();
};

class TestSysDialog : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestSysDuration : public CxxTest::TestSuite {
 public:
    void testConstruction();
    void testComparison();
    void testArithmetic();
    void testTimeout();
};

class TestSysEnvironment : public CxxTest::TestSuite {
 public:
    void testArgs();
    void testEnv();
    void testSettings();
    void testInterface();
    void testInstallationDirectory();
};

class TestSysError : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestSysInternalEnvironment : public CxxTest::TestSuite {
 public:
    void testEmpty();
    void testCommandLine();
    void testStrings();
    void testStream();
    void testWriter();
    void testReader();
};

class TestSysLog : public CxxTest::TestSuite {
 public:
    void testIt();
    void testLifetime();
};

class TestSysLogListener : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestSysLongCommandLineParser : public CxxTest::TestSuite {
 public:
    void testLong();
    void testLong2();
    void testNonOption();
};

class TestSysMutex : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestSysMutexGuard : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestSysParsedTime : public CxxTest::TestSuite {
 public:
    void testIt();
    void testFormat();
    void testMonths();
    void testWeekdays();
    void testHours();
};

class TestSysSemaphore : public CxxTest::TestSuite {
 public:
    void testSimple();
};

class TestSysStandardCommandLineParser : public CxxTest::TestSuite {
 public:
    void testChars();
    void testLong();
    void testNonOption();
};

class TestSysThread : public CxxTest::TestSuite {
 public:
    void testIt();
    void testSleep();
};

class TestSysTime : public CxxTest::TestSuite {
 public:
    void testConstruction();
    void testConstructionLocal();
    void testComparison();
    void testArithmetic();
    void testCurrent();
    void testToString();
};

class TestSysTypes : public CxxTest::TestSuite {
 public:
    void testTimeout();
};

#endif
