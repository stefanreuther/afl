/**
  *  \file afl/net/reconnectable.hpp
  *  \brief Interface afl::net::Reconnectable
  */
#ifndef AFL_AFL_NET_RECONNECTABLE_HPP
#define AFL_AFL_NET_RECONNECTABLE_HPP

namespace afl { namespace net {

    /** Interface for reconnectable connections.
        Some network connections may get disconnected for various reasons.
        Depending on the protocol, a seamless reconnection may be possible or not.
        This interface provides a method for configuring that. */
    class Reconnectable {
     public:
        /** Reconnection mode. */
        enum Mode {
            /** Never reconnect.
                If the connection got disconnected, generate an error. */
            Never,

            /** Reconnect once.
                If the connection got disconnected before the next operation, reconnect.
                If the connection gets disconnected later, generate an error. */
            Once,

            /** Always reconnect.
                Reconnect on every connection loss. */
            Always
        };

        /** Protected destructor.
            Reconnectable cannot serve as a base class for pointers. */
        virtual ~Reconnectable()
            { }

        /** Set reconnect mode.
            \param mode New mode. The default is implementation-defined. */
        virtual void setReconnectMode(Mode mode) = 0;
    };

} }

#endif
