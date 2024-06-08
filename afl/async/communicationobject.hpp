/**
  *  \file afl/async/communicationobject.hpp
  *  \brief Interface afl::async::CommunicationObject
  */
#ifndef AFL_AFL_ASYNC_COMMUNICATIONOBJECT_HPP
#define AFL_AFL_ASYNC_COMMUNICATIONOBJECT_HPP

#include "afl/async/cancelable.hpp"
#include "afl/sys/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace async {

    class Controller;
    class SendOperation;
    class ReceiveOperation;

    /** Communication object.
        Objects of this type implement actual data transfer.
        Communication objects implement both send and receive operations.
        The relation between both is up to the implementation.
        - sent data could appear on the receive side, or on another object (socket-style)
        - transfers could be guaranteed to complete, or allow partial transmission

        Special care must be taken when using timeouts or cancel().
        If an operation is aborted mid-way, there is no way to obtain the status of the operation.
        A data transfer could have completed partially.
        If the CommunicationObject implements a complex protocol, both sides could get out of sync.
        Therefore, in the general case, use timeouts only to detect connection loss or hang,
        not as part of a regular protocol. */
    class CommunicationObject : public Cancelable {
     public:
        /** Send data, synchronous.
            This blocks the current thread until the operation has been performed or the timeout expires.
            Implementations are encouraged to implement special handling for timeout=0 and timeout=INFINITE_TIMEOUT.
            \param ctl Controller
            \param op Send operation specifying data to send
            \param timeout Timeout in milliseconds
            \retval true Data has been sent (check op.getNumSentBytes() for how many)
            \retval false Timeout reached */
        virtual bool send(Controller& ctl, SendOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Send data, asynchronous.
            This call completes without significant blocking.
            The actual operation will be performed in the background,
            possibly when Controller::wait() is called.
            \param ctl Controller
            \param op Send operation specifying data to send.
            ctl.wait() will return this operation when the transfer completed. */
        virtual void sendAsync(Controller& ctl, SendOperation& op) = 0;

        /** Receive data, synchronous.
            This blocks the current thread until the operation has been performed or the timeout expires.
            Implementations are encouraged to implement special handling for timeout=0 and timeout=INFINITE_TIMEOUT.
            \param ctl Controller
            \param op Receive operation specifying buffer to receive
            \param timeout Timeout in milliseconds
            \retval true Data has been received (check op.getNumReceivedBytes() for how many)
            \retval false Timeout reached */
        virtual bool receive(Controller& ctl, ReceiveOperation& op, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT) = 0;

        /** Receive data, asynchronous.
            This call completes without significant blocking.
            The actual operation will be performed in the background,
            possibly when Controller::wait() is called.
            \param ctl Controller
            \param op Receive operation specifying buffer to receive.
            ctl.wait() will return this operation when the transfer completed. */
        virtual void receiveAsync(Controller& ctl, ReceiveOperation& op) = 0;

        /** Cancel an asynchronous operation.
            This can be used to cancel send and receive operations posted on the given Controller.
            This function must, like all functions that access a Controller,
            be called from the thread that owns the Controller.
            \param ctl Controller
            \param op Operation */
        virtual void cancel(Controller& ctl, Operation& op) = 0;

        /** Get name.
            \return Name of this communication object. */
        virtual String_t getName() = 0;

        /** Full send, synchronous.
            If the send() function returns partial sends, this function retries until all data has been sent.
            This blocks the current thread until the operation has been performed or the timeout expires.
            \param ctl Controller
            \param bytes Bytes to send
            \param timeout Timeout in milliseconds
            \throw afl::except::FileProblemException send() returns a timeout or refuses to send.
                   The message will be networkError(). */
        void fullSend(Controller& ctl, afl::base::ConstBytes_t bytes, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);

        /** Full receive, synchronous.
            If the receive() function returns partial data, this function retries until the full buffer has been received.
            This blocks the current thread until the operation has been performed or the timeout expires.
            \param ctl Controller
            \param bytes Buffer to receive
            \param timeout Timeout in milliseconds
            \throw afl::except::FileProblemException send() returns a timeout or refuses to receive.
                   The message will be networkError() or networkConnectionLost(). */
        void fullReceive(Controller& ctl, afl::base::Bytes_t bytes, afl::sys::Timeout_t timeout = afl::sys::INFINITE_TIMEOUT);
    };

} }

#endif
