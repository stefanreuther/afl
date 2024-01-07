/**
  *  \file test/afl/sys/logtest.cpp
  *  \brief Test for afl::sys::Log
  */

#include "afl/sys/log.hpp"
#include "afl/test/testrunner.hpp"

using afl::sys::Log;

namespace {
    struct Helper : public afl::sys::LogListener {
        Helper(afl::test::Assert a)
            : m_assert(a), m_seen(false)
            { }
        void handleMessage(const afl::sys::Log::Message& msg)
            {
                m_assert.check("time", msg.m_time <= afl::sys::Time::getCurrentTime());
                m_assert.checkEqual("channel", msg.m_channel, "channel");
                m_assert.checkEqual("message", msg.m_message, "message");
                m_assert.check("level", msg.m_level == afl::sys::Log::Warn);
                m_seen = true;
            }
        afl::test::Assert m_assert;
        bool m_seen;
    };
}

// We can write on a default-constructed logger
AFL_TEST_NOARG("afl.sys.Log:default-constructed")
{
    afl::sys::Log logger;
    logger.write(afl::sys::Log::Error, "foo", "bar");
}

// We can write on the default instance
AFL_TEST_NOARG("afl.sys.Log:default-instance")
{
    afl::sys::Log::getDefaultInstance().write(afl::sys::Log::Error, "foo", "bar");
}

// Using a listener
AFL_TEST("afl.sys.Log:addListener", a)
{
    Log logger;
    Helper helper(a("first listener"));;
    logger.addListener(helper);
    a.checkEqual("01. not seen", helper.m_seen, false);
    logger.write(afl::sys::Log::Warn, "channel", "message");
    a.checkEqual("02. seen", helper.m_seen, true);

    {
        Helper helper2(a("second listener"));;
        logger.addListener(helper2);
        logger.write(afl::sys::Log::Warn, "channel", "message");
        a.checkEqual("11. seen", helper2.m_seen, true);
    }

    logger.write(afl::sys::Log::Warn, "channel", "message");
}

/*
 *  Test lifetime behaviour
 *  (These tests succeed by not crashing.)
 */
AFL_TEST_NOARG("afl.sys.Log:lifetime:1")
{
    std::auto_ptr<Log> a(new Log());
    std::auto_ptr<Log> b(new Log());
    a->addListener(*b);
    a->write(afl::sys::Log::Warn, "channel", "message");
    b->write(afl::sys::Log::Warn, "channel", "message");
    a.reset();
    b->write(afl::sys::Log::Warn, "channel", "message");
}

AFL_TEST_NOARG("afl.sys.Log:lifetime:2")
{
    std::auto_ptr<Log> a(new Log());
    std::auto_ptr<Log> b(new Log());
    a->write(afl::sys::Log::Warn, "channel", "message");
    b->write(afl::sys::Log::Warn, "channel", "message");
    b.reset();
    a->write(afl::sys::Log::Warn, "channel", "message");
}

AFL_TEST_NOARG("afl.sys.Log:lifetime:3")
{
    std::auto_ptr<Log> a(new Log());
    std::auto_ptr<Log> b(new Log());
    a->addListener(*b);
    a->write(afl::sys::Log::Warn, "channel", "message");
    b->write(afl::sys::Log::Warn, "channel", "message");
    a->removeListener(*b);
    a->write(afl::sys::Log::Warn, "channel", "message");
}
