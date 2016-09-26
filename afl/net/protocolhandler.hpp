/**
  *  \file afl/net/protocolhandler.hpp
  *  \brief Interface afl::net::ProtocolHandler
  */
#ifndef AFL_AFL_NET_PROTOCOLHANDLER_HPP
#define AFL_AFL_NET_PROTOCOLHANDLER_HPP

#include "afl/base/memory.hpp"
#include "afl/base/deletable.hpp"
#include "afl/sys/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net {

    /** Protocol handler.
        Implements a query-response protocol.
        A server implementation connects it to the network.

        The sequence of calls is:
        - call getOperation().
          - if there is data to send, send it. timeToWait specifies the maximum time.
            Call advanceTime(), then handleSendTimeout() if not all data could be sent.
          - otherwise, if close is set, close the connection, call handleConnectionClose(), and finish the session.
          - otherwise, wait timeToWait for data to arrive.
            Call advanceTime(). If data arrived, call handleData().
        An important limitation is that no data is being received while we are sending. */
    class ProtocolHandler : public afl::base::Deletable {
     public:
        /** Protocol handler operation. */
        struct Operation {
            /** Data to send. Next call to getOperation occurs when this data has been sent. */
            afl::base::ConstBytes_t m_dataToSend;

            /** Close connection. If set, the connection is closed and the ProtocolHandler destroyed. */
            bool m_close;

            /** Time to wait for data from the other side to arrive. */
            afl::sys::Timeout_t m_timeToWait;
        };

        /** Get operation to perform.
            \param op [out] Operation to perform (preinitialized to do-nothing, wait-indefinitely) */
        virtual void getOperation(Operation& op) = 0;

        /** Advance time.
            \param msecs [in] Milliseconds since last getOperation() call. */
        virtual void advanceTime(afl::sys::Timeout_t msecs) = 0;

        /** Handle incoming data.
            \param name [in] Name of data source
            \param bytes [in] Data to process */
        virtual void handleData(const String_t& name, afl::base::ConstBytes_t bytes) = 0;

        /** Handle send timeout. */
        virtual void handleSendTimeout(afl::base::ConstBytes_t unsentBytes) = 0;

        /** Handle connection close. */
        virtual void handleConnectionClose() = 0;
    };

} }

#endif
