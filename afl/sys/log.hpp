/**
  *  \file afl/sys/log.hpp
  *  \brief Class afl::sys::Log
  */
#ifndef AFL_AFL_SYS_LOG_HPP
#define AFL_AFL_SYS_LOG_HPP

#include "afl/sys/loglistener.hpp"

namespace afl { namespace sys {

    /** Logging.
        Modules that produce log messages will write those on a Log instance (the default instance, maybe).
        Components that want to receive log messages derive from LogListener, which is registered to the Log instance.
        For convenience, Log is a LogListener as well so they can be chained.

        Log is fully interlocked; all methods can be called from any thread.
        This is absolutely required to get sensible on-line addListener/removeListener functionality.
        However, this means that
        - all log messages will pull a global mutex
        - all log listeners must take care to not block

        LogListener's automatically disconnect from all Log's they watch when they die, and vice versa. */
    class Log : public afl::sys::LogListener {
     public:
        /** Constructor. */
        Log();

        /** Destructor.
            Deregisters all LogListener's. */
        ~Log();

        /** Write log message.
            This function passes a prepared Message structure along.
            This can be useful when chaining Log instances together.
            \param msg Message */
        void handleMessage(const Message& msg);

        /** Get default instance.
            \return a default instance of the Log class.
            This is a useful default log target for all components until a different one is configured. */
        static Log& getDefaultInstance();

        /** Add a log listener.
            A listener can listen multiple times, and it can listen to multiple Log instances.
            \param listener Listener to add */
        void addListener(LogListener& listener);

        /** Remove a log listener.
            If the listener listens multiple times, it is removed just once.
            \param listener Listener to remove */
        void removeListener(LogListener& listener);

     private:
        friend class LogListener;
        std::vector<LogListener*> m_listeners;

        static void killListener(LogListener& listener);
        static void killParent(Log& parent);
    };

} }

#endif
