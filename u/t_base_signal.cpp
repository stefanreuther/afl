/**
  *  \file u/t_base_signal.cpp
  *  \brief Test for afl::base::Signal
  */

#include "afl/base/signal.hpp"

#include <string>
#include <cstdio>
#include "u/t_base.hpp"
#include "afl/base/signalconnection.hpp"

/***************************** Parameterless *****************************/

namespace {
    std::string g_accum;

    class Receiver {
     public:
        int i;
        afl::base::SignalConnection conn;
        Receiver(int i, afl::base::Signal<void()>& sig)
            : i(i),
              conn(sig.add(this, &Receiver::handle))
            { }
        void handle()
            {
                char tmp[30];
                std::sprintf(tmp, "*Rec%d", i);
                g_accum += tmp;
            }
    };

    void staticFun()
    {
        g_accum += "*stat";
    }

    class LifetimeTester {
     public:
        LifetimeTester(std::string& acc)
            : m_acc(acc)
            { m_acc += "+"; }
        ~LifetimeTester()
            { m_acc += "-"; }
        void handle()
            { m_acc += "h"; }
     private:
        LifetimeTester(const LifetimeTester&);
        std::string& m_acc;
    };

}

/** Test signal connect/disconnect with nullary signal. */
void
TestBaseSignal::testFirst()
{
    // explicit signal connect/disconnect; disconnect first handler
    afl::base::Signal<void()> sig;
    {
        Receiver rec(0, sig);
        sig.add(staticFun);

        g_accum.clear();
        sig.raise();
        TS_ASSERT_EQUALS(g_accum, "*stat*Rec0");
    }
    g_accum.clear();
    sig.raise();
    TS_ASSERT_EQUALS(g_accum, "*stat");
}

/** Test signal connect/disconnect with nullary signal. */
void
TestBaseSignal::testSecond()
{
    // explicit signal connect/disconnect; disconnect second handler
    afl::base::Signal<void()> sig;
    {
        sig.add(staticFun);
        Receiver rec(0, sig);

        g_accum.clear();
        sig.raise();
        TS_ASSERT_EQUALS(g_accum, "*Rec0*stat");
    }
    g_accum.clear();
    sig.raise();
    TS_ASSERT_EQUALS(g_accum, "*stat");
}

/** Test death of signal object with nullary signal. */
void
TestBaseSignal::testSigDies()
{
    // signal dies before receiver
    afl::base::Signal<void()>* sig = new afl::base::Signal<void()>();
    Receiver rec(0, *sig);

    TS_ASSERT(rec.conn.isConnected());

    g_accum.clear();
    sig->raise();
    TS_ASSERT_EQUALS(g_accum, "*Rec0");

    delete sig;
    TS_ASSERT(!rec.conn.isConnected());
}

/***************************** With Argument *****************************/

namespace {
    class ReceiverArg {
     public:
        int i;
        afl::base::SignalConnection conn;
        ReceiverArg(int i, afl::base::Signal<void(int)>& sig)
            : i(i), conn(sig.add(this, &ReceiverArg::handle))
            { }
        void handle(int n)
            {
                char tmp[40];
                std::sprintf(tmp, "*Rec%d,%d", i, n);
                g_accum += tmp;
            }
    };

    void staticFunArg(int n)
    {
        char tmp[30];
        std::sprintf(tmp, "*stat%d", n);
        g_accum += tmp;
    }
}

/** Test signal connect/disconnect with unary signal. */
void
TestBaseSignal::testFirstArg()
{
    // explicit signal connect/disconnect; disconnect first handler; with arg
    afl::base::Signal<void(int)> sig;
    {
        ReceiverArg rec(0, sig);
        sig.add(staticFunArg);

        g_accum.clear();
        sig.raise(99);
        TS_ASSERT_EQUALS(g_accum, "*stat99*Rec0,99");
    }
    g_accum.clear();
    sig.raise(88);
    TS_ASSERT_EQUALS(g_accum, "*stat88");
}

/** Test signal connect/disconnect with unary signal. */
void
TestBaseSignal::testSecondArg()
{
    // explicit signal connect/disconnect; disconnect second handler; with arg
    afl::base::Signal<void(int)> sig;
    {
        // we use staticFun here to test invocation without args
        sig.add(staticFun);
        ReceiverArg rec(0, sig);

        g_accum.clear();
        sig.raise(99);
        TS_ASSERT_EQUALS(g_accum, "*Rec0,99*stat");
    }
    g_accum.clear();
    sig.raise(88);
    TS_ASSERT_EQUALS(g_accum, "*stat");
}

/** Test death of signal object with unary signal. */
void
TestBaseSignal::testSigDiesArg()
{
    // signal dies before receiver
    afl::base::Signal<void(int)>* sig = new afl::base::Signal<void(int)>();
    ReceiverArg rec(0, *sig);

    TS_ASSERT(rec.conn.isConnected());

    g_accum.clear();
    sig->raise(77);
    TS_ASSERT_EQUALS(g_accum, "*Rec0,77");

    delete sig;
    TS_ASSERT(!rec.conn.isConnected());
}

/** Test multiple static signal handlers. */
void
TestBaseSignal::testMulti()
{
    // four identical handlers
    afl::base::Signal<void()> sig;
    sig.add(staticFun);
    sig.add(staticFun);
    sig.add(staticFun);
    sig.add(staticFun);

    g_accum.clear();
    sig.raise();
    TS_ASSERT_EQUALS(g_accum, "*stat*stat*stat*stat");
}

