/**
  *  \file afl/sys/loglistener.hpp
  *  \brief Class afl::sys::LogListener
  */
#ifndef AFL_AFL_SYS_LOGLISTENER_HPP
#define AFL_AFL_SYS_LOGLISTENER_HPP

#include <vector>
#include <stdexcept>
#include "afl/base/deletable.hpp"
#include "afl/base/uncopyable.hpp"
#include "afl/string/string.hpp"
#include "afl/sys/time.hpp"

namespace afl { namespace sys {

    class Log;

    /** Log listener.
        A LogListener listens on a Log object.
        It will receive its log messages and can do something with them
        (like writing them out or displaying them).

        LogListener's automatically disconnect from all Log's they watch when they die, and vice versa.
        Interlocking (mutexes) is provided by Log, see there. */
    class LogListener : public afl::base::Deletable, private afl::base::Uncopyable {
     public:
        /** Log level. */
        enum Level {
            Trace,              ///< Trace messages. For developers only.
            Debug,              ///< Debug messages. For technical users, not end-users.
            Info,               ///< Information. General console messages.
            Warn,               ///< Warnings. Something went wrong, but we try to compensate or degrade.
            Error               ///< Errors. Something failed.
        };

        /** Message. */
        struct Message {
            Time     m_time;    ///< Time when message was generated.
            Level    m_level;   ///< Log level.
            String_t m_channel; ///< Log channel, i.e.\ originating component.
            String_t m_message; ///< Log message.
        };

        /** Constructor. */
        LogListener();

        /** Destructor.
            Deregisters from all Log objects I'm observing. */
        ~LogListener();

        /** Write log message (general interface).
            \param level log level
            \param channel log channel (originating component)
            \param message log message (text) */
        void write(Level level, const String_t& channel, const String_t& message);

        /** Write a file/line log message (shortcut).
            \param level log level
            \param channel log channel (originating component)
            \param fileName file name (text)
            \param lineNr line number
            \param message log message (text) */
        void write(Level level, const String_t& channel, const String_t& fileName, int lineNr, const String_t& message);

        /** Write an exception message (shortcut).
            \param level log level
            \param channel log channel (originating component)
            \param message log message (text)
            \param e exception object. Should have been caught by reference to be able to access its original dynamic type. */
        void write(Level level, const String_t& channel, const String_t& message, const std::exception& e);

        /** Write log message (listener interface).
            \param msg Log message */
        virtual void handleMessage(const Message& msg) = 0;

     private:
        friend class Log;
        std::vector<Log*> m_parents;
    };

} }

#endif
