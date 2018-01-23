/**
  *  \file u/t_async_controller.cpp
  *  \brief Test for afl::async::Controller
  */

#include "afl/async/controller.hpp"

#include "u/t_async.hpp"
#include "afl/async/operation.hpp"
#include "afl/sys/thread.hpp"
#include "afl/base/stoppable.hpp"

/** Simple test. */
void
TestAsyncController::testIt()
{
    // Make it empty
    afl::async::Controller ctl;
    TS_ASSERT(ctl.wait(0) == 0);

    // Post one, check that we get it once
    afl::async::Operation op;
    ctl.post(op);
    TS_ASSERT(ctl.wait(0) == &op);
    TS_ASSERT(ctl.wait(0) == 0);

    // Post, cancel. Check that we don't get it
    ctl.post(op);
    ctl.revertPost(op);
    TS_ASSERT(ctl.wait(0) == 0);

    // Same things for postDirect
    ctl.postDirect(op);
    TS_ASSERT(ctl.wait(0) == &op);
    TS_ASSERT(ctl.wait(0) == 0);
    ctl.postDirect(op);
    ctl.revertPost(op);
    TS_ASSERT(ctl.wait(0) == 0);

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

    TS_ASSERT(ctl.wait(300) == &op);
    thread.join();
}
