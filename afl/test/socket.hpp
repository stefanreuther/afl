/**
  *  \file afl/test/socket.hpp
  *  \brief Class afl::test::Socket
  */
#ifndef AFL_AFL_TEST_SOCKET_HPP
#define AFL_AFL_TEST_SOCKET_HPP

#include "afl/base/ref.hpp"
#include "afl/net/name.hpp"
#include "afl/net/socket.hpp"
#include "afl/test/assert.hpp"

namespace afl { namespace test {

    /** Mock: Socket.
        Simulates a socket.
        Expects particular data to be sent, and produces data to be received, as well as identification information.
        No particular sequence of receive and send is enforced.

        It is an error
        - to send data that differs from the expected
        - to call closeSend() when we still expect data to be sent

        It is not an error to send or receive when the data has been exhausted;
        this is reported as 0 bytes sent/received.

        This is completely synchronous and not multi-thread safe. */
    class Socket : public afl::net::Socket {
     public:
        /** Create.
            \param a     Assertion generator
            \param name  Name to report in getPeerName(), getName()
            \param in    Data to produce on receive(). Must have lifetime that exceeds that of the Socket.
            \param out   Data to expect on send(). Must have lifetime that exceeds that of the Socket. */
        static afl::base::Ref<Socket> create(const Assert& a, const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out);

        // Socket:
        virtual void closeSend();
        virtual afl::net::Name getPeerName();
        virtual bool send(afl::async::Controller& ctl, afl::async::SendOperation& op, afl::sys::Timeout_t timeout);
        virtual void sendAsync(afl::async::Controller& ctl, afl::async::SendOperation& op);
        virtual bool receive(afl::async::Controller& ctl, afl::async::ReceiveOperation& op, afl::sys::Timeout_t timeout);
        virtual void receiveAsync(afl::async::Controller& ctl, afl::async::ReceiveOperation& op);
        virtual void cancel(afl::async::Controller& ctl, afl::async::Operation& op);
        virtual String_t getName();

     private:
        Socket(const Assert& a, const afl::net::Name& name, afl::base::ConstBytes_t in, afl::base::ConstBytes_t out);

        const Assert m_assert;
        const afl::net::Name m_name;
        afl::base::ConstBytes_t m_in;
        afl::base::ConstBytes_t m_out;
    };


} }

#endif
