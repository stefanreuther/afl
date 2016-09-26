/**
  *  \file afl/async/communicationstream.cpp
  *  \brief Class afl::async::CommunicationStream
  */

#include <stdexcept>
#include "afl/async/communicationstream.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/string/messages.hpp"
#include "afl/io/filemapping.hpp"

afl::async::CommunicationStream::CommunicationStream(Controller& ctl, afl::base::Ptr<CommunicationObject> obj)
    : afl::io::Stream(),
      m_controller(ctl),
      m_pObj(obj),
      m_timeout(afl::sys::INFINITE_TIMEOUT)
{ }

afl::async::CommunicationStream::~CommunicationStream()
{ }

void
afl::async::CommunicationStream::setTimeout(afl::sys::Timeout_t t)
{
    m_timeout = t;
}

size_t
afl::async::CommunicationStream::read(Bytes_t m)
{
    ReceiveOperation op(m);
    if (!m_pObj->receive(m_controller, op, m_timeout)) {
        // FIXME: throw?
    }
    return op.getNumReceivedBytes();
}

size_t
afl::async::CommunicationStream::write(ConstBytes_t m)
{
    SendOperation op(m);
    if (!m_pObj->send(m_controller, op, m_timeout)) {
        // FIXME: throw?
    }
    return op.getNumSentBytes();
}

void
afl::async::CommunicationStream::flush()
{ }

void
afl::async::CommunicationStream::setPos(FileSize_t /*pos*/)
{ }

afl::async::CommunicationStream::FileSize_t
afl::async::CommunicationStream::getPos()
{
    return 0;
}

afl::async::CommunicationStream::FileSize_t
afl::async::CommunicationStream::getSize()
{
    return 0;
}

uint32_t
afl::async::CommunicationStream::getCapabilities()
{
    return CanRead | CanWrite;
}

String_t
afl::async::CommunicationStream::getName()
{
    return m_pObj->getName();
}

afl::base::Ptr<afl::io::Stream>
afl::async::CommunicationStream::createChild()
{
    throw std::runtime_error(afl::string::Messages::invalidOperation());
}

afl::base::Ptr<afl::io::FileMapping>
afl::async::CommunicationStream::createFileMapping(FileSize_t /*limit*/)
{
    return 0;
}
