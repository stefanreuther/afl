/**
  *  \file afl/io/xml/tagnode.hpp
  *  \brief Class afl::io::xml::TagNode
  */
#ifndef AFL_AFL_IO_XML_TAGNODE_HPP
#define AFL_AFL_IO_XML_TAGNODE_HPP

#include "afl/io/xml/namednode.hpp"

namespace afl { namespace io { namespace xml {

    /** Tag node.
        Represents a tag with its attributes and content.

        Attributes are represented as a afl::data::NameMap / afl::data::StringList_t pair,
        to preserve the original order of attributes.

        Child nodes (content) are owned by the TagNode. */
    class TagNode : public NamedNode {
     public:
        /** Constructor.
            \param name Tag name (case-sensitive) */
        TagNode(const String_t& name);

        /** Destructor. */
        ~TagNode();

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
        /** Child nodes. */
        Nodes_t m_children;
    };

} } }

#endif
