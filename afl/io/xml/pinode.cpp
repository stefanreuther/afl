/**
  *  \file afl/io/xml/pinode.cpp
  *  \brief Class afl::io::xml::PINode
  */

#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/visitor.hpp"

afl::io::xml::PINode::PINode(const String_t& name)
    : NamedNode(name)
{ }

void
afl::io::xml::PINode::visit(Visitor& v) const
{
    v.visitPI(*this);
}
