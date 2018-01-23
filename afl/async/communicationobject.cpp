/**
  *  \file afl/async/communicationobject.cpp
  */

#include "afl/async/communicationobject.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/string/messages.hpp"
#include "afl/sys/time.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/async/receiveoperation.hpp"

namespace {
    void adjustTimeout(afl::sys::Timeout_t& timeout, uint32_t t)
    {
        if (timeout != afl::sys::INFINITE_TIMEOUT) {
            uint32_t now = afl::sys::Time::getTickCounter();
            uint32_t elapsed = now - t;
            timeout -= std::min(timeout, elapsed);
            t = now;
        }
    }
}


void
afl::async::CommunicationObject::fullSend(Controller& ctl, afl::base::ConstBytes_t bytes, afl::sys::Timeout_t timeout)
{
    afl::async::SendOperation op(bytes);
    uint32_t t = afl::sys::Time::getTickCounter();
    while (!op.getUnsentBytes().empty()) {
        if (!send(ctl, op, timeout) || op.getNumSentBytes() == 0) {
            throw afl::except::FileProblemException(getName(), afl::string::Messages::networkError());
        }
        op.setData(op.getUnsentBytes());
        adjustTimeout(timeout, t);
    }
}

void
afl::async::CommunicationObject::fullReceive(Controller& ctl, afl::base::Bytes_t bytes, afl::sys::Timeout_t timeout)
{
    afl::async::ReceiveOperation op(bytes);
    uint32_t t = afl::sys::Time::getTickCounter();
    while (!op.isCompleted()) {
        op.setData(op.getUnreceivedBytes());
        if (!receive(ctl, op, timeout)) {
            throw afl::except::FileProblemException(getName(), afl::string::Messages::networkError());
        }
        if (op.getNumReceivedBytes() == 0) {
            throw afl::except::FileProblemException(getName(), afl::string::Messages::networkConnectionLost());
        }
        adjustTimeout(timeout, t);
    }
}
