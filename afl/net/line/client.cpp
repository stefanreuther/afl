/**
  *  \file afl/net/line/client.cpp
  *  \brief Class afl::net::line::Client
  */

#include <memory>
#include "afl/net/line/client.hpp"
#include "afl/async/communicationsink.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/base/growablememory.hpp"
#include "afl/net/line/linehandler.hpp"
#include "afl/net/line/linesink.hpp"
#include "afl/sys/mutexguard.hpp"

namespace {
    class Collector : public afl::net::line::LineSink {
     public:
        Collector()
            : m_futureOutput()
            { }

        virtual void handleLine(const String_t& line)
            {
                static const uint8_t CRLF[] = {'\r','\n'};
                if (m_futureOutput.get() == 0) {
                    m_futureOutput.reset(new afl::base::GrowableBytes_t());
                }
                m_futureOutput->append(afl::string::toBytes(line));
                m_futureOutput->append(CRLF);
            }

        std::auto_ptr<afl::base::GrowableBytes_t> extract()
            { return m_futureOutput; }

     private:
        std::auto_ptr<afl::base::GrowableBytes_t> m_futureOutput;
    };
}


afl::net::line::Client::Client(NetworkStack& stack, const Name& name)
    : m_stack(stack),
      m_name(name),
      m_mutex(),
      m_controller(),
      m_socket(),
      m_buffer()
{
    connect();
}

afl::net::line::Client::~Client()
{ }

void
afl::net::line::Client::call(LineHandler& request)
{
    // Serialize
    afl::sys::MutexGuard g(m_mutex);

    // Initialize
    Collector c;
    bool end = request.handleOpening(c);
    while (1) {
        std::auto_ptr<afl::base::GrowableBytes_t> data(c.extract());
        if (data.get() != 0) {
            // We have data to send
            afl::base::ConstBytes_t d(*data);
            afl::async::CommunicationSink(m_controller, m_socket).handleData(d);
        } else if (end) {
            // Connection ends
            break;
        } else {
            // Read
            String_t line;
            while (1) {
                // If buffer is empty, re-fill
                if (m_buffer.empty()) {
                    afl::async::ReceiveOperation op(m_bytes);
                    m_socket->receive(m_controller, op, afl::sys::INFINITE_TIMEOUT);
                    m_buffer = op.getReceivedBytes();
                    if (m_buffer.empty()) {
                        if (!line.empty()) {
                            request.handleLine(line, c);
                        }
                        request.handleConnectionClose();
                        end = true;
                        break;
                    }
                }

                // Try to find a line
                // FIXME: make this a configurable character set conversion?
                line.append(afl::string::fromBytes(m_buffer.split(m_buffer.find('\n'))));

                // If m_buffer is not empty, we're looking at a '\n'.
                if (!m_buffer.empty()) {
                    // Remove the '\n'
                    m_buffer.split(1);

                    // Process the string
                    if (!line.empty() && line[line.size()-1] == '\r') {
                        line.erase(line.size()-1);
                    }
                    if (request.handleLine(line, c)) {
                        end = true;
                    }
                    break;
                }
            }
        }
    }
}

void
afl::net::line::Client::connect()
{
    m_socket = m_stack.connect(m_name).asPtr();
}
