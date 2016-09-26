/**
  *  \file afl/base/signalhandler.hpp
  *  \brief Base Class afl::base::SignalHandler
  *
  *  From cpluslib/sigslot.h 1.13.
  */
#ifndef AFL_AFL_BASE_SIGNALHANDLER_HPP
#define AFL_AFL_BASE_SIGNALHANDLER_HPP

#include "afl/base/uncopyable.hpp"

namespace afl { namespace base {

    /** Base class for a signal handler. Together with SignalConnection,
        this provides lifetime management for signal connections. Users
        never directly interact with this class, except by passing the
        SignalHandler* they got from SignalX.add() to
        SignalConnection(). */
    class SignalHandler : public afl::base::Uncopyable {
     public:
        /** Virtual destructor.
            Whenever a signal handler dies, it will automatically
            - log off its SignalConnection (pointed-to by #m_pBacklink)
            - log off its Signal's handler list (managed with #m_pNext) */
        virtual ~SignalHandler();

        /** Get next signal handler. */
        SignalHandler* next() const;

        /** Produce a backlink. */
        void backlink(SignalHandler** pBacklink);

     protected:
        /** Constructor. Sets up pointers for hooking this element after
            *prev, but does not modify *prev yet. */
        explicit SignalHandler(SignalHandler** pNext);

     private:
        /** Points to the pointer to this SignalHandler in a SignalConnection. May be null. */
        SignalHandler** m_pBacklink;

        /** Pointer to previous element's next link in signal chain. */
        SignalHandler** m_pNext;

        /** Next signal handler in this signal chain. */
        SignalHandler* m_next;
    };

} }

/******************************** Inlines ********************************/

inline afl::base::SignalHandler*
afl::base::SignalHandler::next() const
{
    return m_next;
}

inline
afl::base::SignalHandler::SignalHandler(SignalHandler** pNext)
    : Uncopyable(),
      m_pBacklink(0),
      m_pNext(pNext),
      m_next(*pNext)
{
    if (m_next) {
        m_next->m_pNext = &m_next;
    }
}

#endif
