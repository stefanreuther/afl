/**
  *  \file afl/io/xml/tagnode.hpp
  *  \brief Class afl::io::xml::TagNode
  */
#ifndef AFL_AFL_IO_XML_TAGNODE_HPP
#define AFL_AFL_IO_XML_TAGNODE_HPP

#include "afl/data/namemap.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/io/xml/node.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io { namespace xml {

    /** Tag node.
        Represents a tag with its attributes and content.

        Attributes are represented as a afl::data::NameMap / afl::data::StringList_t pair,
        to preserve the original order of attributes.

        Child nodes (content) are owned by the TagNode. */
    class TagNode : public Node {
     public:
        /** Constructor.
            \param name Tag name (case-sensitive) */
        TagNode(const String_t& name);

        /** Destructor. */
        ~TagNode();

        /** Set tag name.
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

        /** Add new child (content) node.
            \param pNode New child. Must not be null. Will become owned by this TagNode. */
        void addNewChild(Node* pNode);

        /** Access child (content) nodes.
            All contained nodes are owned by this TagNode.
            \return Child nodes */
        const Nodes_t& getChildren() const;

        /** Access child (content) nodes.
            All contained nodes are owned by this TagNode.
            \return Child nodes */
        Nodes_t& getChildren();

        // Node:
        virtual void visit(Visitor& v) const;

     private:
        /** Tag name. */
        String_t m_name;

        /** Attribute names. Maps names to indexes. */
        afl::data::NameMap m_attributeNames;

        /** Attribute values. Indexes provided by m_attributeNames. */
        afl::data::StringList_t m_attributeValues;

        /** Child nodes. */
        Nodes_t m_children;
    };

} } }

#endif
