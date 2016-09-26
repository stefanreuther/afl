/**
  *  \file afl/base/signalconnection.hpp
  *  \brief Class afl::base::SignalConnection
  */
#ifndef AFL_AFL_BASE_SIGNALCONNECTION_HPP
#define AFL_AFL_BASE_SIGNALCONNECTION_HPP

#include "afl/base/uncopyable.hpp"
#include "afl/base/signalhandler.hpp"

namespace afl { namespace base {

    /** Signal connection.
        This provides lifetime management for signal connections.

        A signal receiver initialize this object with the result of some
        sig.add(...). This will ensure that the signal is disconnected
        when the receiver dies. It's also possible that the sender dies,
        in this case the SignalConnection will automatically revert to
        unconnected state. */
    class SignalConnection : public Uncopyable {
     public:
        /** Constructor. Creates a connected SignalConnection.
            \param handler result of some_signal.add(...), or null */
        SignalConnection(SignalHandler* handler);

        /** Constructor. Creates a disconnected SignalConnection. */
        SignalConnection();

        /** Destructor. */
        ~SignalConnection();

        /** Disconnect this signal. */
        void disconnect();

        /** Re-assign the signal. This will disconnect the previous handler. */
        void operator=(SignalHandler* handler);

        /** Check whether this connection is occupied. */
        bool isConnected() const;

     private:
        SignalHandler* m_handler;
    };

} }

inline
afl::base::SignalConnection::SignalConnection()
    : m_handler(0)
{ }

inline
afl::base::SignalConnection::~SignalConnection()
{
    delete m_handler;
}

inline void
afl::base::SignalConnection::disconnect()
{
    // Deleting the handler will set it to null.
    delete m_handler;
}

inline bool
afl::base::SignalConnection::isConnected() const
{
    return m_handler != 0;
}

#endif
