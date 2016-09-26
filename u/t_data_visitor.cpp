/**
  *  \file u/t_data_visitor.cpp
  *  \brief Test for afl::data::Visitor
  */

#include "afl/data/visitor.hpp"
#include "u/t_data.hpp"

/** Simple test.
    Test that the interface is well-formed,
    i.e. allows us to derive a class that can be successfully created. */
void
TestDataVisitor::testIt()
{
    class Tester : public afl::data::Visitor {
     public:
        virtual void visitString(const String_t& /*str*/)
            { }
        virtual void visitInteger(int32_t /*iv*/)
            { }
        virtual void visitFloat(double /*fv*/)
            { }
        virtual void visitBoolean(bool /*bv*/)
            { }
        virtual void visitHash(const afl::data::Hash& /*hv*/)
            { }
        virtual void visitVector(const afl::data::Vector& /*vv*/)
            { }
        virtual void visitOther(const afl::data::Value& /*other*/)
            { }
        virtual void visitError(const String_t& /*source*/, const String_t& /*str*/)
            { }
        virtual void visitNull()
            { }
    };
    Tester t;
    t.visit(0);
}
