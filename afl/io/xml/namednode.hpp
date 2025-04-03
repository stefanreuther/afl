/**
  *  \file afl/io/xml/namednode.hpp
  *  \brief Class afl::io::xml::NamedNode
  */
#ifndef AFL_AFL_IO_XML_NAMEDNODE_HPP
#define AFL_AFL_IO_XML_NAMEDNODE_HPP

#include "afl/data/namemap.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/io/xml/node.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io { namespace xml {

    /** Node with name and attributes.

        This class implements the common code of PINode and TagNode.

        Attributes are represented as a afl::data::NameMap / afl::data::StringList_t pair,
        to preserve the original order of attributes. */
    class NamedNode : public Node {
     public:
        /** Constructor.
            \param name PI name (case-sensitive) */
        NamedNode(const String_t& name);

        /** Destructor. */
        ~NamedNode();

        /** Set name.
            \param name Name (case-sensitive) */
        void setName(const String_t& name);

        /** Get name.
            \return name (case-sensitive) */
        const String_t& getName() const;

        /** Set attribute.
            If the attribute already exists, changes its value.
            Otherwise, adds it.
            \param name Attribute name (case-sensitive)
            \param value Value */
        void setAttribute(const String_t& name, const String_t& value);

        /** Get attribute value by name.
            \param name Attribute name (case-sensitive)
            \return Attribute value. Empty if not present. */
        String_t getAttributeByName(const String_t& name) const;

        /** Get attribute by index.
            \param index Index
            \return Attribute value. Empty if index out of range (attribute not present). */
        String_t getAttributeByIndex(afl::data::NameMap::Index_t index) const;

        /** Get attribute names.
            Usable to iterate through attributes.
            \return Name list */
        const afl::data::NameMap& getAttributeNames() const;

     private:
        String_t m_name;
        afl::data::NameMap m_attributeNames;
        afl::data::StringList_t m_attributeValues;
    };

} } }

#endif
