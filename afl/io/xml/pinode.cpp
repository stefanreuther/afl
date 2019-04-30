/**
  *  \file afl/io/xml/pinode.cpp
  */

#include "afl/io/xml/pinode.hpp"
#include "afl/io/xml/visitor.hpp"

afl::io::xml::PINode::PINode(const String_t& name)
    : Node(),
      m_name(name),
      m_attributeNames(),
      m_attributeValues()
{ }

afl::io::xml::PINode::~PINode()
{ }

void
afl::io::xml::PINode::setName(const String_t& name)
{
    m_name = name;
}

const String_t&
afl::io::xml::PINode::getName() const
{
    return m_name;
}

void
afl::io::xml::PINode::setAttribute(const String_t& name, const String_t& value)
{
    afl::data::NameMap::Index_t index = m_attributeNames.addMaybe(name);
    if (m_attributeValues.size() <= index) {
        m_attributeValues.resize(index+1);
    }
    m_attributeValues[index] = value;
}

String_t
afl::io::xml::PINode::getAttributeByName(const String_t& name) const
{
    return getAttributeByIndex(m_attributeNames.getIndexByName(name));
}

String_t
afl::io::xml::PINode::getAttributeByIndex(afl::data::NameMap::Index_t index) const
{
    if (index < m_attributeValues.size()) {
        return m_attributeValues[index];
    } else {
        return String_t();
    }
}

const afl::data::NameMap&
afl::io::xml::PINode::getAttributeNames() const
{
    return m_attributeNames;
}

void
afl::io::xml::PINode::visit(Visitor& v) const
{
    v.visitPI(*this);
}
