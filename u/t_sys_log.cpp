/**
  *  \file u/t_sys_log.cpp
  *  \brief Test for afl::sys::Log
  */

#include <memory>
#include "afl/sys/log.hpp"

#include "u/t_sys.hpp"

namespace {
    struct Helper : public afl::sys::LogListener {
        Helper()
            : m_seen(false)
            { }
        void handleMessage(const afl::sys::Log::Message& msg)
            {
                TS_ASSERT(msg.m_time <= afl::sys::Time::getCurrentTime());
                TS_ASSERT_EQUALS(msg.m_channel, "channel");
                TS_ASSERT_EQUALS(msg.m_message, "message");
                TS_ASSERT_EQUALS(msg.m_level, afl::sys::Log::Warn);
                m_seen = true;
            }
        bool m_seen;
    };
}

/** Simple test. */
void
TestSysLog::testIt()
{
    {
        // We can write on a default-constructed logger
        afl::sys::Log logger;
        logger.write(afl::sys::Log::Error, "foo", "bar");
    }
    {
        // We can write on the default instance
        afl::sys::Log::getDefaultInstance().write(afl::sys::Log::Error, "foo", "bar");
    }
    {
        // Using a listener
        afl::sys::Log logger;
        Helper helper;
        logger.addListener(helper);
        TS_ASSERT_EQUALS(helper.m_seen, false);
        logger.write(afl::sys::Log::Warn, "channel", "message");
        TS_ASSERT_EQUALS(helper.m_seen, true);

        {
            Helper helper2;
            logger.addListener(helper2);
            logger.write(afl::sys::Log::Warn, "channel", "message");
            TS_ASSERT_EQUALS(helper2.m_seen, true);
        }

        logger.write(afl::sys::Log::Warn, "channel", "message");
    }
}

/** Test lifetime behaviour. */
void
TestSysLog::testLifetime()
{
    using afl::sys::Log;
    {
        std::auto_ptr<Log> a(new Log());
        std::auto_ptr<Log> b(new Log());
        a->addListener(*b);
        a->write(afl::sys::Log::Warn, "channel", "message");
        b->write(afl::sys::Log::Warn, "channel", "message");
        a.reset();
        b->write(afl::sys::Log::Warn, "channel", "message");
    }
    {
        std::auto_ptr<Log> a(new Log());
        std::auto_ptr<Log> b(new Log());
        a->write(afl::sys::Log::Warn, "channel", "message");
        b->write(afl::sys::Log::Warn, "channel", "message");
        b.reset();
        a->write(afl::sys::Log::Warn, "channel", "message");
    }
}

