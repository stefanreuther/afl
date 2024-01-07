/**
  *  \file test/afl/base/signaltest.cpp
  *  \brief Test for afl::base::Signal
  */

#include <cstdio>
#include "afl/base/signal.hpp"
#include "afl/test/testrunner.hpp"
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
AFL_TEST("afl.base.Signal:nullary:disconnect-first", a) {
    // explicit signal connect/disconnect; disconnect first handler
    afl::base::Signal<void()> sig;
    {
        Receiver rec(0, sig);
        sig.add(staticFun);

        g_accum.clear();
        sig.raise();
        a.checkEqual("after receiver+static", g_accum, "*stat*Rec0");
    }
    g_accum.clear();
    sig.raise();
    a.checkEqual("after disconnect of receiver", g_accum, "*stat");
}

/** Test signal connect/disconnect with nullary signal. */
AFL_TEST("afl.base.Signal:nullary:disconnect-second", a)
{
    // explicit signal connect/disconnect; disconnect second handler
    afl::base::Signal<void()> sig;
    {
        sig.add(staticFun);
        Receiver rec(0, sig);

        g_accum.clear();
        sig.raise();
        a.checkEqual("after static+receiver", g_accum, "*Rec0*stat");
    }
    g_accum.clear();
    sig.raise();
    a.checkEqual("after disconnect of receiver", g_accum, "*stat");
}

