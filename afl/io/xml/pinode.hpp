/**
  *  \file afl/io/xml/pinode.hpp
  */
#ifndef AFL_AFL_IO_XML_PINODE_HPP
#define AFL_AFL_IO_XML_PINODE_HPP

#include "afl/io/xml/node.hpp"
#include "afl/string/string.hpp"
#include "afl/data/namemap.hpp"
#include "afl/data/stringlist.hpp"

namespace afl { namespace io { namespace xml {

    /** Processing instruction.
        Represents a single PI with its attributes.

        Attributes are represented as a afl::data::NameMap / afl::data::StringList_t pair,
        to preserve the original order of attributes. */
    class PINode : public Node {
     public:
        /** Constructor.
            \param name PI name (case-sensitive) */
        PINode(const String_t& name);

        /** Destructor. */
        ~PINode();

        /** Set PI name.
            \param name Name (case-sensitive) */
        void setName(const String_t& name);

        /** Get tag name.
            \return name (case-sensitive) */
        const String_t& getName() const;

        /** Set attribute.
            If the attribute already exists, changes its name.
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

        // Node:
        virtual void visit(Visitor& v) const;

     private:
        String_t m_name;

        afl::data::NameMap m_attributeNames;

        afl::data::StringList_t m_attributeValues;
    };

} } }

#endif
