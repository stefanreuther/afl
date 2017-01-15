/**
  *  \file arch/posix/posixnetworkstack.hpp
  *  \brief Class arch::posix::PosixNetworkStack
  */
#ifndef AFL_ARCH_POSIX_POSIXNETWORKSTACK_HPP
#define AFL_ARCH_POSIX_POSIXNETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/types.hpp"

namespace arch { namespace posix {

    /** Implementation of afl::net::NetworkStack for POSIX. */
    class PosixNetworkStack : public afl::net::NetworkStack {
     public:
        class Listener;
        class Socket;

        virtual afl::base::Ref<afl::net::Listener> listen(const afl::net::Name& name, int backlogSize);
        virtual afl::base::Ref<afl::net::Socket> connect(const afl::net::Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    };

} }

#endif