/** Test multiple bound signal handlers. */
void
TestBaseSignal::testMultiRec()
{
    // four objects
    afl::base::Signal<void()> sig;
    Receiver r1(0, sig), r2(1, sig), r3(2, sig), r4(3, sig);

    g_accum.clear();
    sig.raise();
    TS_ASSERT_EQUALS(g_accum, "*Rec3*Rec2*Rec1*Rec0");
}

namespace {
    /* The distinct part of the Signal2..Signal6 is the 'raise' implementation.
       A possible error case could be type errors in there.
       Thus, we test with six distinct types.

       The code to handle registration and deregistration is identical to the
       Signal0/Signal1 version and already well-tested above. */
    enum One { one };
    enum Two { two };
    enum Three { three };
    enum Four { four };
    enum Five { five };
    enum Six { six };

    void twoArgs(One, Two)
    {
        g_accum += "two";
    }

    void threeArgs(One, Two, Three)
    {
        g_accum += "three";
    }

    void fourArgs(One, Two, Three, Four)
    {
        g_accum += "four";
    }

    void fiveArgs(One, Two, Three, Four, Five)
    {
        g_accum += "five";
    }

    void sixArgs(One, Two, Three, Four, Five, Six)
    {
        g_accum += "six";
    }
}

/** Test multiple-argument signal handlers. */
void
TestBaseSignal::testManyArgs()
{
    {
        afl::base::Signal<void(One, Two)> sig;
        sig.add(twoArgs);
        g_accum.clear();
        sig.raise(one, two);
        TS_ASSERT_EQUALS(g_accum, "two");
    }

    {
        afl::base::Signal<void(One, Two, Three)> sig;
        sig.add(threeArgs);
        g_accum.clear();
        sig.raise(one, two, three);
        TS_ASSERT_EQUALS(g_accum, "three");
    }

    {
        afl::base::Signal<void(One, Two, Three, Four)> sig;
        sig.add(fourArgs);
        g_accum.clear();
        sig.raise(one, two, three, four);
        TS_ASSERT_EQUALS(g_accum, "four");
    }

    {
        afl::base::Signal<void(One, Two, Three, Four, Five)> sig;
        sig.add(fiveArgs);
        g_accum.clear();
        sig.raise(one, two, three, four, five);
        TS_ASSERT_EQUALS(g_accum, "five");
    }

    {
        afl::base::Signal<void(One, Two, Three, Four, Five, Six)> sig;
        sig.add(sixArgs);
        g_accum.clear();
        sig.raise(one, two, three, four, five, six);
        TS_ASSERT_EQUALS(g_accum, "six");
    }
}

namespace {
    template<int n>
    int returnValue()
    {
        return n;
    }
}

/** Test result merging behaviour. */
void
TestBaseSignal::testResult()
{
    afl::base::Signal<int()> sig;
    TS_ASSERT_EQUALS(sig.raise(), 0);

    afl::base::SignalConnection conn1(sig.add(returnValue<5>));
    TS_ASSERT_EQUALS(sig.raise(), 5);

    afl::base::SignalConnection conn2(sig.add(returnValue<9>));
    TS_ASSERT_EQUALS(sig.raise(), 5);

    afl::base::SignalConnection conn3(sig.add(returnValue<3>));
    TS_ASSERT_EQUALS(sig.raise(), 5);

    // dropping first one changes result
    conn1.disconnect();
    TS_ASSERT_EQUALS(sig.raise(), 9);

    // dropping last one does not change result
    conn3.disconnect();
    TS_ASSERT_EQUALS(sig.raise(), 9);

    // dropping middle one, signal now empty
    conn2.disconnect();
    TS_ASSERT_EQUALS(sig.raise(), 0);
}

namespace {
    int sum1(int a) { return a; }
    int sum2(int a, int b) { return a+b; }
    int sum3(int a, int b, int c) { return a+b+c; }
    int sum4(int a, int b, int c, int d) { return a+b+c+d; }
    int sum5(int a, int b, int c, int d, int e) { return a+b+c+d+e; }
    int sum6(int a, int b, int c, int d, int e, int f) { return a+b+c+d+e+f; }
}

/** Test result handling for signals with arguments. */
void
TestBaseSignal::testResult2()
{
    {
        afl::base::Signal<int(int)> sig;
        TS_ASSERT_EQUALS(sig.raise(9), 0);
        sig.add(sum1);
        TS_ASSERT_EQUALS(sig.raise(9), 9);
    }
    {
        afl::base::Signal<int(int,int)> sig;
        TS_ASSERT_EQUALS(sig.raise(9,5), 0);
        sig.add(sum2);
        TS_ASSERT_EQUALS(sig.raise(9,5), 14);
    }
    {
        afl::base::Signal<int(int,int,int)> sig;
        TS_ASSERT_EQUALS(sig.raise(2,9,5), 0);
        sig.add(sum3);
        TS_ASSERT_EQUALS(sig.raise(2,9,5), 16);
    }
    {
        afl::base::Signal<int(int,int,int,int)> sig;
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5), 0);
        sig.add(sum4);
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5), 63);
    }
    {
        afl::base::Signal<int(int,int,int,int,int)> sig;
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5,12), 0);
        sig.add(sum5);
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5,12), 75);
    }
    {
        afl::base::Signal<int(int,int,int,int,int,int)> sig;
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5,4,10), 0);
        sig.add(sum6);
        TS_ASSERT_EQUALS(sig.raise(2,47,9,5,4,10), 77);
    }
}

/** Test Signal.addNew(). */
void
TestBaseSignal::testNew()
{
    std::string log;
    {
        afl::base::Signal<void()> sig;
        sig.addNew(new LifetimeTester(log), &LifetimeTester::handle);
        sig.raise();
        sig.raise();
    }
    TS_ASSERT_EQUALS(log, "+hh-");
}
