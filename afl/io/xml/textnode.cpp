/**
  *  \file afl/io/xml/textnode.cpp
  *  \brief Class afl::io::xml::TextNode
  */

#include "afl/io/xml/textnode.hpp"
#include "afl/io/xml/visitor.hpp"

// Constructor.
afl::io::xml::TextNode::TextNode()
    : Node(),
      m_value()
{ }

// Constructor.
afl::io::xml::TextNode::TextNode(const String_t& value)
    : Node(),
      m_value(value)
{ }

// Destructor.
afl::io::xml::TextNode::~TextNode()
{ }

// Set contained text.
void
afl::io::xml::TextNode::set(const String_t& newValue)
{
    m_value = newValue;
}

// Get contained text.
const String_t&
afl::io::xml::TextNode::get() const
{
    return m_value;
}

// Visit this node.
void
afl::io::xml::TextNode::visit(Visitor& v) const
{
    v.visitText(*this);
}