/** Test death of signal object with nullary signal. */
AFL_TEST("afl.base.Signal:nullary:signal-dies", a)
{
    // signal dies before receiver
    afl::base::Signal<void()>* sig = new afl::base::Signal<void()>();
    Receiver rec(0, *sig);

    a.check("signal is connected", rec.conn.isConnected());

    g_accum.clear();
    sig->raise();
    a.checkEqual("after raise", g_accum, "*Rec0");

    delete sig;
    a.check("signal is disconnected", !rec.conn.isConnected());
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
AFL_TEST("afl.base.Signal:unary:disconnect-first", a)
{
    // explicit signal connect/disconnect; disconnect first handler; with arg
    afl::base::Signal<void(int)> sig;
    {
        ReceiverArg rec(0, sig);
        sig.add(staticFunArg);

        g_accum.clear();
        sig.raise(99);
        a.checkEqual("after receiver+static", g_accum, "*stat99*Rec0,99");
    }
    g_accum.clear();
    sig.raise(88);
    a.checkEqual("after disconnect of receiver", g_accum, "*stat88");
}

/** Test signal connect/disconnect with unary signal. */
AFL_TEST("afl.base.Signal:unary:disconnect-second", a)
{
    // explicit signal connect/disconnect; disconnect second handler; with arg
    afl::base::Signal<void(int)> sig;
    {
        // we use staticFun here to test invocation without args
        sig.add(staticFun);
        ReceiverArg rec(0, sig);

        g_accum.clear();
        sig.raise(99);
        a.checkEqual("after static+receiver", g_accum, "*Rec0,99*stat");
    }
    g_accum.clear();
    sig.raise(88);
    a.checkEqual("after disconnect of receiver", g_accum, "*stat");
}

/** Test death of signal object with unary signal. */
AFL_TEST("afl.base.Signal:unary:signal-dies", a)
{
    // signal dies before receiver
    afl::base::Signal<void(int)>* sig = new afl::base::Signal<void(int)>();
    ReceiverArg rec(0, *sig);

    a.check("signal is connected", rec.conn.isConnected());

    g_accum.clear();
    sig->raise(77);
    a.checkEqual("after raise", g_accum, "*Rec0,77");

    delete sig;
    a.check("signal is disconnect", !rec.conn.isConnected());
}

/** Test multiple static signal handlers. */
AFL_TEST("afl.base.Signal:multi:static", a)
{
    // four identical handlers
    afl::base::Signal<void()> sig;
    sig.add(staticFun);
    sig.add(staticFun);
    sig.add(staticFun);
    sig.add(staticFun);

    g_accum.clear();
    sig.raise();
    a.checkEqual("", g_accum, "*stat*stat*stat*stat");
}

/** Test multiple bound signal handlers. */
AFL_TEST("afl.base.Signal:multi:bound", a)
{
    // four objects
    afl::base::Signal<void()> sig;
    Receiver r1(0, sig), r2(1, sig), r3(2, sig), r4(3, sig);

    g_accum.clear();
    sig.raise();
    a.checkEqual("", g_accum, "*Rec3*Rec2*Rec1*Rec0");
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
AFL_TEST("afl.base.Signal:binary", a)
{
    afl::base::Signal<void(One, Two)> sig;
    sig.add(twoArgs);
    g_accum.clear();
    sig.raise(one, two);
    a.checkEqual("", g_accum, "two");
}

AFL_TEST("afl.base.Signal:ternary", a)
{
    afl::base::Signal<void(One, Two, Three)> sig;
    sig.add(threeArgs);
    g_accum.clear();
    sig.raise(one, two, three);
    a.checkEqual("", g_accum, "three");
}

AFL_TEST("afl.base.Signal:quaternary", a)
{
    afl::base::Signal<void(One, Two, Three, Four)> sig;
    sig.add(fourArgs);
    g_accum.clear();
    sig.raise(one, two, three, four);
    a.checkEqual("", g_accum, "four");
}

AFL_TEST("afl.base.Signal:quinary", a)
{
    afl::base::Signal<void(One, Two, Three, Four, Five)> sig;
    sig.add(fiveArgs);
    g_accum.clear();
    sig.raise(one, two, three, four, five);
    a.checkEqual("", g_accum, "five");
}

AFL_TEST("afl.base.Signal:senary", a)
{
    afl::base::Signal<void(One, Two, Three, Four, Five, Six)> sig;
    sig.add(sixArgs);
    g_accum.clear();
    sig.raise(one, two, three, four, five, six);
    a.checkEqual("", g_accum, "six");
}


namespace {
    template<int n>
    int returnValue()
    {
        return n;
    }
}

/** Test result merging behaviour. */
AFL_TEST("afl.base.Signal:result:nullary", a)
{
    afl::base::Signal<int()> sig;
    a.checkEqual("01", sig.raise(), 0);

    afl::base::SignalConnection conn1(sig.add(returnValue<5>));
    a.checkEqual("02", sig.raise(), 5);

    afl::base::SignalConnection conn2(sig.add(returnValue<9>));
    a.checkEqual("03", sig.raise(), 5);

    afl::base::SignalConnection conn3(sig.add(returnValue<3>));
    a.checkEqual("04", sig.raise(), 5);

    // dropping first one changes result
    conn1.disconnect();
    a.checkEqual("05", sig.raise(), 9);

    // dropping last one does not change result
    conn3.disconnect();
    a.checkEqual("06", sig.raise(), 9);

    // dropping middle one, signal now empty
    conn2.disconnect();
    a.checkEqual("07", sig.raise(), 0);
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
AFL_TEST("afl.base.Signal:result:unary", a)
{
    afl::base::Signal<int(int)> sig;
    a.checkEqual("before", sig.raise(9), 0);
    sig.add(sum1);
    a.checkEqual("after", sig.raise(9), 9);
}

AFL_TEST("afl.base.Signal:result:binary", a)
{
    afl::base::Signal<int(int,int)> sig;
    a.checkEqual("before", sig.raise(9,5), 0);
    sig.add(sum2);
    a.checkEqual("after", sig.raise(9,5), 14);
}

AFL_TEST("afl.base.Signal:result:ternary", a)
{
    afl::base::Signal<int(int,int,int)> sig;
    a.checkEqual("before", sig.raise(2,9,5), 0);
    sig.add(sum3);
    a.checkEqual("after", sig.raise(2,9,5), 16);
}

AFL_TEST("afl.base.Signal:result:quaternary", a)
{
    afl::base::Signal<int(int,int,int,int)> sig;
    a.checkEqual("before", sig.raise(2,47,9,5), 0);
    sig.add(sum4);
    a.checkEqual("after", sig.raise(2,47,9,5), 63);
}

AFL_TEST("afl.base.Signal:result:quinary", a)
{
    afl::base::Signal<int(int,int,int,int,int)> sig;
    a.checkEqual("before", sig.raise(2,47,9,5,12), 0);
    sig.add(sum5);
    a.checkEqual("after", sig.raise(2,47,9,5,12), 75);
}

AFL_TEST("afl.base.Signal:result:senary", a)
{
    afl::base::Signal<int(int,int,int,int,int,int)> sig;
    a.checkEqual("before", sig.raise(2,47,9,5,4,10), 0);
    sig.add(sum6);
    a.checkEqual("after", sig.raise(2,47,9,5,4,10), 77);
}

/** Test Signal.addNew(). */
AFL_TEST("afl.base.Signal:addNew", a)
{
    std::string log;
    {
        afl::base::Signal<void()> sig;
        sig.addNew(new LifetimeTester(log), &LifetimeTester::handle);
        sig.raise();
        sig.raise();
    }
    a.checkEqual("", log, "+hh-");
}

namespace {
    class Disconnector {
     public:
        Disconnector(afl::base::Signal<void(int)>& sig, int n, int& counter)
            : m_value(n),
              m_counter(counter),
              conn_signal(sig.add(this, &Disconnector::onSignal))
            { }
        void onSignal(int n)
            {
                ++m_counter;
                if (n == m_value) {
                    conn_signal.disconnect();
                }
            }
     private:
        const int m_value;
        int& m_counter;
        afl::base::SignalConnection conn_signal;
    };
}

/** Test signals that disconnect themselves. */
AFL_TEST("afl.base.Signal:disconnect-self", as)
{
    for (int i = 1; i <= 3; ++i) {
        int counter = 0;
        afl::base::Signal<void(int)> testee;
        Disconnector a(testee, 1, counter);
        Disconnector b(testee, 2, counter);
        Disconnector c(testee, 3, counter);
        testee.raise(i);
        as.checkEqual("1", counter, 3);
        testee.raise(i);
        as.checkEqual("2", counter, 5);
        testee.raise(i);
        as.checkEqual("3", counter, 7);
    }
}
