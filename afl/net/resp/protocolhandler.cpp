/**
  *  \file afl/net/resp/protocolhandler.cpp
  *  \brief Class afl::net::resp::ProtocolHandler
  */

#include <memory>
#include "afl/net/resp/protocolhandler.hpp"
#include "afl/sys/types.hpp"
#include "afl/data/visitor.hpp"
#include "afl/io/resp/writer.hpp"
#include "afl/data/vector.hpp"
#include "afl/string/messages.hpp"

// Constructor.
afl::net::resp::ProtocolHandler::ProtocolHandler(CommandHandler& ch)
    : afl::net::ProtocolHandler(),
      m_state(Idle),
      m_ch(ch),
      m_data(),
      m_factory(),
      m_parser(m_factory)
{
    m_parser.setAcceptShortForm(true);
}

// Destructor.
afl::net::resp::ProtocolHandler::~ProtocolHandler()
{ }

// ProtocolHandler operations:
void
afl::net::resp::ProtocolHandler::getOperation(Operation& op)
{
    // Acknowledge previous data
    if (m_state == Sending) {
        m_data.popFront();
        m_state = Idle;
    }

    // Next data
    if (m_state == Idle && !m_data.empty()) {
        op.m_dataToSend = m_data.front()->getContent();
        m_state = Sending;
    } else {
        op.m_dataToSend.reset();
    }

    // Close?
    op.m_close = (m_state == Failed);

    // Timeout
    op.m_timeToWait = afl::sys::INFINITE_TIMEOUT;
}

void
afl::net::resp::ProtocolHandler::advanceTime(afl::sys::Timeout_t /*msecs*/)
{ }

void
afl::net::resp::ProtocolHandler::handleData(afl::base::ConstBytes_t bytes)
{
    try {
        while (!bytes.empty()) {
            if (m_parser.handleData(bytes)) {
                handleNewValue(m_parser.extract());
            }
        }
    }
    catch (std::exception& e) {
        // FIXME: log it
        m_state = Failed;
    }
    catch (...) {
        // FIXME: log it
        m_state = Failed;
    }
}

void
afl::net::resp::ProtocolHandler::handleSendTimeout(afl::base::ConstBytes_t /*unsentBytes*/)
{
    // FIXME: log it
    m_state = Failed;
}

void
afl::net::resp::ProtocolHandler::handleConnectionClose()
{ }

void
afl::net::resp::ProtocolHandler::handleNewValue(afl::data::Value* p)
{
    class Visitor : public afl::data::Visitor {
     public:
        Visitor(CommandHandler& ch, afl::io::resp::Writer& writer)
            : m_ch(ch),
              m_writer(writer)
            { }
        virtual void visitString(const String_t& /*str*/)
            { fail(); }
        virtual void visitInteger(int32_t /*iv*/)
            { fail(); }
        virtual void visitFloat(double /*fv*/)
            { fail(); }
        virtual void visitBoolean(bool /*bv*/)
            { fail(); }
        virtual void visitHash(const afl::data::Hash& /*hv*/)
            { fail(); }
        virtual void visitVector(const afl::data::Vector& vv)
            {
                try {
                    std::auto_ptr<afl::data::Value> result(m_ch.call(vv));
                    m_writer.visit(result.get());
                }
                catch (std::exception& e) {
                    m_writer.sendError(e.what());
                }
            }
        virtual void visitOther(const afl::data::Value& /*other*/)
            { fail(); }
        virtual void visitNull()
            { fail(); }
        virtual void visitError(const String_t& /*source*/, const String_t& /*str*/)
            { fail(); }

     private:
        CommandHandler& m_ch;
        afl::io::resp::Writer& m_writer;
        void fail()
            { m_writer.sendError(afl::string::Messages::invalidOperation()); }
    };

    // Grab hold of the value
    std::auto_ptr<afl::data::Value> pp(p);

    // Make room for output
    std::auto_ptr<afl::io::InternalSink> sink(new afl::io::InternalSink());
    afl::io::resp::Writer writer(*sink);

    // Generate output
    Visitor(m_ch, writer).visit(pp.get());

    // Stash it away
    m_data.pushBackNew(sink.release());
}
