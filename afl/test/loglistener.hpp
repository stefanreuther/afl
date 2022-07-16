/**
  *  \file afl/test/loglistener.hpp
  *  \brief Class afl::test::LogListener
  */
#ifndef AFL_AFL_TEST_LOGLISTENER_HPP
#define AFL_AFL_TEST_LOGLISTENER_HPP

#include "afl/sys/loglistener.hpp"

namespace afl { namespace test {

    /** Log listener for testing.
        Counts the messages, but otherwise ignores them.
        This can be used to test requirements of the form "action X must generate a warning message". */
    class LogListener : public afl::sys::LogListener {
     public:
        /** Constructor.
            Sets all counters to 0. */
        LogListener();

        /** Reset message counters. */
        void clear();

        /** Get total number of messages.
            \return number */
        size_t getNumMessages() const;

        /** Get total number of messages on level Error.
            \return number */
        size_t getNumErrors() const;

        /** Get total number of messages on level Warn.
            \return number */
        size_t getNumWarnings() const;

        // LogListener
        virtual void handleMessage(const Message& msg);

     private:
        size_t m_numMessages;
        size_t m_numErrors;
        size_t m_numWarnings;
    };

} }

#endif
