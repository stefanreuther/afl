/**
  *  \file afl/net/tunnel/socketwrapper.hpp
  */
#ifndef AFL_AFL_NET_TUNNEL_SOCKETWRAPPER_HPP
#define AFL_AFL_NET_TUNNEL_SOCKETWRAPPER_HPP

#include "afl/net/socket.hpp"
#include "afl/base/ref.hpp"
#include "afl/net/name.hpp"

namespace afl { namespace net { namespace tunnel {

    class SocketWrapper : public Socket {
     public:
        SocketWrapper(afl::base::Ref<Socket> base, const Name& socketName, const Name& peerName);
        ~SocketWrapper();

        // Socket
        virtual void closeSend();
        virtual Name getPeerName();

        // Socket
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);
        virtual bool send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
        virtual void sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op);
        virtual bool receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
        virtual void receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op);
        virtual String_t getName();

     private:
        afl::base::Ref<Socket> m_base;
        Name m_socketName;
        Name m_peerName;
    };


} } }

#endif
