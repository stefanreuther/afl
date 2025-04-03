/**
  *  \file afl/io/xml/tagnode.cpp
  *  \brief Class afl::io::xml::TagNode
  */

#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/visitor.hpp"

afl::io::xml::TagNode::TagNode(const String_t& name)
    : NamedNode(name),
      m_children()
{ }

afl::io::xml::TagNode::~TagNode()
{ }

void
afl::io::xml::TagNode::addNewChild(Node* pNode)
{
    if (pNode != 0) {
        m_children.pushBackNew(pNode);
    }
}

const afl::io::xml::Nodes_t&
afl::io::xml::TagNode::getChildren() const
{
    return m_children;
}

afl::io::xml::Nodes_t&
afl::io::xml::TagNode::getChildren()
{
    return m_children;
}

void
afl::io::xml::TagNode::visit(Visitor& v) const
{
    v.visitTag(*this);
}
