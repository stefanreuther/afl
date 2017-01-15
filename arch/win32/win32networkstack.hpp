/**
  *  \file arch/win32/win32networkstack.hpp
  *  \brief Class arch::win32::Win32NetworkStack
  */
#ifndef AFL_ARCH_WIN32_WIN32NETWORKSTACK_HPP
#define AFL_ARCH_WIN32_WIN32NETWORKSTACK_HPP

#include "afl/net/networkstack.hpp"
#include "afl/net/listener.hpp"
#include "afl/net/socket.hpp"
#include "afl/net/name.hpp"
#include "afl/sys/types.hpp"

namespace arch { namespace win32 {

    /** Implementation of afl::net::NetworkStack for Win32. */
    class Win32NetworkStack : public afl::net::NetworkStack {
     public:
        class Listener;
        class Socket;

        Win32NetworkStack();

        virtual afl::base::Ref<afl::net::Listener> listen(const afl::net::Name& name, int backlogSize);
        virtual afl::base::Ref<afl::net::Socket> connect(const afl::net::Name& name, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    };

} }

#endif
