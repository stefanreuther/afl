/**
  *  \file afl/async/communicationsink.cpp
  *  \brief Class afl::async::CommunicationSink
  */

#include <stdexcept>
#include "afl/async/communicationsink.hpp"
#include "afl/async/controller.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/string/messages.hpp"
#include "afl/except/fileproblemexception.hpp"

afl::async::CommunicationSink::CommunicationSink(Controller& ctl, afl::base::Ptr<CommunicationObject> obj)
    : afl::io::DataSink(),
      m_controller(ctl),
      m_pObj(obj),
      m_timeout(afl::sys::INFINITE_TIMEOUT)
{ }

afl::async::CommunicationSink::~CommunicationSink()
{ }

void
afl::async::CommunicationSink::setTimeout(afl::sys::Timeout_t t)
{
    m_timeout = t;
}

// DataSink methods:
bool
afl::async::CommunicationSink::handleData(const String_t& /*name*/, afl::base::ConstBytes_t& data)
{
    while (!data.empty()) {
        SendOperation op(data);
        if (!m_pObj->send(m_controller, op, m_timeout)) {
            // Timeout
            throw afl::except::FileProblemException(m_pObj->getName(), afl::string::Messages::cannotWrite());
        }
        if (op.getNumSentBytes() == 0) {
            // We didn't make progress
            throw afl::except::FileProblemException(m_pObj->getName(), afl::string::Messages::cannotWrite());
        }
        data = op.getUnsentBytes();
    }
    return false;
}
