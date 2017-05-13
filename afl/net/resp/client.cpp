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
      m_mode(Always),

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
    try {
        sendCommand(command);
    }
    catch (std::exception&) {
        // Send threw. Try to reconnect and send again (if configured).
        // If it fails again, fail the call.
        if (m_mode == Never) {
            throw;
        }
        connect();
        sendCommand(command);
    }

    // If we are configured to reconnect once, this was our request. Disable reconnect.
    if (m_mode == Once) {
        m_mode = Never;
    }

    // Read response
    try {
        return readResponse();
    }
    catch (afl::except::RemoteErrorException&) {
        throw;
    }
    catch (std::exception&) {
        if (m_mode == Never) {
            throw;
        }
        connect();
        sendCommand(command);
        return readResponse();
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

// Set reconnect mode.
void
afl::net::resp::Client::setReconnectMode(Mode mode)
{
    m_mode = mode;
}

// Send a command.
void
afl::net::resp::Client::sendCommand(const Segment_t& command)
{
    afl::async::CommunicationSink comSink(m_controller, m_socket);
    afl::io::BufferedSink sink(comSink);
    afl::io::resp::Writer(sink).visitSegment(command);
    sink.flush();
}

// Read a response.
afl::data::Value*
afl::net::resp::Client::readResponse()
{
    while (1) {
        if (m_input.empty()) {
            // Read some input data
            afl::async::ReceiveOperation op(m_inputBuffer);
            if (!m_socket->receive(m_controller, op)) {
                // FIXME: how to deal with timeouts?
                assert(0);
            }
            if (op.getNumReceivedBytes() == 0) {
                // End of file, i.e. other side closed.
                throw afl::except::FileTooShortException(m_socket->getName());
            }
            m_input = op.getReceivedBytes();
        }
        if (m_parser.handleData(m_input)) {
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

// Build connection.
void
afl::net::resp::Client::connect()
{
    // Use a timeout of 10000. It hangs forever on Windows otherwise.
    // FIXME: make this configurable (or figure out why Windows hangs here).
    m_socket = m_stack.connect(m_name, 10000).asPtr();
}
