/**
  *  \file u/mock/commandhandlermock.cpp
  *  \class CommandHandlerMock
  */

#include "u/mock/commandhandlermock.hpp"
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

CommandHandlerMock::CommandHandlerMock()
{ }

CommandHandlerMock::Value_t*
CommandHandlerMock::call(const Segment_t& command)
{
    // Verify command by stringifying it
    TS_ASSERT(!m_parameters.empty());
    String_t s;
    for (size_t i = 0, n = command.size(); i < n; ++i) {
        if (i != 0) {
            s += ", ";
        }
        Stringifier(s).visit(command[i]);
    }
    TS_ASSERT_EQUALS(s, m_parameters.front());
    m_parameters.pop_front();
                
    TS_ASSERT(!m_results.empty());
    return m_results.extractFront();
}

void
CommandHandlerMock::callVoid(const Segment_t& command)
{
    delete call(command);
}

void
CommandHandlerMock::addParameterList(const String_t& list)
{
    m_parameters.push_back(list);
}

void
CommandHandlerMock::addNewResult(Value_t* value)
{
    m_results.pushBackNew(value);
}
