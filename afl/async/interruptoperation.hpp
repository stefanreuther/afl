/**
  *  \file afl/async/interruptoperation.hpp
  *  \brief Class afl::async::InterruptOperation
  */
#ifndef AFL_AFL_ASYNC_INTERRUPTOPERATION_HPP
#define AFL_AFL_ASYNC_INTERRUPTOPERATION_HPP

#include "afl/async/operation.hpp"
#include "afl/bits/smallset.hpp"

namespace afl { namespace async {

    /** Operation for use with afl::async::Interrupt.
        Represents the operation to wait for one or more external interrupts (signals, control events). */
    class InterruptOperation : public Operation {
     public:
        /** Interrupt kind. */
        enum Kind {
            /** User-requested program break.
                - POSIX: SIGINT (i.e. user pressed Ctrl+C)
                - Win32: CTRL_C_EVENT, CTRL_BREAK_EVENT (i.e. user pressed Ctrl+C, Ctrl+Break) */
            Break,

            /** Program exit requested by loss of execution environment.
                - POSIX: SIGHUP (i.e. user terminated session/closed terminal window)
                - Win32: CTRL_LOGOFF_EVENT, CTRL_SHUTDOWN_EVENT (i.e. user terminated session) */
            Hangup,

            /** Program exit requested by task management.
                - POSIX: SIGTERM (i.e. user used "kill")
                - Win32: CTRL_CLOSE_EVENT (i.e. user used Task Manager to close the program, or closed terminal window) */
            Terminate
        };

        /** Set of interrupt kinds. */
        typedef afl::bits::SmallSet<Kind> Kinds_t;


        /** Default constructor.
            A default-constructed InterruptOperation does not wait for any event. */
        InterruptOperation();

        /** Constructor.
            \param k Interrupt to wait for */
        InterruptOperation(Kind k);

        /** Constructor.
            \param k Set of interrupts to wait for */
        InterruptOperation(Kinds_t k);

        /** Get requested kind of interrupts.
            \return set */
        Kinds_t getRequested() const;

        /** Set requested kind of interrupts.
            Note that you must not modify this set while you're waiting using this set.
            \param k set */
        void setRequested(Kinds_t k);

        /** Get received kind of interrupts.
            After a successful wait, this will be a subset of getRequested(), containing the received interrupt(s).
            \return set */
        Kinds_t getReceived() const;

        /** Set received kind of interrupts.
            This is used by the Interrupt implementation.
            \param k set */
        void setReceived(Kinds_t k);

     private:
        Kinds_t m_requested;
        Kinds_t m_received;
    };

} }

// Default constructor.
inline
afl::async::InterruptOperation::InterruptOperation()
    : m_requested(), m_received()
{ }

// Constructor.
inline
afl::async::InterruptOperation::InterruptOperation(Kind k)
    : m_requested(k), m_received()
{ }

// Constructor.
inline
afl::async::InterruptOperation::InterruptOperation(Kinds_t k)
    : m_requested(k), m_received()
{ }

// Get requested kind of interrupts.
inline afl::async::InterruptOperation::Kinds_t
afl::async::InterruptOperation::getRequested() const
{
    return m_requested;
}

// Set requested kind of interrupts.
inline void
afl::async::InterruptOperation::setRequested(Kinds_t k)
{
    m_requested = k;
}

// Get received kind of interrupts.
inline afl::async::InterruptOperation::Kinds_t
afl::async::InterruptOperation::getReceived() const
{
    return m_received;
}

// Set received kind of interrupts.
inline void
afl::async::InterruptOperation::setReceived(Kinds_t k)
{
    m_received = k;
}

#endif
