/**
  *  \file afl/io/xml/node.hpp
  *  \brief Interface afl::io::xml::Node
  */
#ifndef AFL_AFL_IO_XML_NODE_HPP
#define AFL_AFL_IO_XML_NODE_HPP

#include "afl/base/deletable.hpp"
#include "afl/container/ptrvector.hpp"

namespace afl { namespace io { namespace xml {

    class Visitor;
    class BaseReader;

    /** XML DOM node.
        Represents a node in a XML document. */
    class Node : public afl::base::Deletable {
     public:
        /** Visit this node.
            Calls the appropriate visit method.
            \param v Visitor */
        virtual void visit(Visitor& v) const = 0;
    };

    /** Array of nodes.
        Used to store and own a list of nodes. */
    typedef afl::container::PtrVector<Node> Nodes_t;

} } }

#endif
