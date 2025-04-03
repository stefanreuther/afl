/**
  *  \file afl/io/xml/namednode.cpp
  *  \brief Class afl::io::xml::NamedNode
  */

#include "afl/io/xml/namednode.hpp"

afl::io::xml::NamedNode::NamedNode(const String_t& name)
    : Node(),
      m_name(name),
      m_attributeNames(),
      m_attributeValues()
{ }

afl::io::xml::NamedNode::~NamedNode()
{ }

void
afl::io::xml::NamedNode::setName(const String_t& name)
{
    m_name = name;
}

const String_t&
afl::io::xml::NamedNode::getName() const
{
    return m_name;
}

void
afl::io::xml::NamedNode::setAttribute(const String_t& name, const String_t& value)
{
    afl::data::NameMap::Index_t index = m_attributeNames.addMaybe(name);
    if (m_attributeValues.size() <= index) {
        m_attributeValues.resize(index+1);
    }
    m_attributeValues[index] = value;
}

String_t
afl::io::xml::NamedNode::getAttributeByName(const String_t& name) const
{
    return getAttributeByIndex(m_attributeNames.getIndexByName(name));
}

String_t
afl::io::xml::NamedNode::getAttributeByIndex(afl::data::NameMap::Index_t index) const
{
    if (index < m_attributeValues.size()) {
        return m_attributeValues[index];
    } else {
        return String_t();
    }
}

const afl::data::NameMap&
afl::io::xml::NamedNode::getAttributeNames() const
{
    return m_attributeNames;
}
