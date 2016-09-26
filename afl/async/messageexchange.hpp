/**
  *  \file afl/async/messageexchange.hpp
  *  \brief Class afl::async::MessageExchange
  */
#ifndef AFL_AFL_ASYNC_MESSAGEEXCHANGE_HPP
#define AFL_AFL_ASYNC_MESSAGEEXCHANGE_HPP

#include "afl/async/communicationobject.hpp"
#include "afl/async/operationlist.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/sys/mutex.hpp"
#include "afl/sys/types.hpp"

namespace afl { namespace async {

    class Controller;

    /** Asynchronous message exchange.
        Provides a rendezvous point for multiple participants to exchange messages.
        Every send() operation is paired with a receive() operation.

        Messages are byte blocks.
        Message boundaries will be preserved.
        If sender and receiver buffer size do not match, the message will be truncated appropriately;
        both sides will know how many bytes were transferred.

        Data transfer is atomic.
        cancel() or timeouts never cause a message to be truncated.
        (However, it is still possible that one side still runs into a timeout or cancel
        wheres the other one already got confirmation of the transmission.) */
    class MessageExchange : public CommunicationObject {
     public:
        /** Constructor. */
        MessageExchange();

        /** Destructor. */
        ~MessageExchange();

        /** Send message.
            \param ctl Controller
            \param op Send operation specifying message to send
            \param timeout Timeout in milliseconds
            \retval true Message has been sent (check op.getNumSentBytes() for how many bytes other side accepted)
            \retval false Timeout reached */
        bool send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Send message, asynchronous.
            \param ctl Controller
            \param op Send operation specifying message to send.
            ctl.wait() will return this operation when the message has been received by receive(). */
        void sendAsync(Controller& ctl, SendOperation& op);

        /** Receive message.
            \param ctl Controller
            \param op Receive operation specifying buffer to receive
            \param timeout Timeout in milliseconds
            \retval true Message has been received (check op.getNumReceivedBytes() for how many bytes other side sent)
            \retval false Timeout reached */
        bool receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Receive message, asynchronous.
            \param ctl Controller
            \param op Receive operation specifying buffer to receive.
            ctl.wait() will return this operation when a message has been sent by send(). */
        void receiveAsync(Controller& ctl, ReceiveOperation& op);

        void cancel(Controller& ctl, Operation& op);

        String_t getName();

     private:
        afl::sys::Mutex m_mutex;
        OperationList<SendOperation> m_pendingSends;
        OperationList<ReceiveOperation> m_pendingReceives;
    };

} }

#endif
