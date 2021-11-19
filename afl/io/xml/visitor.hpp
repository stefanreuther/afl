/**
  *  \file afl/io/xml/visitor.hpp
  *  \brief Interface afl::io::xml::Visitor
  */
#ifndef AFL_AFL_IO_XML_VISITOR_HPP
#define AFL_AFL_IO_XML_VISITOR_HPP

#include "afl/base/deletable.hpp"
#include "afl/io/xml/node.hpp"

namespace afl { namespace io { namespace xml {

    class PINode;
    class TagNode;
    class TextNode;

    /** Node visitor.
        Used to visit a single XML node instead of a type switch. */
    class Visitor : public afl::base::Deletable {
     public:
        /** Visit a processing instruction.
            \param node Processing instruction */
        virtual void visitPI(const PINode& node) = 0;

        /** Visit a tag.
            \param node Tag */
        virtual void visitTag(const TagNode& node) = 0;

        /** Visit a text node.
            \param node Node */
        virtual void visitText(const TextNode& node) = 0;

        /** Visit a node.
            This is a convenience method that calls Node::visit().
            \param node Node */
        void visit(const Node& node);

        /** Visit a node list.
            This is a convenience method that calls visit() for all elements of the list.
            \param nodes Nodes */
        void visit(const Nodes_t& nodes);
    };

} } }

#endif
