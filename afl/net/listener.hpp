/**
  *  \file afl/net/listener.hpp
  *  \brief Interface afl::net::Listener
  */
#ifndef AFL_AFL_NET_LISTENER_HPP
#define AFL_AFL_NET_LISTENER_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/refcounted.hpp"
#include "afl/base/ptr.hpp"
#include "afl/sys/types.hpp"
#include "afl/async/cancelable.hpp"

namespace afl { namespace net {

    class Name;
    class Socket;
    class AcceptOperation;

    /** Socket listener.
        This wraps a socket opened with listen(),
        and allows accepting connections.
        This is a separate interface because listening sockets do not allow data transfers. */
    class Listener : public afl::async::Cancelable /* implies Deletable */, public afl::base::RefCounted {
     public:
        /** Destructor. */
        virtual ~Listener();

        /** Accept a connection.
            \param timeout Maximum time to wait for a connection.
            \return Newly-allocated Socket object for an incoming connection.
            Null if the timeout was reached.
            (Null is never returned when the timeout is INFINITE_TIMEOUT.) */
        virtual afl::base::Ptr<Socket> accept(afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Accept a connection, asynchronously.
            \param ctl Controller
            \param op Async operation.
            ctl.wait() will return this operation when a message has been sent by send(). */
        virtual void acceptAsync(afl::async::Controller& ctl, AcceptOperation& op) = 0;

        /** Cancel accept operation.
            This can be used to cancel accept operations posted on the given Controller.
            This function must, like all functions that access a Controller,
            be called from the thread that owns the Controller.
            \param ctl Controller
            \param op Operation */
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op) = 0;
    };

} }

inline
afl::net::Listener::~Listener()
{ }

#endif
