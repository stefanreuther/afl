/**
  *  \file afl/data/visitor.cpp
  *  \brief Class afl::data::Visitor
  */

#include "afl/data/visitor.hpp"
#include "afl/data/value.hpp"

void
afl::data::Visitor::visit(const Value* value)
{
    if (value != 0) {
        value->visit(*this);
    } else {
        visitNull();
    }
}
