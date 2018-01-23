/**
  *  \file afl/test/commandhandler.cpp
  */

#include "afl/test/commandhandler.hpp"
#include "afl/data/visitor.hpp"
#include "afl/string/format.hpp"

namespace {
    class Stringifier : public afl::data::Visitor {
     public:
        Stringifier(String_t& result)
            : m_result(result)
            { }
        virtual void visitString(const String_t& str)
            { m_result += str; }
        virtual void visitInteger(int32_t iv)
            { m_result += afl::string::Format("%d", iv); }
        virtual void visitFloat(double fv)
            { m_result += afl::string::Format("%.2f", fv); }
        virtual void visitBoolean(bool bv)
            { m_result += bv ? "1" : "0"; }
        virtual void visitHash(const afl::data::Hash& /*hv*/)
            { m_result += "<hash>"; }
        virtual void visitVector(const afl::data::Vector& /*vv*/)
            { m_result += "<vector>"; }
        virtual void visitOther(const afl::data::Value& /*other*/)
            { m_result += "<other>"; }
        virtual void visitNull()
            { m_result += "<null>"; }
        virtual void visitError(const String_t& /*source*/, const String_t& /*str*/)
            { m_result += "<error>"; }
     private:
        String_t& m_result;
    };
}

afl::test::CommandHandler::CommandHandler(const Assert& a)
    : m_assert(a)
{ }

afl::net::CommandHandler::Value_t*
afl::test::CommandHandler::call(const Segment_t& command)
{
    // Verify command by stringifying it
    m_assert.check("call: need pending call", !m_queue.empty());
    String_t s;
    for (size_t i = 0, n = command.size(); i < n; ++i) {
        if (i != 0) {
            s += ", ";
        }
        Stringifier(s).visit(command[i]);
    }
    m_assert.checkEqual("call", s, m_queue.front());
    m_queue.pop_front();

    m_assert.check("call: must have result", !m_results.empty());
    return m_results.extractFront();
}

void
afl::test::CommandHandler::callVoid(const Segment_t& command)
{
    delete call(command);
}

void
afl::test::CommandHandler::expectCall(const String_t& list)
{
    m_queue.push_back(list);
}

void
afl::test::CommandHandler::provideNewResult(Value_t* value)
{
    m_results.pushBackNew(value);
}

void
afl::test::CommandHandler::checkFinish()
{
    m_assert.check("checkFinish: must have no more pending calls", m_queue.empty());
    m_assert.check("checkFinish: must have no more pending returns", m_results.empty());
}
