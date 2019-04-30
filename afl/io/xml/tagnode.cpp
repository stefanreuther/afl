/**
  *  \file afl/io/xml/tagnode.cpp
  *  \brief Class afl::io::xml::TagNode
  */

#include "afl/io/xml/tagnode.hpp"
#include "afl/io/xml/visitor.hpp"

afl::io::xml::TagNode::TagNode(const String_t& name)
    : Node(),
      m_name(name),
      m_attributeNames(),
      m_attributeValues(),
      m_children()
{ }

afl::io::xml::TagNode::~TagNode()
{ }

void
afl::io::xml::TagNode::setName(const String_t& name)
{
    m_name = name;
}

const String_t&
afl::io::xml::TagNode::getName() const
{
    return m_name;
}

void
afl::io::xml::TagNode::setAttribute(const String_t& name, const String_t& value)
{
    afl::data::NameMap::Index_t index = m_attributeNames.addMaybe(name);
    if (m_attributeValues.size() <= index) {
        m_attributeValues.resize(index+1);
    }
    m_attributeValues[index] = value;
}

String_t
afl::io::xml::TagNode::getAttributeByName(const String_t& name) const
{
    return getAttributeByIndex(m_attributeNames.getIndexByName(name));
}

String_t
afl::io::xml::TagNode::getAttributeByIndex(afl::data::NameMap::Index_t index) const
{
    if (index < m_attributeValues.size()) {
        return m_attributeValues[index];
    } else {
        return String_t();
    }
}

const afl::data::NameMap&
afl::io::xml::TagNode::getAttributeNames() const
{
    return m_attributeNames;
}

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
