/**
  *  \file u/t_async.hpp
  *  \brief Test for afl::async
  */
#ifndef AFL_U_T_ASYNC_HPP
#define AFL_U_T_ASYNC_HPP

#include <cxxtest/TestSuite.h>

class TestAsyncCancelable : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncCommunicationObject : public CxxTest::TestSuite {
 public:
    void testIt();
    void testFullSend();
    void testFullReceive();
};

class TestAsyncCommunicationSink : public CxxTest::TestSuite {
 public:
    void testMX();
    void testSocket();
    void testTimeout();
};

class TestAsyncCommunicationStream : public CxxTest::TestSuite {
 public:
    void testMX();
    void testSocket();
    void testTimeout();
};

class TestAsyncController : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncInterrupt : public CxxTest::TestSuite {
 public:
    void testDummy();
};

class TestAsyncInterruptOperation : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncMessageExchange : public CxxTest::TestSuite {
 public:
    void testPingPong();
    void testData();
};

class TestAsyncMutex : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testMulti();
};

class TestAsyncNotifier : public CxxTest::TestSuite {
 public:
    void testSimple();
    void testIntegration();
};

class TestAsyncOperation : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncOperationList : public CxxTest::TestSuite {
 public:
    void testBase();
    void testDerived();
};

class TestAsyncReceiveOperation : public CxxTest::TestSuite {
 public:
    void testIt();
    void testCopy();
};

class TestAsyncSemaphore : public CxxTest::TestSuite {
 public:
    void testPingPong();
    void testPingPongMulti();
};

class TestAsyncSendOperation : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncSynchronisationObject : public CxxTest::TestSuite {
 public:
    void testIt();
};

class TestAsyncTimer : public CxxTest::TestSuite {
 public:
    void testIt();
};

#endif
