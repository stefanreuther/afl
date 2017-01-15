/**
  *  \file afl/net/resp/client.cpp
  *  \brief Class afl::net::resp::Client
  */

#include <cassert>              // FIXME
#include "afl/net/resp/client.hpp"
#include "afl/async/communicationsink.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/data/access.hpp"
#include "afl/except/filetooshortexception.hpp"
#include "afl/except/remoteerrorexception.hpp"
#include "afl/io/bufferedsink.hpp"
#include "afl/io/resp/writer.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/sys/mutexguard.hpp"

// Constructor.
afl::net::resp::Client::Client(NetworkStack& stack, const Name& name)
    : CommandHandler(),
      m_stack(stack),
      m_name(name),
      m_mutex(),
      m_factory(),
      m_controller(),

      m_socket(),

      m_input(),
      m_parser(m_factory)
{
    connect();
}

// Destructor.
afl::net::resp::Client::~Client()
{ }

// Call, with result.
afl::net::resp::Client::Value_t*
afl::net::resp::Client::call(const Segment_t& command)
{
    // Serialize
    afl::sys::MutexGuard g(m_mutex);

    // Send the command
    afl::async::CommunicationSink comSink(m_controller, m_socket);
    afl::io::BufferedSink sink(comSink);
    afl::io::resp::Writer(sink).visitSegment(command);
    sink.flush();

    // Read response
    while (1) {
        if (m_input.empty()) {
            // Read some input data
            afl::async::ReceiveOperation op(m_inputBuffer);
            if (!m_socket->receive(m_controller, op)) {
                // FIXME?
                assert(0);
            }
            if (op.getNumReceivedBytes() == 0) {
                // FIXME
                throw afl::except::FileTooShortException(m_socket->getName());
            }
            m_input = op.getReceivedBytes();
        }
        if (m_parser.handleData(m_socket->getName(), m_input)) {
            // Fetch result, fend off errors
            std::auto_ptr<Value_t> result(m_parser.extract());
            String_t src, err;
            if (afl::data::Access(result.get()).isError(src, err)) {
                throw afl::except::RemoteErrorException(m_socket->getName(), err);
            }
            return result.release();
        }
    }
}

// Call, without result.
void
afl::net::resp::Client::callVoid(const Segment_t& command)
{
    // Minimum implementation follows.
    // FIXME: optimize by implementing pipelining.
    delete call(command);
}

// Build connection.
void
afl::net::resp::Client::connect()
{
    m_socket = m_stack.connect(m_name).asPtr();
}
