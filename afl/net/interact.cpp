/**
  *  \file afl/net/interact.cpp
  *  \brief Function afl::net::interact()
  */

#include "afl/net/interact.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/net/protocolhandler.hpp"
#include "afl/sys/time.hpp"
#include "afl/sys/types.hpp"

namespace {
    /** Advance time for the protocol handler.
        \param t Last time we've talked to the ProtocolHandler
        \param handler ProtocolHandler */
    void advanceTime(uint32_t& t, afl::net::ProtocolHandler& handler)
    {
        uint32_t t2 = afl::sys::Time::getTickCounter();
        handler.advanceTime(t2 - t);
        t = t2;
    }
}

// Operate a ProtocolHandler on a given CommunicationObject.
void
afl::net::interact(afl::async::CommunicationObject& obj, ProtocolHandler& handler)
{
    // Buffer for receiving.
    uint8_t buffer[4096];

    // Perform protocol operations
    uint32_t t = afl::sys::Time::getTickCounter();
    afl::async::Controller ctl;
    while (1) {
        // Get an operation
        ProtocolHandler::Operation op;
        op.m_dataToSend.reset();
        op.m_close = false;
        op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
        handler.getOperation(op);

        // Do it
        if (!op.m_dataToSend.empty()) {
            // Send requested
            while (1) {
                afl::async::SendOperation tx(op.m_dataToSend);
                if (!obj.send(ctl, tx, op.m_timeToWait)) {
                    // Send failed
                    advanceTime(t, handler);
                    handler.handleSendTimeout(tx.getUnsentBytes());
                    break;
                } else {
                    // Send succeeded
                    op.m_dataToSend = tx.getUnsentBytes();
                    if (op.m_dataToSend.empty()) {
                        advanceTime(t, handler);
                        break;
                    }
                }
            }
        } else if (op.m_close) {
            // Close requested
            handler.handleConnectionClose();
            break;
        } else {
            // Receive requested
            afl::async::ReceiveOperation rx(buffer);
            bool got = obj.receive(ctl, rx, op.m_timeToWait);

            // Update time
            advanceTime(t, handler);

            // Submit data
            if (got) {
                if (rx.getNumReceivedBytes() == 0) {
                    // Other end closed
                    handler.handleConnectionClose();
                    break;
                } else {
                    handler.handleData(rx.getReceivedBytes());
                }
            }
        }
    }
}
