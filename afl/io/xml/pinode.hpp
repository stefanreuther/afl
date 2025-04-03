/**
  *  \file afl/io/xml/pinode.hpp
  *  \brief Class afl::io::xml::PINode
  */
#ifndef AFL_AFL_IO_XML_PINODE_HPP
#define AFL_AFL_IO_XML_PINODE_HPP

#include "afl/io/xml/namednode.hpp"

namespace afl { namespace io { namespace xml {

    /** Processing instruction.
        Represents a single PI with its attributes.

        Attributes are represented as a afl::data::NameMap / afl::data::StringList_t pair,
        to preserve the original order of attributes. */
    class PINode : public NamedNode {
     public:
        /** Constructor.
            \param name PI name (case-sensitive) */
        PINode(const String_t& name);

        // Node:
        virtual void visit(Visitor& v) const;
    };

} } }

#endif
