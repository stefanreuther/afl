/**
  *  \file test/afl/async/controllertest.cpp
  *  \brief Test for afl::async::Controller
  */

#include "afl/async/controller.hpp"

#include "afl/async/operation.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/sys/thread.hpp"
#include "afl/test/testrunner.hpp"

/** Simple test. */
AFL_TEST("afl.async.Controller", a)
{
    // Make it empty
    afl::async::Controller ctl;
    a.check("01. wait", ctl.wait(0) == 0);

    // Post one, check that we get it once
    afl::async::Operation op;
    ctl.post(op);
    a.check("11. wait", ctl.wait(0) == &op);
    a.check("12. wait", ctl.wait(0) == 0);

    // Post, cancel. Check that we don't get it
    ctl.post(op);
    ctl.revertPost(op);
    a.check("21. wait", ctl.wait(0) == 0);

    // Now from another thread
    class Tester : public afl::base::Stoppable {
     public:
        Tester(afl::async::Controller& ctl, afl::async::Operation& op)
            : m_ctl(ctl), m_op(op)
            { }
        void run()
            {
                afl::sys::Thread::sleep(100);
                m_ctl.post(m_op);
            }
        void stop()
            { }
     private:
        afl::async::Controller& m_ctl;
        afl::async::Operation& m_op;
    };
    Tester tester(ctl, op);
    afl::sys::Thread thread("TestAsyncController", tester);
    thread.start();

    a.check("41. wait", ctl.wait(300) == &op);
    thread.join();
}
